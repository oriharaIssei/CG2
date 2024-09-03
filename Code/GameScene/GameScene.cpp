#include "GameScene.h"

#include <System.h>
#include <PrimitiveDrawer.h>
#include "MyFileSystem.h"
#include <TextureManager.h>

#include <string>

#include "GameObject/ModelObject.h"
#include "GameObject/SphereObject.h"
#include "GameObject/SpriteObject.h"

#include "imgui.h"

constexpr char dockingIDName[] = "ObjectsWindow";

GameScene::~GameScene(){
}

void GameScene::Init(){
	debugCamera.Init();
	debugCamera.setViewTranslate({0.0f,0.0f,-12.0f});
	viewProj_.Init();
	input_ = Input::getInstance();

	materialManager_ = System::getInstance()->getMaterialManager();

	textureList_ = myFs::SearchFile("./resource","png");
	objectList_ = myFs::SearchFile("./resource","obj");

	std::unique_ptr<SphereObject> sprite = std::make_unique<SphereObject>();
	sprite->Init("","sphere");
	gameObjects_.emplace_back(std::move(sprite));
}

void GameScene::Update(){
	debugCamera.Update();
	debugCamera.DebugUpdate();
	viewProj_.viewMat = debugCamera.getViewProjection().viewMat;
	viewProj_.projectionMat = debugCamera.getViewProjection().projectionMat;
	viewProj_.ConvertToBuffer();

	if(ImGui::Begin("MaterialManager")){
		materialManager_->DebugUpdate();
	}
	ImGui::End();

	System::getInstance()->getDirectionalLight()->DebugUpdate();
	System::getInstance()->getPointLight()->DebugUpdate();
	System::getInstance()->getSpotLight()->DebugUpdate();

	if(ImGui::Begin("FileLists")){
		if(ImGui::TreeNode("TextureFiles")){
			ImGui::TreePop();
			if(ImGui::BeginChild("TextureFiles",ImVec2(250,128),true,ImGuiWindowFlags_HorizontalScrollbar)){
				for(auto &pngFile : textureList_){
					ImGui::Bullet();
					if(!ImGui::Button(pngFile.second.c_str())){
						continue;
					}
					std::unique_ptr<SpriteObject> sprite = std::make_unique<SpriteObject>();
					sprite->Init(pngFile.first,pngFile.second);
					gameObjects_.emplace_back(std::move(sprite));
				}
			}
			ImGui::EndChild();
		}
		// 高さ20pxのスペース
		ImGui::Dummy(ImVec2(0.0f,20.0f));

		if(ImGui::TreeNode("ModelList")){
			ImGui::TreePop();
			if(ImGui::BeginChild("ObjectFiles",ImVec2(250,128),true,ImGuiWindowFlags_HorizontalScrollbar)){
				for(auto &objFile : objectList_){
					ImGui::Bullet();
					if(!ImGui::Button(objFile.second.c_str())){
						continue;
					}
					std::unique_ptr<ModelObject> model = std::make_unique<ModelObject>();
					model->Init(objFile.first,objFile.second);
					gameObjects_.emplace_back(std::move(model));
				}
			}
			ImGui::EndChild();
		}

		ImGui::Dummy(ImVec2(0.0f,10.0f));
		if(ImGui::Button("Create Sphere")){
			std::unique_ptr<SphereObject> sprite = std::make_unique<SphereObject>();
			sprite->Init("","sphere");
			gameObjects_.emplace_back(std::move(sprite));
		}

		int32_t index = 0;

		ImGuiID dockingID = ImGui::GetID(dockingIDName);
		for(auto objectItr = gameObjects_.begin(); objectItr != gameObjects_.end();){
			ImGui::SetNextWindowDockID(dockingID,ImGuiCond_FirstUseEver);
			auto &object = *objectItr;
			std::string label = "# " + std::to_string(index) + object->getName();
			++index;
			if(ImGui::Begin(label.c_str())){
				object->Updata();

				ImGui::Dummy(ImVec2(0.0f,7.0f));
				if(ImGui::Button("Delete this")){
					objectItr = gameObjects_.erase(objectItr);
				} else{
					objectItr++;
				}
			}
			ImGui::End();
		}
	}
	ImGui::End();
}

void GameScene::Draw(){
	for(auto &object : gameObjects_){
		object->Draw(viewProj_);
	}
}