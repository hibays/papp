#include "pybind11/pybind11.h"

#include "get_pysrc.h"

#include "utils.hh"

#include "os.hh"

int main(int argc, char *argv[]) {
	pybind11::scoped_interpreter guard{};

	// Set main argvs
	py_sys_setargv(argc, argv);

	// init os
	init_os();

	// main
	auto py_src_path = Path(program_location()).parent_path().append("src").append("main.py").string();

#ifndef __EMBED_PY_SRC
	auto _py_code_opt = get_py_src(py_src_path);
	if (!_py_code_opt.has_value())
		return 1;
	auto py_code = _py_code_opt.value();
#endif

	auto ok = py_exec(py_code.c_str(), py_src_path.c_str(), EXEC_MODE, NULL);
	if (!ok) {
		py_printexc();
		return 1;
	}

	return 0;
}
