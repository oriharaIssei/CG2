#include "MapEditor.h"

#include "AllViewState.h"

#include <imgui.h>
#include "PrimitiveDrawer.h"
#include "System.h"
#include "filesystem"

#include <iostream>

#include <shobjidl.h>
#include <shlwapi.h>

Vector2 MapEditor::EditChip::size;

void MapEditor::Init() {
	chips_.resize(1);
	chips_[0].push_back(std::make_unique<EditChip>());
	EditChip::size = {5.0f,5.0f};
	chips_[0].back()->Init({0,0});

	currentState_ = std::make_unique <AllViewState>(this);
}

void MapEditor::Update() {
	ImGui::Begin("MapEditor",nullptr,ImGuiWindowFlags_MenuBar);
	if(ImGui::BeginMenuBar()) {
		if(ImGui::BeginMenu("File")) {
			if(ImGui::MenuItem("save")) {
				SaveToFile();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	currentState_->Update();

	ImGui::End();
}

void MapEditor::SaveToFile() {
	std::string saveFolderPath= "./resource/Map";

	std::filesystem::create_directories(saveFolderPath);

	std::ofstream mapFile(saveFolderPath + "mapInformation.txt"); // ファイルを作成/開く
	if(mapFile.is_open()) {
		mapFile << "ChipSize ";
		mapFile << std::to_string(MapEditor::EditChip::size.x) + ',' + std::to_string(MapEditor::EditChip::size.y) + '\n';
		mapFile << "MaxAddress ";
		mapFile << std::to_string(chips_[0].size()) + ',' + std::to_string(chips_.size()) + '\n';
	}

	for(size_t col = 0; col < chips_.size(); ++col) {
		std::string colPath = saveFolderPath + "/" + std::to_string(col);

		std::filesystem::create_directories(colPath);

		for(size_t row = 0; row < chips_[col].size(); row++) {
			std::ofstream mapChipFile(colPath + '/' + std::to_string(row) + ".area");
			assert(mapChipFile.is_open());
			for(auto &mapObj : chips_[col][row]->gameObjects_) {
				auto &transform = mapObj->getWorldTransform();

				mapChipFile << "CreateModel\n";
				mapChipFile << "scale=" + std::to_string(transform.transformData.scale.x) + ","
					+ std::to_string(transform.transformData.scale.y) + ","
					+ std::to_string(transform.transformData.scale.z) + '\n';
				mapChipFile << "rotate=" + std::to_string(transform.transformData.rotate.x) + ","
					+ std::to_string(transform.transformData.rotate.y) + ","
					+ std::to_string(transform.transformData.rotate.z) + '\n';
				mapChipFile << "translate=" + std::to_string(transform.transformData.translate.x) + ","
					+ std::to_string(transform.transformData.translate.y) + ","
					+ std::to_string(transform.transformData.translate.z) + '\n';
				mapChipFile << "modelDirectoryPath=" + mapObj->getModel()->getDirectory() + '\n';
				mapChipFile << "modelName=" + mapObj->getModel()->getName() + '\n';
				mapChipFile << "end\n";
			}
		}
	}
}

void MapEditor::Draw(const ViewProjection &viewProj) {
	currentState_->Draw(viewProj);
}

void MapEditor::TransitionState(IMapEditState *nextState) {
	currentState_.reset();
	currentState_ = std::unique_ptr<IMapEditState>(nextState);
}

void MapEditor::EditChip::Init(std::pair<uint32_t,uint32_t> address) {
	address_ = address;
	transform_.Init();
	transform_.transformData.translate = {static_cast<float>(address.first * size.x),0.0f,static_cast<float>(address.second * size.y)};
	transform_.Update();

	floorMaterial_ = System::getInstance()->getMaterialManager()->Create("Ground");
}

void MapEditor::EditChip::Draw(const ViewProjection &viewProj) {
	transform_.transformData.translate = {static_cast<float>(address_.first * size.x),0.0f,static_cast<float>(address_.second * size.y)};
	transform_.Update();

	PrimitiveDrawer::Quad(
		{0.0f,0.0f,size.y},
		{size.x,0.0f,size.y},
		{0.0f,0.0f,0},
		{size.x,0.0f,0},
		transform_,viewProj,
		floorMaterial_.get()
	);

	for(auto &object : gameObjects_) {
		object->Draw(viewProj);
	}
}