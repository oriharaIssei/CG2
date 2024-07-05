#include "GameScene.h"

#include <System.h>
#include <PrimitiveDrawer.h>
#include <TextureManager.h>

#include <imgui.h>

GameScene::~GameScene() {
}

void GameScene::Init() {
	input_ = Input::getInstance();
	debugCamera.Init();
}

void GameScene::Update() {
	System::getInstance()->getMaterialManager()->DebugUpdate();// あまりおすすめしない
	debugCamera.Update();
	debugCamera.DebugUpdate();
}

void GameScene::Draw() {
}