#pragma once

#include <list>

#include <string>

#include <filesystem>

class MyFileSystem {
public:
	std::list<std::pair<std::string,std::string>> SearchFile(const std::string &directory,const std::string &extension);
	std::list<std::string> SearchSubFolder(const std::string &directory);
	bool removeEmptyFolder(const std::string &directory);
	std::uintmax_t deleteFolder(const std::string &path);
};