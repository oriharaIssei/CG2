#include "GameScene.h"

#include <System.h>
#include <PrimitiveDrawer.h>
#include <TextureManager.h>

#include <imgui.h>

GameScene::~GameScene() {
	viewProj_.Finalize();
}

void GameScene::Init() {
	input_ = Input::getInstance();

	debugCamera.Init({1.3f,0.0f,0.0f},{2.5f,8.0f,2.5f});
	viewProj_.Init();

	mapEditor_ = std::make_unique<MapEditor>();
	mapEditor_->Init();
}

void GameScene::Update() {
	System::getInstance()->getMaterialManager()->DebugUpdate();

	debugCamera.DebugUpdate();
	debugCamera.Update();

	viewProj_.viewMat = debugCamera.getViewProjection().viewMat;
	viewProj_.projectionMat = debugCamera.getViewProjection().projectionMat;
	viewProj_.ConvertToBuffer();

	mapEditor_->Update();
}

void GameScene::Draw() {
	mapEditor_->Draw(viewProj_);
}