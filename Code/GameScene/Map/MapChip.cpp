#include "MapChip.h"

#include "GameMap.h"

#include "PrimitiveDrawer.h"

#include <assert.h>

Vector2 MapChip::size_;

void MapChip::Init(const uint32_t row,const uint32_t col,MaterialManager *materialManager,const std::string &directoryPath) {
	address_.first = row;
	address_.second = col;

	transform_.Init();
	transform_.transformData.translate = {col * MapChip::size_.x,0, row * MapChip::size_.y};
	transform_.Update();

	material_ = materialManager->Create("White");

	std::string filePath = directoryPath + '/' + std::to_string(address_.first) + '/' + std::to_string(address_.second) + ".gcp";
	std::ifstream file(filePath);
	assert(file.is_open());

	std::string line;
	while(std::getline(file,line)) {
		std::istringstream s(line);
		std::string identifier;
		s >> identifier;

		if(identifier == "CreateModel") {
			auto mapObj = std::make_unique<GameObject>();

			Transform transform;
			std::string materialName;
			std::string modelFile[2];

			std::string modelParamLine;
			while(std::getline(file,modelParamLine)) {
				std::istringstream modelParamStream(modelParamLine);
				std::string modelParam;
				modelParamStream >> modelParam;

				if(modelParam == "end") {
					break;
				}
				if(modelParam == "scale") {
					modelParamStream >> transform.scale.x >>
						transform.scale.y >>
						transform.scale.z;
				} else if(modelParam == "rotate") {
					modelParamStream >> transform.rotate.x
						>> transform.rotate.y
						>> transform.rotate.z;
				} else if(modelParam == "translate") {
					modelParamStream >> transform.translate.x
						>> transform.translate.y
						>> transform.translate.z;
				} else if(modelParam == "material") {
					modelParamStream >> materialName;
				} else if(modelParam == "modelDirectoryPath") {
					modelParamStream >> modelFile[0];
				} else if(modelParam == "modelName") {
					modelParamStream >> modelFile[1];
				}
			}
			mapObj->Init(modelFile[1],modelFile[0],materialName,materialManager,transform);
			gameObjects_.push_back(std::move(mapObj));
		}
		isLoaded_ = true;
	}
}
void MapChip::Update() {
}

void MapChip::Draw(float drawDistance,const Vector3 &playerPos,const ViewProjection &viewProj) {

	PrimitiveDrawer::Quad(
		{0.0f,0.0f,MapChip::size_.y},
		{MapChip::size_.x,0.0f,MapChip::size_.y},
		{0.0f,0.0f,0},
		{MapChip::size_.x,0.0f,0},
		transform_,viewProj,
		material_.get()
	);

	Vector3 objectPos;
	for(auto &object : gameObjects_) {
		objectPos = Vector3(object->getWorldTransform().worldMat[3]);
		if((objectPos - playerPos).length() > drawDistance - 0.1f) {
			continue;
		}
		object->Draw(viewProj);
	}
}