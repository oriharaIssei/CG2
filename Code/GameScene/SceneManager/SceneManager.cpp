#include "SceneManager.h"

#include "IScene.h"

void SceneManager::Init(IScene *start) {
	currentScene_.reset(start);
	currentScene_->Init();
}

void SceneManager::Run() {
	currentScene_->Update();
	currentScene_->Draw();
}

void SceneManager::TransitionScene(IScene *next) {
	currentScene_.reset(next);
	currentScene_->Init();
}
