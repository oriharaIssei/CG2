#include "GameScene.h"

#include <System.h>

#include <imgui.h>

void GameScene::Init() {
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f } };
	cameraT_= { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f } };

	wvpVpMa_ = MakeMatrix::Affine(transform_.scale, transform_.rotate, transform_.translate)
		* MakeMatrix::Affine(cameraT_.scale, cameraT_.rotate, cameraT_.translate).Inverse()
		* MakeMatrix::PerspectiveFov(0.45f, static_cast<float>(System::getWinApp()->getWidth()) / static_cast<float>(System::getWinApp()->getHeight()), 0.1f, 100.0f);
}

void GameScene::Update() {
	transform_.rotate.y += 0.03f;
	wvpVpMa_ = MakeMatrix::Affine(transform_.scale, transform_.rotate, transform_.translate)
		* MakeMatrix::Affine(cameraT_.scale, cameraT_.rotate, cameraT_.translate).Inverse()
		* MakeMatrix::PerspectiveFov(0.45f, static_cast<float>(System::getWinApp()->getWidth()) / static_cast<float>(System::getWinApp()->getHeight()), 0.1f, 100.0f);
}

void GameScene::Draw() {
	System::DrawTriangle(wvpVpMa_);
}