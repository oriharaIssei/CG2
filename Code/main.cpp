#include <System.h>

#include <memory>

#include <GameScene.h>

////========================================
////ToDo:
/*
	 Sprite の描画確認
*/
////========================================
struct LeakChecker {
	~LeakChecker() {
		IDXGIDebug1 *debug;
		if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			debug->Release();
		}
	}
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	LeakChecker leaker;
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