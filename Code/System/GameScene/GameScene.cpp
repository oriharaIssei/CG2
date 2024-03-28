#include "GameScene.h"

#include <System.h>

#include <imgui.h>

void GameScene::Init() {
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f } };
	cameraT_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f } };

	wvpVpMa_ = MakeMatrix::Affine(transform_.scale, transform_.rotate, transform_.translate)
		* MakeMatrix::Identity()
		* MakeMatrix::Orthographic(0, 0, static_cast<float>( System::getWinApp()->getWidth() ), static_cast<float>( System::getWinApp()->getHeight() ), 0.0f, 100.0f);
	gh_ = System::LoadTexture("uvChecker.png");
}

void GameScene::Update() {
	
}

void GameScene::Draw() {
	System::DrawSprite(wvpVpMa_, gh_);
}