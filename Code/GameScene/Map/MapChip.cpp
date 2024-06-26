#include "MapChip.h"

#include "GameMap.h"

#include <assert.h>

Vector2 MapChip::size_;

void MapChip::Init(const uint32_t row,const uint32_t col,const std::string &directoryPath) {
	address_.first = row;
	address_.second = col;

	std::string filePath = directoryPath + '/' + std::to_string(address_.second) + '/' + std::to_string(address_.first) + ".area";
	std::ifstream file(filePath);
	assert(file.is_open());

	std::string line;
	while(std::getline(file,line)) {
		std::istringstream s(line);
		std::string identifier;
		s >> identifier;

		if(identifier == "CreateModel") {
			std::string modelFileName_Path[2];
			std::string materialName;
			Transform transform;
			while(std::getline(file,line)) {
				std::istringstream tokenStream(line);
				std::string token;
				std::getline(tokenStream,token,'=');

				if(token == "scale") {
					char comma;
					tokenStream >> transform.scale.x >> comma >> transform.scale.y >> comma >> transform.scale.z;
				} else if(token == "rotate") {
					char comma;
					tokenStream >> transform.rotate.x >> comma >> transform.rotate.y >> comma >> transform.rotate.z;
				} else if(token == "translate") {
					char comma;
					tokenStream >> transform.translate.x >> comma >> transform.translate.y >> comma >> transform.translate.z;
				} else if(token == "modelDirectoryPath") {
					tokenStream >> modelFileName_Path[0];
				} else if(token == "modelName") {
					tokenStream >> modelFileName_Path[1];
				} else if(token == "material") {
					tokenStream >> materialName;
				} else if(token == "end") {
					break;
				}
			}
			gameObjects_.push_back(std::make_unique<GameObject>());
			gameObjects_.back()->Init(modelFileName_Path[1],modelFileName_Path[0],materialName,transform);
		}
	}
	isLoaded_ = true;
}

void MapChip::Update() {
}

void MapChip::Draw(const ViewProjection &viewProj) {
	for(auto &object : gameObjects_) {
		object->Draw(viewProj);
	}
}