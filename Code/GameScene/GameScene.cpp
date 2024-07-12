#include "GameScene.h"

#include <System.h>
#include <PrimitiveDrawer.h>
#include <TextureManager.h>

#include <imgui.h>

GameScene::~GameScene(){
	particle->Finalize();
}

void GameScene::Init(){
	debugCamera.Init();
	viewProj_.Init();
	input_ = Input::getInstance();

	particle = std::make_unique<Particle>();
	particle->Init(10);

	materialManager_ = std::make_unique<MaterialManager>();
	materialManager_->Create("white");

	for(int i = 1; i >= 0; --i){
		transform_[i].Init();
		transform_[i].translate.z = 12.0f;
		model_[i] = Model::Create("./resource/fence","fence.obj");
	}
}

void GameScene::Update(){
	debugCamera.Update();
	debugCamera.DebugUpdate();
	viewProj_.viewMat = debugCamera.getViewProjection().viewMat;
	viewProj_.projectionMat = debugCamera.getViewProjection().projectionMat;
	viewProj_.ConvertToBuffer();

	materialManager_->DebugUpdate();

	ImGui::Begin("Object0");
	transform_[0].Debug("Object0 Transform");
	transform_[0].Update();
	ImGui::End();

	ImGui::Begin("Object1");
	transform_[1].Debug("Object1 Transform");
	transform_[1].Update();
}

void GameScene::Draw(){
	for(int i = 0; i < 2; i++){
		model_[i]->Draw(transform_[i],viewProj_,materialManager_->getMaterial("white"));
	}
	particle->Draw(viewProj_,materialManager_->getMaterial("white"));
}