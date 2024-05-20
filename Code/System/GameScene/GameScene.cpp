#include "GameScene.h"

#include <System.h>
#include <TextureManager.h>

#include <imgui.h>

GameScene::~GameScene() {
}

void GameScene::Init() {
	input_ = Input::getInstance();
	camera = {
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,0.0f, 0.0f }
	};
	sphere = {
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,0.0f, 0.0f }
	};

	worldTransform_.Init();
	viewProj_.Init();

	model.reset(Model::Create("./resource", "axis.obj"));
}

void GameScene::Update() {
	ImGui::Begin("camera");
	ImGui::DragFloat3("scale", &camera.scale.x, 0.01f);
	ImGui::DragFloat3("rotate", &camera.rotate.x, 0.01f);
	ImGui::DragFloat3("translate", &camera.translate.x, 0.01f);
	ImGui::End();
	ImGui::Begin("sphere");
	ImGui::DragFloat3("scale", &sphere.scale.x, 0.01f);
	ImGui::DragFloat3("rotate", &sphere.rotate.x, 0.01f);
	ImGui::DragFloat3("translate", &sphere.translate.x, 0.01f);
	ImGui::End();
	/*ImGui::Begin("UV");
	ImGui::DragFloat2("scale", &uv.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("rotate", &uv.rotate.z);
	ImGui::DragFloat2("translate", &uv.translate.x, 0.01f, -10.0f, 10.0f);
	ImGui::End();
	model->setUv(MakeMatrix::Identity());
	*/

	worldTransform_.transformData = sphere;

	worldTransform_.Update();
	viewProj_.UpdateMatrix();

	viewProj_.viewMat = MakeMatrix::Affine(camera).Inverse();
	viewProj_.ConvertToBuffer();
}

void GameScene::Draw() {
	if(input_->isPressKey(DIK_W)) {
		model->Draw(worldTransform_, viewProj_);
	}
}