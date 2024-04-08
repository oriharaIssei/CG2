#include "GameScene.h"

#include <System.h>

#include <imgui.h>

void GameScene::Init() {
	camera = {
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,0.0f, 0.0f }
	};
	sphere = {
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,0.0f, 5.0f }
	};

	uv = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	gh[0] = System::LoadTexture("uvChecker.png");
	gh[1] = System::LoadTexture("monsterBall.png");
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
	ImGui::Checkbox("GH", &ghSwitcher_);
	ImGui::End();
	ImGui::Begin("UV");
	ImGui::DragFloat2("scale", &uv.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("rotate", &uv.rotate.z);
	ImGui::DragFloat2("translate", &uv.translate.x, 0.01f, -10.0f, 10.0f);
	ImGui::End();
}

void GameScene::Draw() {
	System::DrawSphere(
		MakeMatrix::Affine(sphere.scale, sphere.rotate, sphere.translate),
		MakeMatrix::Affine(camera.scale, camera.rotate, camera.translate).Inverse(),
		{1.0f,1.0f,1.0f,1.0f}
		);
	System::DrawSprite(
		{ 0.0f,0.0f,1.0f },
		{ 500.0f,500.0f },
		uv.scale,
		uv.rotate,
		uv.translate,
		0
	);
}