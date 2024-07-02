#include "GameScene.h"

#include <System.h>
#include <PrimitiveDrawer.h>
#include <TextureManager.h>

#include "SceneManager/SceneManager.h"
#include "EditScene.h"

#include <imgui.h>

void GameScene::Init() {
	input_ = Input::getInstance();

	viewProj_.Init();

	player_ = std::make_unique<Player>();
	player_->Init({20.0f,0.0f,30.0f});

	gameMap_ = std::make_unique<GameMap>();
	gameMap_->Init("./resource/Map",player_->getTransform().transformData.translate,10.0f);
}

void GameScene::Update() {
	ImGui::Begin("GameScene");

	if(ImGui::Button("go to EditorMode") || input_->isPressKey(DIK_ESCAPE)) {
		System::getInstance()->setShowCursor(true);
		host_->TransitionScene(new EditScene(host_));
		ImGui::End();
		return;
	}

	System::getInstance()->getMaterialManager()->DebugUpdate();

	player_->Update();

	gameMap_->Update(player_->getTransform().worldMat[3]);

	ImGui::End();

	viewProj_.translate = player_->getTransform().transformData.translate;
	viewProj_.rotate = player_->getTransform().transformData.rotate;
	viewProj_.UpdateMatrix();
	viewProj_.ConvertToBuffer();
}

void GameScene::Draw() {
	gameMap_->Draw(player_->getTransform().worldMat[3],viewProj_);
}