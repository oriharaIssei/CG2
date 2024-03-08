#include <WinApp.h>

#include <memory>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	std::unique_ptr<WinApp> window = std::make_unique<WinApp>();
	window->CreateGameWindow(L"title", WS_OVERLAPPEDWINDOW, 1280, 720);

	while (!window->ProcessMessage()) {

	}

	return 0;
}