#pragma once

#include <list>

#include <string>

#include <filesystem>

class MyFileSystem {
public:
	static std::list<std::pair<std::string,std::string>> SearchFile(const std::string &directory,const std::string &extension);
	static std::list<std::string> SearchSubFolder(const std::string &directory);

	static bool CreateFolder(const std::string &directory);

	static bool removeEmptyFolder(const std::string &directory);
	static std::uintmax_t deleteFolder(const std::string &path);
};

using myfs = MyFileSystem;
using myFs = MyFileSystem;