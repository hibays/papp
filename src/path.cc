#include "pybind11/pybind11.h"
namespace py = pybind11;

#include "path.hh"

// 添加 pathlib 子模块 Path
PYBIND11_EMBEDDED_MODULE(pathlib, m) {
	py::class_<PathWrapper>(m, "Path")
		.def(py::init<const std::string &>())				   // 构造函数
		.def(py::init<const fs::path &>())					   // 构造函数
		.def("__str__", &PathWrapper::string)				   // str(path)
		.def("__repr__", &PathWrapper::repr)				   // repr(path)
		.def("__truediv__", &PathWrapper::operator/)		   // path / other
		.def_property_readonly("name", &PathWrapper::name)	   // path.name
		.def_property_readonly("stem", &PathWrapper::stem)	   // path.stem
		.def_property_readonly("suffix", &PathWrapper::suffix) // path.suffix
		.def_property_readonly("parent", &PathWrapper::parent) // path.parent
		.def("exists", &PathWrapper::exists)				   // path.exists()
		.def("is_dir", &PathWrapper::is_dir)				   // path.is_dir()
		.def("is_file", &PathWrapper::is_file)				   // path.is_file()
		.def("absolute", &PathWrapper::absolute)			   // path.absolute()
		.def("joinpath", &PathWrapper::joinpath)			   // path.joinpath(other)
		.def("read_text", &PathWrapper::read_text)			   // path.read_text()
		.def("write_text", &PathWrapper::write_text)		   // path.write_text(content)
		.def_static("cwd", &PathWrapper::cwd);				   // Path.cwd()
}