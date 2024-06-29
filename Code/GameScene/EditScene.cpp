#include "EditScene.h"

#include "GameScene.h"
#include "SceneManager.h"

#include "PrimitiveDrawer.h"
#include "System.h"

#include "IScene.h"

void EditScene::Init() {
	input_ = Input::getInstance();

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Init({1.3f,0.0f,0.0f},{2.5f,8.0f,2.5f});
	viewProj_.Init();

	mapEditor_ = std::make_unique<MapEditor>();
	mapEditor_->Init();
}

void EditScene::Update() {
	if(input_->isPressKey(DIK_RETURN)) {
		host_->TransitionScene(new GameScene(host_));
		return;
	}
	System::getInstance()->getMaterialManager()->DebugUpdate();

	debugCamera_->DebugUpdate();
	debugCamera_->Update();

	viewProj_.viewMat = debugCamera_->getViewProjection().viewMat;
	viewProj_.projectionMat = debugCamera_->getViewProjection().projectionMat;
	viewProj_.ConvertToBuffer();

	mapEditor_->Update();
}

void EditScene::Draw() {
	mapEditor_->Draw(viewProj_);
}