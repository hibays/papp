#include <stdio.h>

#include <cstdlib>
#include <filesystem>
#include <format>
#include <string>

#include "pybind11/pybind11.h"
namespace py = pybind11;

#include <windows.h>
#include <winnt.h>

#include "get_pysrc.h"

typedef std::filesystem::path Path;

int main(int argc, char *argv[]) {
	py::scoped_interpreter guard{};

	// Set main argv
	py::list argv_;
	for (short i = 1; i < argc; ++i)
		argv_.append(py::str(argv[i]));
	py::module_::import("sys").attr("argv") = std::move(argv_);

	// Init os
	auto os = py::module_::import("os");

	os.attr("curdir") = py::str(std::filesystem::current_path().string());

	// TODO: Change to pybind11 expression
	py::vm->bind(py::vm->_modules["os"]->attr("path"), "dirname(path: str)-> bool",
				 [](pkpy::VM *vm, pkpy::ArgsView args) {
					 const char *path = py_cast<pkpy::CString>(vm, args[0]);
					 auto parent = Path(path).parent_path().string();
					 return VAR(pkpy::Str(parent));
				 });

	os.def("system", [](const char *cmd) {
		auto code = system(cmd);
		return code;
	});

	os.def("getenv", [](const char *name) {
		auto res = getenv(name);
		if (res == NULL)
			py::eval("raise KeyError('Given name not found.')");
		return py::str(res);
	});

	// main
	auto py_src_path = Path(_pgmptr).parent_path().append("src").append("main.py").string();

#ifndef __EMBED_PY_SRC
	auto _py_code_opt = get_py_src(py_src_path);
	if (!_py_code_opt.has_value())
		return 1;
	auto py_code = _py_code_opt.value();
#endif

	py::vm->exec(py_code, py_src_path, pkpy::EXEC_MODE);

	return 0;
}

// Init pocketpy runtime
PYBIND11_EMBEDDED_MODULE(launcher, m) {
	m.attr("__exec_path__") = py::str(_pgmptr);
	m.attr("__launcher_dir__") = py::str(Path(_pgmptr).parent_path().string());
	m.attr("__runtime_dir__") = py::str(Path(_pgmptr).parent_path().append("runtime").string());

	m.def("create_directories", [](const char *dir) {
		auto code = std::filesystem::create_directories(dir);
		return py::bool_(code);
	});

	m.def("create_directory_symlink", [](const char *target, const char *link) {
		std::error_code ec;
		std::filesystem::create_directory_symlink(target, link, ec);
		return ec.value();
	});

	m.def("setenv", [](const char *name, const char *value) {
		auto code = putenv(std::format("{}={}", name, value).c_str());
		return code;
	});

	m.def("stimu_pipe_run", [](const char *app_abs_path, const char *app_args) {
		// track use CreateProcess (argv[0] = app_args[0], not app_abs_path)
		std::string result;

		std::string extpath = app_abs_path;
		if (!std::filesystem::is_regular_file(extpath)) {
			for (auto suf : {".exe", ".com", ".bat", ".cmd"}) {
				if (std::filesystem::is_regular_file(extpath + suf))
					extpath += suf;
			}
		}

		SECURITY_ATTRIBUTES sa;
		HANDLE hRead, hWrite;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE; // 使子进程继承此句柄

		if (!CreatePipe(&hRead, &hWrite, &sa, 0)) { // 创建匿名管道
			py::eval(std::format("raise RuntimeError('Creating pipe failed: {} {}')", app_abs_path, app_args));
		}

		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESTDHANDLES; // 使用标准输入输出句柄，并隐藏窗口
		si.wShowWindow = SW_HIDE;
		si.hStdOutput = hWrite; // 将标准输出重定向到管道写端
		si.hStdError = hWrite;	// 将标准错误重定向到管道写端

		// 启动子进程
		if (!CreateProcess(extpath.c_str(), (char *)app_args, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si,
						   &pi)) {
			CloseHandle(hWrite);
			CloseHandle(hRead);
			py::eval(std::format("raise RuntimeError('Executing command failed: {} {}')", app_abs_path, app_args));
		}

		CloseHandle(hWrite);

		for (DWORD dwBytesRead = 0;;) {
			char tmp[257] = {0}; // 用于存储命令行输出
			if (!ReadFile(hRead, tmp, 256, &dwBytesRead, NULL) || !dwBytesRead)
				break;
			result += tmp;
		}

		CloseHandle(hRead);

		// 终止子进程
		CloseHandle(pi.hThread);
		TerminateProcess(pi.hProcess, 0);
		CloseHandle(pi.hProcess);

		return py::str(result);
	});

	m.def(
		"shell_exec",
		[](const char *cmd, const bool runas) {
			SHELLEXECUTEINFO sei = {sizeof(SHELLEXECUTEINFO)};
			if (runas)
				sei.lpVerb = "runas";
			sei.lpFile = cmd;
			sei.nShow = SW_SHOWNORMAL; // without this,the windows will be hiden
			if (!ShellExecuteEx(&sei)) {
				DWORD dwStatus = GetLastError();
				if (dwStatus == ERROR_CANCELLED) {
					puts("RunAS rejected by user.");
					return py::bool_(false);
				} else if (dwStatus == ERROR_FILE_NOT_FOUND) {
					return py::bool_(false);
				}
			}

			return py::bool_(true);
		},
		py::arg("cmd"), py::arg("runas") = py::bool_(false));

	m.def("msgbox", [](const char *title, const char *text) {
		auto code = MessageBox(NULL, text, title, MB_OK);

		return code;
	});
}