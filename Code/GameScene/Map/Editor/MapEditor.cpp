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

const std::string SAVE_FOLDER_PATH = "./resource/Map";
const std::string MAP_INFO_FILENAME = "mapInformation.gmp";
const std::string MATERIAL_LIST_START = "MaterialList";
const std::string MATERIAL_LIST_END = "EndMaterialList";
const std::string CREATE_MODEL = "CreateModel";
const std::string END_MODEL = "end";

void MapEditor::Init() {
	materialManager_ = std::make_unique<MaterialManager>("MapMaterialManager");

	chips_.resize(1);
	chips_[0].push_back(std::make_unique<EditChip>());
	EditChip::size = {5.0f,5.0f};
	chips_[0].back()->Init(this,{0,0});

	currentState_ = std::make_unique <AllViewState>(this);
}

void MapEditor::Update() {
	ImGui::Begin("MapEditor",nullptr,ImGuiWindowFlags_MenuBar);
	if(ImGui::BeginMenuBar()) {
		if(ImGui::BeginMenu("File")) {
			if(ImGui::MenuItem("save")) {
				SaveToFile();
			} else if(ImGui::MenuItem("load")) {
				Load();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	materialManager_->DebugUpdate();
	currentState_->Update();

	ImGui::End();
}

void MapEditor::SaveMapInformation() {
	std::ofstream mapFile(SAVE_FOLDER_PATH + '/' + MAP_INFO_FILENAME);
	if(!mapFile.is_open()) {
		throw std::ios_base::failure("Failed to open map information file.");
	}

	mapFile << "ChipSize " << MapEditor::EditChip::size.x << ',' << MapEditor::EditChip::size.y << '\n';
	mapFile << "MaxAddress " << chips_[0].size() << ',' << chips_.size() << '\n';

	mapFile << MATERIAL_LIST_START + "\n";
	for(const auto &material : materialManager_->getMateriaList()) {
		mapFile << "Name " << material.first << '\n';
		const auto &color = material.second->getColor();
		mapFile << "Color " << color.x << ',' << color.y << ',' << color.z << ',' << color.w << '\n';
		mapFile << "EnableLighting " << material.second->getEnableLighting() << '\n';
	}
	mapFile << MATERIAL_LIST_END;
}

void MapEditor::SaveMapChips() {
	for(size_t col = 0; col < chips_.size(); ++col) {
		std::string colPath = SAVE_FOLDER_PATH + "/" + std::to_string(col);
		std::filesystem::create_directories(colPath);

		for(size_t row = 0; row < chips_[col].size(); ++row) {
			std::ofstream mapChipFile(colPath + '/' + std::to_string(row) + ".gcp");
			if(!mapChipFile.is_open()) {
				throw std::ios_base::failure("Failed to open map chip file.");
			}

			for(const auto &mapObj : chips_[col][row]->gameObjects_) {
				WriteGameObjectInfo(mapChipFile,*mapObj);
			}
		}
	}
}

void MapEditor::WriteGameObjectInfo(std::ofstream &mapChipFile,GameObject &mapObj) {
	const auto &transform = mapObj.getWorldTransform();
	mapChipFile << CREATE_MODEL << '\n';
	mapChipFile << "scale " << transform.transformData.scale.x << ' ' << transform.transformData.scale.y << ' ' << transform.transformData.scale.z << ' ' << '\n';
	mapChipFile << "rotate " << transform.transformData.rotate.x << ' ' << transform.transformData.rotate.y << ' ' << transform.transformData.rotate.z << ' ' << '\n';
	mapChipFile << "translate " << transform.transformData.translate.x << ' ' << transform.transformData.translate.y << ' ' << transform.transformData.translate.z << ' ' << '\n';
	mapChipFile << "material " << mapObj.getMaterialName() << '\n';
	mapChipFile << "modelDirectoryPath " << mapObj.getModel()->getDirectory() << '\n';
	mapChipFile << "modelName " << mapObj.getModel()->getName() << '\n';
	mapChipFile << END_MODEL + '\n';
}

void MapEditor::SaveToFile() {
	std::uintmax_t deleted_count = std::filesystem::remove_all(SAVE_FOLDER_PATH);
	std::filesystem::create_directories(SAVE_FOLDER_PATH);

	SaveMapInformation();
	SaveMapChips();
}

void MapEditor::LoadMaterialList(std::ifstream &mapFile) {
	std::string line;

	MaterialData data;
	std::string name;
	while(std::getline(mapFile,line)) {
		if(line == MATERIAL_LIST_END) {
			break;
		}
		std::istringstream iss(line);
		std::string key;
		iss >> key;

		if(key == "Name") {
			iss >> name;
		} else if(key == "Color") {
			char comma;
			iss >> data.color.x >> comma >> data.color.y >> comma >> data.color.z >> comma >> data.color.w;
		} else if(key == "EnableLighting") {
			iss >> data.enableLighting;
		}

		materialManager_->Create(name,data);
	}
}

void MapEditor::LoadMapInformation() {
	std::ifstream mapFile(SAVE_FOLDER_PATH + '/' + MAP_INFO_FILENAME);
	if(!mapFile.is_open()) {
		throw std::ios_base::failure("Failed to open map information file.");
	}

	std::string line;
	while(std::getline(mapFile,line)) {
		std::istringstream iss(line);
		std::string key;
		iss >> key;

		if(key == "ChipSize") {
			char comma;
			iss >> MapEditor::EditChip::size.x >> comma >> MapEditor::EditChip::size.y;
		} else if(key == "MaxAddress") {
			char comma;
			size_t maxAddressX,maxAddressY;
			iss >> maxAddressX >> comma >> maxAddressY;
			chips_.resize(maxAddressY);
			uint32_t col = 0;
			uint32_t row = 0;
			for(auto &column : chips_) {
				column.resize(maxAddressX);
				row = 0;
				for(auto &chip : column) {
					chip = std::make_unique<EditChip>();
					chip->Init(this,{row,col});
					++row;
				}
				++col;
			}
		} else if(key == MATERIAL_LIST_START) {
			LoadMaterialList(mapFile);
		}
	}
}

void MapEditor::LoadMapChips() {
	for(size_t col = 0; col < chips_.size(); ++col) {
		std::string colPath = SAVE_FOLDER_PATH + "/" + std::to_string(col);

		for(size_t row = 0; row < chips_[col].size(); ++row) {
			std::ifstream mapChipFile(colPath + '/' + std::to_string(row) + ".gcp");

			chips_[col][row]->Init(this,{row,col});

			if(!mapChipFile.is_open()) {
				throw std::ios_base::failure("Failed to open map chip file.");
			}

			while(mapChipFile) {
				std::string line;
				std::getline(mapChipFile,line);
				if(line == CREATE_MODEL) {
					LoadGameObjectInfo(mapChipFile,col,row);
				}
			}
		}
	}
}

void MapEditor::LoadGameObjectInfo(std::ifstream &mapChipFile,size_t col,size_t row) {
	auto mapObj = std::make_unique<GameObject>();

	Transform transform;
	std::string materialName;
	std::string modelFile[2];

	std::string line;
	while(std::getline(mapChipFile,line)) {
		if(line == END_MODEL) {
			break;
		}

		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if(identifier == "scale") {
			s >> transform.scale.x >>
				transform.scale.y >>
				transform.scale.z;
		} else if(identifier == "rotate") {
			s >> transform.rotate.x
				>> transform.rotate.y
				>> transform.rotate.z;
		} else if(identifier == "translate") {
			char comma;
			s >> transform.translate.x
				>> transform.translate.y
				>> transform.translate.z;
		} else if(identifier == "material") {
			s >> materialName;
		} else if(identifier == "modelDirectoryPath") {
			s >> modelFile[0];
		} else if(identifier == "modelName") {
			s >> modelFile[1];
		}
	}
	mapObj->Init(modelFile[1],modelFile[0],materialName,transform);
	chips_[col][row]->gameObjects_.emplace_back(std::move(mapObj));
}

void MapEditor::Load() {
	TransitionState(new AllViewState(this));

	LoadMapInformation();
	LoadMapChips();
}

void MapEditor::Draw(const ViewProjection &viewProj) {
	currentState_->Draw(viewProj);
}

void MapEditor::TransitionState(IMapEditState *nextState) {
	currentState_.reset();
	currentState_ = std::unique_ptr<IMapEditState>(nextState);
}

void MapEditor::EditChip::Init(MapEditor *host,std::pair<uint32_t,uint32_t> address) {
	address_ = address;
	transform_.Init();
	transform_.transformData.translate = {static_cast<float>(address.first * size.x),0.0f,static_cast<float>(address.second * size.y)};
	transform_.Update();

	host_ = host;
	floorMaterial_ = host_->materialManager_->Create("Ground");
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