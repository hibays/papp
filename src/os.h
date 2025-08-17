#include "pybind11/pybind11.h" // IWYU pragma: keep
namespace py = pybind11;

#include <filesystem>
#include <format>

#include <windows.h>

typedef std::filesystem::path Path;

// Init os
inline int init_os() {
	auto m = py::module_::import("os");
	if (m.is_none()) {
		py::eval("raise ImportError('os module not found')");
		return 1;
	}

	m.attr("curdir") = py::str(std::filesystem::current_path().string());

	m.def("getcwd", []() { return py::str(std::filesystem::current_path().string()); });

	m.def("chdir", [](const std::string &path) {
		std::error_code ec;
		std::filesystem::current_path(path, ec);
		if (ec) {
			py::eval(std::format("raise OSError('{}')", ec.message()));
		}
	});

	m.def("system", [](const char *cmd) {
		auto code = system(cmd);
		return code;
	});

	m.def("getenv", [](const char *name) {
		auto size = GetEnvironmentVariable(name, NULL, 0);
		if (size == 0)
			py::eval("raise KeyError('Given name not found.')");

		std::vector<char> buffer(size);
		GetEnvironmentVariable(name, buffer.data(), size);

		return py::str(buffer.data(), size - 1);
	});

	// Warn: 这里的 environ 行为与 CPython 不同，CPython 修改后会作用到当前进程，但是此实现不会有影响
	m.attr("environ") = []() {
		auto raw = GetEnvironmentStrings();
		if (!raw) {
			py::eval("raise OSError('GetEnvironmentStrings failed')");
		}

		py::dict env;

		for (const char *p = raw; *p; ++p) {
			const char *eq = p;
			while (*eq && *eq != '=')
				++eq;					 // 找到 '=' 或结尾
			if (*eq == '=' && eq != p) { // 有正常 key
				auto key = py::str(p, eq - p);
				auto val = py::str(eq + 1);
				env[key] = val;
			}
			// 跳到下一字符串
			while (*p)
				++p;
		}

		FreeEnvironmentStrings(raw);
		return env;
	}();

	auto os_path = m.def_submodule("path");

	os_path.def("dirname", [](const char *path) {
		auto parent = Path(path).parent_path().string();
		return py::str(parent);
	});

	os_path.def("join", [](const char *path, py::args args) {
		auto parent = Path(path);
		for (auto &&arg : args) {
			parent.append(arg.cast<std::string>());
		}
		return py::str(parent.string());
	});

	return 0;
}