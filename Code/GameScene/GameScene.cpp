#include "GameScene.h"

#include <System.h>
#include <PrimitiveDrawer.h>
#include <TextureManager.h>
#include "MyFileSystem.h"

#include <string>

#include "GameObject/ModelObject.h"
#include "GameObject/SpriteObject.h"

#include <imgui.h>

GameScene::~GameScene(){
}

void GameScene::Init(){
	debugCamera.Init();
	viewProj_.Init();
	input_ = Input::getInstance();

	materialManager_ = std::make_unique<MaterialManager>();
	materialManager_->Create("white");

	textureList_ = myFs::SearchFile("./resource","png");
	objectList_ = myFs::SearchFile("./resource","obj");
}

void GameScene::Update(){
	debugCamera.Update();
	debugCamera.DebugUpdate();
	viewProj_.viewMat = debugCamera.getViewProjection().viewMat;
	viewProj_.projectionMat = debugCamera.getViewProjection().projectionMat;
	viewProj_.ConvertToBuffer();

	materialManager_->DebugUpdate();
	ImGui::Begin("GameObjectsManager");

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

	if(ImGui::BeginChild("ObjectFiles",ImVec2(250,64),true,ImGuiWindowFlags_HorizontalScrollbar)){
		for(auto &objFile : objectList_){
			ImGui::Bullet();
			if(!ImGui::Button(objFile.second.c_str())){
				continue;
			}
			std::unique_ptr<ModelObject> sprite = std::make_unique<ModelObject>();
			sprite->Init(objFile.first,objFile.second);
			gameObjects_.emplace_back(std::move(sprite));
		}
	}
	ImGui::EndChild();

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