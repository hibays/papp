#include "pybind11/pybind11.h"
namespace py = pybind11;

#include <filesystem>
#include <format>

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
		auto res = getenv(name);
		if (res == NULL)
			py::eval("raise KeyError('Given name not found.')");
		return py::str(res);
	});

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