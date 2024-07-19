#include "GameScene.h"

#include <System.h>
#include <PrimitiveDrawer.h>
#include <TextureManager.h>

#include <imgui.h>

GameScene::~GameScene(){
}

void GameScene::Init(){
	debugCamera.Init();
	viewProj_.Init();
	input_ = Input::getInstance();

	materialManager_ = std::make_unique<MaterialManager>();
	materialManager_->Create("white");

	emitter_ = std::make_unique<Emitter>();
	emitter_->Init(3,materialManager_.get());
}

void GameScene::Update(){
	debugCamera.Update();
	debugCamera.DebugUpdate();
	viewProj_.viewMat = debugCamera.getViewProjection().viewMat;
	viewProj_.projectionMat = debugCamera.getViewProjection().projectionMat;
	viewProj_.ConvertToBuffer();

	materialManager_->DebugUpdate();

	emitter_->Update();
}

void GameScene::Draw(){
	emitter_->Draw(viewProj_);
}