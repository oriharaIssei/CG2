#include <System.h>

#include <memory>

#include <GameScene.h>
#include "GlobalVariables.h"

#include "DXDebug.h"

#include <d3d12sdklayers.h>

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int){
	std::unique_ptr<DXDebug> debug = std::make_unique<DXDebug>();
	System *system = System::getInstance();
	system->Init();

	GlobalVariables *variables = GlobalVariables::getInstance();
	variables->LoadAllFile();

	std::unique_ptr<GameScene> scene = std::make_unique<GameScene>();
	scene->Init();

	while(!system->ProcessMessage()){
		system->BeginFrame();

		variables->Update();

		scene->Update();
		scene->Draw();

		system->EndFrame();
	}

	system->Finalize();
	return 0;
}