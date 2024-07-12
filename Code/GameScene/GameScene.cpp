#include "GameScene.h"

#include <System.h>
#include <PrimitiveDrawer.h>
#include <TextureManager.h>

#include <imgui.h>

GameScene::~GameScene(){
}

void GameScene::Init(){
	input_ = Input::getInstance();
	debugCamera.Init();
	bunny = Model::Create("./resource","bunny.obj");

	transform.Init();

}

void GameScene::Update(){
	System::getInstance()->getMaterialManager()->DebugUpdate();// あまりおすすめしない
	debugCamera.Update();
	debugCamera.DebugUpdate();

	ImGui::Begin("transform");
	ImGui::DragFloat3("scale",&transform.transformData.scale.x,0.1f);
	ImGui::DragFloat3("rotate",&transform.transformData.rotate.x,0.1f);
	ImGui::DragFloat3("translate",&transform.transformData.translate.x,0.1f);
	transform.Update();
	ImGui::End();
}

void GameScene::Draw(){
}