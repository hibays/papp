#include <stdio.h>
#include <windows.h>
#include <winnt.h>

#include <cstdlib>
#include <filesystem>
#include <format>
#include <string>

#include "get_pysrc.h"
#include "pocketpy.h"  // IWYU pragma: keep

using namespace pkpy;

typedef std::filesystem::path Path;

auto Init_PkpyVM(int argc, char *argv[]) {
	VM *vm = new VM();

	vm->set_main_argv(argc - 1, argv + 1);

	// Init launcher
	auto lun_m = vm->new_module("launcher");

	vm->setattr(lun_m, "__exec_path__", VAR(Str(_pgmptr)));
	vm->setattr(lun_m, "__launcher_dir__",
				VAR(Str(Path(_pgmptr).parent_path().string())));
	vm->setattr(
		lun_m, "__runtime_dir__",
		VAR(Str(Path(_pgmptr).parent_path().append("runtime").string())));

	vm->bind(lun_m, "setenv(name: str, value: str)-> int",
			 [](VM *vm, ArgsView args) {
				 const char *name = py_cast<CString>(vm, args[0]),
							*value = py_cast<CString>(vm, args[1]);
				 int code = putenv(std::format("{}={}", name, value).c_str());
				 return VAR(code);
			 });

	vm->bind(
		lun_m, "stimu_pipe_run(app_abs_path: str, app_args: str)-> str",
		[](VM *vm, ArgsView args) {
			// track use CreateProcess (argv[0] = app_args[0], not app_abs_path)
			const char *app_abs_path = py_cast<CString>(vm, args[0]),
					   *app_args = py_cast<CString>(vm, args[1]);
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
			sa.bInheritHandle = TRUE;  // 使子进程继承此句柄

			if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {	 // 创建匿名管道
				vm->RuntimeError(std::format("Creating pipe failed: {} {}",
											 app_abs_path, app_args));
			}

			STARTUPINFO si;
			PROCESS_INFORMATION pi;

			memset(&si, 0, sizeof(si));
			si.cb = sizeof(si);
			si.dwFlags =
				STARTF_USESTDHANDLES;  // 使用标准输入输出句柄，并隐藏窗口
			si.wShowWindow = SW_HIDE;
			si.hStdOutput = hWrite;	 // 将标准输出重定向到管道写端
			si.hStdError = hWrite;	 // 将标准错误重定向到管道写端

			// 启动子进程
			if (!CreateProcess(extpath.c_str(), (char *)app_args, NULL, NULL,
							   TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
				CloseHandle(hWrite);
				CloseHandle(hRead);
				vm->RuntimeError(std::format("Executing command failed: {} {}",
											 app_abs_path, app_args));
			}

			CloseHandle(hWrite);

			for (DWORD dwBytesRead = 0;;) {
				char tmp[257] = {0};  // 用于存储命令行输出
				if (!ReadFile(hRead, tmp, 256, &dwBytesRead, NULL) ||
					!dwBytesRead)
					break;
				result += tmp;
			}

			CloseHandle(hRead);

			// 终止子进程
			CloseHandle(pi.hThread);
			TerminateProcess(pi.hProcess, 0);
			CloseHandle(pi.hProcess);

			return VAR(Str(result));
		});

	vm->bind(lun_m, "shell_exec(cmd: str, runas: bool=False)-> bool",
			 [](VM *vm, ArgsView args) {
				 const char *cmd = py_cast<CString>(vm, args[0]);
				 const auto runas = py_cast<bool>(vm, args[1]);

				 SHELLEXECUTEINFO sei = {sizeof(SHELLEXECUTEINFO)};
				 if (runas) sei.lpVerb = "runas";
				 sei.lpFile = cmd;
				 sei.nShow =
					 SW_SHOWNORMAL;	 // without this,the windows will be hiden
				 if (!ShellExecuteEx(&sei)) {
					 DWORD dwStatus = GetLastError();
					 if (dwStatus == ERROR_CANCELLED) {
						 puts("RunAS rejected by user.");
						 return VAR(false);
					 } else if (dwStatus == ERROR_FILE_NOT_FOUND) {
						 return VAR(false);
					 }
				 }

				 return VAR(true);
			 });

	vm->bind(lun_m, "msgbox(title: str, text: str)-> int",
			 [](VM *vm, ArgsView args) {
				 const char *title = py_cast<CString>(vm, args[0]),
							*text = py_cast<CString>(vm, args[1]);

				 auto code = MessageBox(NULL, text, title, MB_OK);

				 return VAR(code);
			 });

	// Init os
	vm->setattr(vm->_modules["os"], "curdir",
				VAR(Str(std::filesystem::current_path().string())));

	vm->bind(vm->_modules["os"]->attr("path"), "dirname(path: str)-> str",
			 [](VM *vm, ArgsView args) {
				 const char *path = py_cast<CString>(vm, args[0]);
				 auto parent = Path(path).parent_path().string();
				 return VAR(Str(parent));
			 });

	vm->bind(vm->_modules["os"], "system(cmd: str)-> int",
			 [](VM *vm, ArgsView args) {
				 const char *cmd = py_cast<CString>(vm, args[0]);
				 int code = system(cmd);
				 return VAR(code);
			 });

	vm->bind(vm->_modules["os"], "getenv(name: str)-> str",
			 [](VM *vm, ArgsView args) {
				 const char *name = py_cast<CString>(vm, args[0]);
				 auto res = getenv(name);
				 if (res == NULL) vm->KeyError(VAR("Given name not found."));
				 return VAR(Str(res));
			 });

	return vm;
}

int main(int argc, char *argv[]) {
	auto vm = Init_PkpyVM(argc, argv);
	auto py_src_path =
		Path(_pgmptr).parent_path().append("src").append("main.py").string();

#ifndef __EMBED_PY_SRC
	auto _py_code_opt = get_py_src(py_src_path);
	if (!_py_code_opt.has_value()) return 1;
	auto py_code = _py_code_opt.value();
#endif

	vm->exec(py_code, py_src_path, pkpy::EXEC_MODE);

	delete vm;
	return 0;
}