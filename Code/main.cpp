#include <WinApp.h>

#include <DirectXCommon.h>

#include <memory>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<WinApp> window = std::make_unique<WinApp>();
	window->CreateGameWindow(L"title", WS_OVERLAPPEDWINDOW, 1280, 720);

	std::unique_ptr<DirectXCommon> directXCommon = std::make_unique<DirectXCommon>(window.get());
	directXCommon->Init();
	while (!window->ProcessMessage()) {
		directXCommon->PreDraw();

		directXCommon->PostDraw();
	}

	directXCommon->Finalize();

	window->TerminateGameWindow();
	return 0;
}