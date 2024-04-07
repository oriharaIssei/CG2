#include <System.h>

#include <memory>

#include <GameScene.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	System::Init();
	
	std::unique_ptr<GameScene> scene = std::make_unique<GameScene>();
	scene->Init();

	while (!System::ProcessMessage()) {
		System::BeginFrame();

		scene->Update();
		scene->Draw();


		System::EndFrame();
	}

	System::Finalize();
	return 0;
}