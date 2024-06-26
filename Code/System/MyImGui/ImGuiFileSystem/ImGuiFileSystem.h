#pragma once

#include <vector>

#include <string>

#include <filesystem>

class ImGuiFileSystem {
public:
	void Init(const std::string &parentDirectory);
	void Search(const std::string &extension);
private:
	std::string parentDirectory_;
	/// <summary>
	/// first = directoryPath, seconds = fileName
	/// </summary>
	std::vector<std::pair<std::string,std::string>> fileList_;
public:
	const std::vector<std::pair<std::string,std::string>> &getFileList()const { return fileList_; }
};