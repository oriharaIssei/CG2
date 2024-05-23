#include "GameScene.h"

#include <System.h>
#include <PrimitiveDrawer.h>
#include <TextureManager.h>

#include <imgui.h>

GameScene::~GameScene() {
}

void GameScene::Init() {
	input_ = Input::getInstance();
	camera = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	sphere = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	debugCamera.Init();

	worldTransform_.Init();
	viewProj_.Init();
}

void GameScene::Update() {
	ImGui::Begin("Mouse");
	ImGui::Text("Mouse Pos x : %f, y : %f",input_->getCurrentMousePos().x,input_->getCurrentMousePos().y);
	ImGui::Text("Mouse Velocity x : %f, y : %f",input_->getMouseVelocity().x,input_->getMouseVelocity().y);
	ImGui::End();

	debugCamera.DebugUpdate();
	debugCamera.Update();

	viewProj_.viewMat = debugCamera.getViewProjection().viewMat;
	viewProj_.projectionMat = debugCamera.getViewProjection().projectionMat;
	viewProj_.ConvertToBuffer();

	ImGui::Begin("sphere");
	ImGui::DragFloat3("scale",&sphere.scale.x,0.01f);
	ImGui::DragFloat3("rotate",&sphere.rotate.x,0.01f);
	ImGui::DragFloat3("translate",&sphere.translate.x,0.01f);
	ImGui::End();

	worldTransform_.transformData = sphere;
	worldTransform_.Update();
}

void GameScene::Draw() {
	for(float i = 0; i < 32; i++) {
		PrimitiveDrawer::Line({i * 2,0.0f,i},{i * 3,i,0.0f},worldTransform_,viewProj_);
		PrimitiveDrawer::Quad({i,i + 1.0f,i},{i,i,i},{i + 1.0f,i + 1.0f,i + 1.0f},{i + 1.0f,i,i + 1.0f},worldTransform_,viewProj_);
	}
}