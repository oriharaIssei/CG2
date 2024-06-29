#include "GameScene.h"

#include <System.h>
#include <PrimitiveDrawer.h>
#include <TextureManager.h>

#include "SceneManager/SceneManager.h"
#include "EditScene.h"

#include <imgui.h>

void GameScene::Init() {
	input_ = Input::getInstance();

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Init({1.3f,0.0f,0.0f},{2.5f,8.0f,2.5f});
	viewProj_.Init();

	player_ = std::make_unique<Player>();
	player_->Init({20.0f,0.0f,0.0f});

	gameMap_ = std::make_unique<GameMap>();
	gameMap_->Init("./resource/Map",player_->getTransform().transformData.translate,5.0f);
}

void GameScene::Update() {
	ImGui::Begin("GameScene");

	if(ImGui::Button("go to EditorMode") || input_->isPressKey(DIK_ESCAPE)) {
		host_->TransitionScene(new EditScene(host_));
		return;
	}

	ImGui::DragFloat("eye speed",&eyeSpeed_,0.001f,0.001f,1.0f);

	System::getInstance()->getMaterialManager()->DebugUpdate();

	player_->Update();

	gameMap_->Update(player_->getTransform().transformData.translate);

	ImGui::End();

	viewProj_.translate = player_->getTransform().transformData.translate;
	viewProj_.translate *= -1.0f;
	Vector3 eyeDir_ = {static_cast<float>(input_->getMouseVelocity().y),static_cast<float>(input_->getMouseVelocity().x),0};
	viewProj_.rotate += eyeDir_ * eyeSpeed_;
	viewProj_.UpdateMatrix();
	viewProj_.ConvertToBuffer();
}

void GameScene::Draw() {
	gameMap_->Draw(viewProj_);
}