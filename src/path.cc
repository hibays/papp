#include "pybind11/internal/builtins.h"
#include "pybind11/pybind11.h" // IWYU pragma: keep
namespace py = pybind11;

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class PathWrapper {
  public:
	explicit PathWrapper(const char *path) : path_(path) {}
	explicit PathWrapper(const std::string &path) : path_(path) {}
	explicit PathWrapper(const fs::path &path) : path_(path) {}

	// 返回路径字符串（对应 Python 的 str(path)）
	std::string string() const { return path_.string(); }

	// 返回路径字符串（对应 Python 的 repr(path)）
	std::string repr() const { return "Path('" + path_.string() + "')"; }

	// 获取父路径（对应 path.parent()）
	PathWrapper parent() const { return PathWrapper(path_.parent_path()); }

	// 获取文件名（含扩展名）（对应 path.name）
	std::string name() const { return path_.filename().string(); }

	// 获取文件名（不含扩展名）（对应 path.stem）
	std::string stem() const { return path_.stem().string(); }

	// 获取扩展名（对应 path.suffix）
	std::string suffix() const { return path_.extension().string(); }

	// 获取驱动器（对应 path.drive）
	std::string drive() const { return path_.root_name().string(); }

	// 获取路径的根目录（对应 path.root）
	std::string root() const { return path_.root_directory().string(); }

	// 判断路径是否存在（对应 path.exists()）
	bool exists() const { return fs::exists(path_); }

	// 判断是否为目录（对应 path.is_dir()）
	bool is_dir() const { return fs::is_directory(path_); }

	// 判断是否为文件（对应 path.is_file()）
	bool is_file() const { return fs::is_regular_file(path_); }

	// 返回绝对路径（对应 path.absolute()）
	PathWrapper absolute() const { return PathWrapper(fs::absolute(path_)); }

	std::vector<std::string> parts() const {
		std::vector<std::string> parts;
		for (auto &p : path_)
			parts.push_back(p.string());
		return parts;
	}

	PathWrapper joinpath(const std::string &other) const { return PathWrapper(path_ / other); }

	// 读取文件内容（对应 path.read_text()）
	std::string read_text() const {
		if (!fs::exists(path_))
			throw std::runtime_error("File not found: " + path_.string());

		std::ifstream file(path_);
		if (!file.is_open())
			throw std::runtime_error("Failed to open file: " + path_.string());

		return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	}

	// 写入文件内容（对应 path.write_text(content)）
	void write_text(const std::string &content) const {
		std::ofstream file(path_);
		if (!file.is_open())
			throw std::runtime_error("Failed to open file for writing: " + path_.string());

		file << content;
	}

	// 路径拼接（对应 path / other）
	PathWrapper operator/(const std::string &other) const { return PathWrapper(path_ / other); }

	// 相等判断（对应 path == other）
	bool operator==(const PathWrapper &other) const { return path_ == other.path_; }
	// 不相等判断（对应 path != other）
	bool operator!=(const PathWrapper &other) const { return path_ != other.path_; }

	// 静态方法：返回当前工作目录（对应 Path.cwd()）
	static PathWrapper cwd() { return PathWrapper(fs::current_path()); }

  private:
	fs::path path_;
};

// 添加 pathlib 子模块 Path
PYBIND11_EMBEDDED_MODULE(pathlib, m) {
	py::class_<PathWrapper>(m, "Path")
		.def(py::init<const char *>())										 // 构造函数
		.def(py::init<const std::string &>())								 // 构造函数
		.def(py::init<const fs::path &>())									 // 构造函数
		.def("__str__", &PathWrapper::string)								 // str(path)
		.def("__repr__", &PathWrapper::repr)								 // repr(path)
		.def("__truediv__", &PathWrapper::operator/)						 // path / other
		.def("__eq__", &PathWrapper::operator==)							 // path == other
		.def("__ne__", &PathWrapper::operator!=)							 // path != other
		.def_property_readonly("name", &PathWrapper::name)					 // path.name
		.def_property_readonly("stem", &PathWrapper::stem)					 // path.stem
		.def_property_readonly("suffix", &PathWrapper::suffix)				 // path.suffix
		.def_property_readonly("drive", &PathWrapper::drive)				 // path.drive
		.def_property_readonly("root", &PathWrapper::root)					 // path.root
		.def("_parent", &PathWrapper::parent, py::return_value_policy::move) // path.parent()
		.def_property_readonly("parent",
							   [](const PathWrapper &self) {
								   auto py_self = py::cast(self);
								   return py_self.attr("_parent")(); // 调用 _parent() 方法
							   })
		.def("exists", &PathWrapper::exists)									// path.exists()
		.def("is_dir", &PathWrapper::is_dir)									// path.is_dir()
		.def("is_file", &PathWrapper::is_file)									// path.is_file()
		.def("absolute", &PathWrapper::absolute, py::return_value_policy::move) // path.absolute()
		.def("joinpath", &PathWrapper::joinpath, py::return_value_policy::move) // path.joinpath(other)
		.def("read_text", &PathWrapper::read_text)								// path.read_text()
		.def("write_text", &PathWrapper::write_text)							// path.write_text(content)
		.def_static("cwd", &PathWrapper::cwd)									// Path.cwd()
		.def_property_readonly("parts", [](const PathWrapper &self) {
			py::list parts;
			for (auto &p : self.parts())
				parts.append(py::str(p));
			return parts;
		}); // Path.parts()
}