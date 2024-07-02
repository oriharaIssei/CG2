#include "ChipEditState.h"
#include "AllViewState.h"

#include "imgui.h"

ChipEditState::ChipEditState(MapEditor *host,std::pair<uint32_t,uint32_t> address):IMapEditState(host) {
	chip_ = host_->getChipList()[address.first][address.second].get();
	fileSystem_ = std::make_unique<ImGuiFileSystem>();
	fileSystem_->Init("./resource");
	fileSystem_->Search("obj");
}

void ChipEditState::Update() {
	ImGui::Begin("ModelList");
	if(ImGui::Button("Reload List")) {
		fileSystem_->Search("obj");
	}

	for(auto &fileName : fileSystem_->getFileList()) {
		if(ImGui::Button(fileName.second.c_str())) {
			std::unique_ptr<GameObject> object = std::make_unique<GameObject>();
			object->Init(fileName.second + ".obj",fileName.first,"WHITE",host_->getMaterialManager(),{{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},chip_->transform_.transformData.translate});
			chip_->gameObjects_.push_back(std::move(object));
		}
	}
	ImGui::End();

	ImGui::Begin("Active Objects");
	if(!chip_->gameObjects_.empty()) {
		objectNames_.clear();
		for(int i = 0; i < chip_->gameObjects_.size(); ++i) {
			objectNames_.push_back(std::to_string(i) + '.' + chip_->gameObjects_[i]->getModel()->getName());
		}


		ImGui::BeginChild("Objects",ImVec2(200,100),true);
		for(int i = 0; i < objectNames_.size(); ++i) {
			if(ImGui::Selectable(objectNames_[i].c_str(),selectedObject_ == i)) {
				selectedObject_ = i;
			}
		}
		ImGui::EndChild();

		chip_->gameObjects_[selectedObject_]->DebugUpdate(host_->getMaterialManager());
		if(ImGui::Button("Delete Current Ojbect")) {
			chip_->gameObjects_[selectedObject_].reset();
			chip_->gameObjects_.erase(chip_->gameObjects_.begin() + selectedObject_);
		}
	}
	ImGui::End();

	if(ImGui::Button("Back To AllViewState")) {
		host_->TransitionState(new AllViewState(host_));
	}
}

void ChipEditState::Draw(const ViewProjection &viewProj) {
	chip_->Draw(viewProj);
}