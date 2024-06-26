#include "ImGuiFileSystem.h"

#include <imgui.h>

void ImGuiFileSystem::Init(const std::string &parentDirectory) {
	parentDirectory_ = parentDirectory;
}

void ImGuiFileSystem::Search(const std::string &extension) {
	for(const auto &entry : std::filesystem::recursive_directory_iterator(parentDirectory_)) {
		if(entry.is_regular_file() && entry.path().extension() == ('.' + extension)) {
			fileList_.push_back({entry.path().parent_path().string(),entry.path().filename().stem().string()});
		}
	}
}