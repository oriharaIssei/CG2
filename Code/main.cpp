#include <System.h>

#include <memory>

#include "EditScene.h"
#include "SceneManager.h"

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

	std::unique_ptr<SceneManager> sceneManager = std::make_unique<SceneManager>();
	sceneManager->Init(new EditScene(sceneManager.get()));

	while(!system->ProcessMessage()) {
		system->BeginFrame();

		sceneManager->Run();

		system->EndFrame();
	}

	system->Finalize();
	return 0;
}