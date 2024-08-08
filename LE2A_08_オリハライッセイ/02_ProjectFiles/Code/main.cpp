#include <System.h>

#include <memory>

#include <GameScene.h>

#include "DXDebug.h"

#include <d3d12sdklayers.h>

////========================================
////ToDo:
/*
	 Sprite の描画確認
*/
////========================================

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	std::unique_ptr<DXDebug> debug = std::make_unique<DXDebug>();
	System *system = System::getInstance();
	system->Init();

	std::unique_ptr<GameScene> scene = std::make_unique<GameScene>();
	scene->Init();

	while(!system->ProcessMessage()) {
		system->BeginFrame();

		scene->Update();
		scene->Draw();

		system->EndFrame();
	}

	system->Finalize();
	return 0;
}