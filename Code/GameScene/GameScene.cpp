#include "GameScene.h"

#include <System.h>
#include <PrimitiveDrawer.h>
#include <TextureManager.h>

#include <imgui.h>

GameScene::~GameScene() {
}

void GameScene::Init() {
	debugCamera.Init();
	viewProj_.Init();
	input_ = Input::getInstance();

	materialManager_ = std::make_unique<MaterialManager>();
	materialManager_->Create("white");

	transform_.Init();
	transform_.translate.z = 12.0f;
	model_ = Model::Create("./resource","plane.obj");
}

void GameScene::Update() {
	debugCamera.Update();
	debugCamera.DebugUpdate();
	viewProj_.viewMat = debugCamera.getViewProjection().viewMat;
	viewProj_.projectionMat = debugCamera.getViewProjection().projectionMat;
	viewProj_.ConvertToBuffer();

	materialManager_->DebugUpdate();

	ImGui::Begin("Object");
	transform_.Debug("Object Transform");
	transform_.Update();
	ImGui::End();
}

void GameScene::Draw() {
	model_->Draw(transform_,viewProj_,materialManager_->getMaterial("white"));
}