#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <format>
#include <vector>


namespace fs = std::filesystem;

class PathWrapper {
  public:
	explicit PathWrapper(const std::string &path) : path_(path) {}
	explicit PathWrapper(const fs::path &p) : path_(p) {}

	// 返回路径字符串（对应 Python 的 str(path)）
	std::string string() const { return path_.string(); }

	// 返回路径字符串（对应 Python 的 repr(path)）
	std::string repr() const { return std::format("Path('{}')", path_.string()); }

	// 获取父路径（对应 path.parent()）
	PathWrapper parent() const { return PathWrapper(path_.parent_path()); }

	// 获取文件名（含扩展名）（对应 path.name）
	std::string name() const { return path_.filename().string(); }

	// 获取文件名（不含扩展名）（对应 path.stem）
	std::string stem() const { return path_.stem().string(); }

	// 获取扩展名（对应 path.suffix）
	std::string suffix() const { return path_.extension().string(); }

	// 判断路径是否存在（对应 path.exists()）
	bool exists() const { return fs::exists(path_); }

	// 判断是否为目录（对应 path.is_dir()）
	bool is_dir() const { return fs::is_directory(path_); }

	// 判断是否为文件（对应 path.is_file()）
	bool is_file() const { return fs::is_regular_file(path_); }

	// 返回绝对路径（对应 path.absolute()）
	std::string absolute() const { return fs::absolute(path_).string(); }

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
