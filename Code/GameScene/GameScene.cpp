#include "GameScene.h"

#include <System.h>
#include <PrimitiveDrawer.h>
#include <TextureManager.h>
#include "MyFileSystem.h"

#include <string>

#include "GameObject/ModelObject.h"
#include "GameObject/SphereObject.h"
#include "GameObject/SpriteObject.h"

#include <imgui.h>

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

	materialManager_->DebugUpdate();

	ImGui::Begin("Light");
	LightBuffer *light = System::getInstance()->getStanderdLight();
	ImGui::DragFloat3("Direction",&light->direction.x,0.01f);
	light->direction = light->direction.Normalize();
	ImGui::DragFloat4("Color",&light->color.x,0.01f,0.0f,1.0f);
	ImGui::SliderFloat("Intensity",&light->intensity,0.0f,1.0f);
	light->ConvertToBuffer();
	ImGui::End();

	ImGui::Begin("GameObjectsManager");

	if(ImGui::TreeNode("TextureFiles")){
		ImGui::TreePop();
		if(ImGui::BeginChild("TextureFiles",ImVec2(250,64),true,ImGuiWindowFlags_HorizontalScrollbar)){
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

	if(ImGui::TreeNode("ModelList")){
		ImGui::TreePop();
		if(ImGui::BeginChild("ObjectFiles",ImVec2(250,64),true,ImGuiWindowFlags_HorizontalScrollbar)){
			for(auto &objFile : objectList_){
				ImGui::Bullet();
				if(!ImGui::Button(objFile.second.c_str())){
					continue;
				}
				std::unique_ptr<ModelObject> model = std::make_unique<ModelObject>();
				model->Init(objFile.first,objFile.second);
				gameObjects_.emplace_back(std::move(model));
			}
			ImGui::EndChild();
		}
	}

	for(auto &object : gameObjects_){
		object->Updata();
	}
	ImGui::End();
}

void GameScene::Draw(){
	for(auto &object : gameObjects_){
		object->Draw(viewProj_);
	}
}