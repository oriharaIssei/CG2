#include "ChipEditState.h"
#include "AllViewState.h"

#include "imgui.h"

ChipEditState::ChipEditState(MapEditor *host,std::pair<uint32_t,uint32_t> address):IMapEditState(host) {
	chip = host_->getChipList()[address.second][address.first].get();
	fileSystem_ = std::make_unique<ImGuiFileSystem>();
	fileSystem_->Init("./resource");
}

void ChipEditState::Update() {
	ImGui::Begin("ModelList");
	if(ImGui::Button("Reload List")) {
		fileSystem_->Search("obj");
	}

	for(auto &fileName : fileSystem_->getFileList()) {
		if(ImGui::Button(fileName.second.c_str())) {
			std::unique_ptr<GameObject> object = std::make_unique<GameObject>();
			object->Init(fileName.second,fileName.first,"WHITE",{{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}});
			object->setTransformParent(&chip->transform_);
			chip->gameObjects_.push_back(std::move(object));
		}
	}
	ImGui::End();
	if(ImGui::TreeNode("Objects")) {
		ImGui::TreePop();
		uint32_t listNumber = 0;
		for(auto &object : chip->gameObjects_) {
			std::string nodeName = std::to_string(listNumber++) + '.' + object->getModel()->getName();
			ImGui::Checkbox(nodeName.c_str(),&isActiveEditWindow[object.get()]);
		}
	}

	uint32_t windowNumber = 0;
	for(auto &isActiveEdit : isActiveEditWindow) {
		std::string nodeName = std::to_string(windowNumber++) + '.' + isActiveEdit.first->getModel()->getName();
		if(isActiveEdit.second) {
			ImGui::Begin(nodeName.c_str());
			isActiveEdit.first->DebugUpdate(host_->getMaterialManager());
			ImGui::End();
		}
	}

	if(ImGui::Button("Back To AllViewState")) {
		host_->TransitionState(new AllViewState(host_));
	}
}

void ChipEditState::Draw(const ViewProjection &viewProj) {
	chip->Draw(viewProj);
}