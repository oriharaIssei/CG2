#pragma once

#include <memory>
#include <wrl.h>

#include <d3d12.h>

class WinApp;
class DXDevice;
class DXSwapChain;
class DXCommand;

class ImGuiManager {
public:
	static ImGuiManager *getInstance();

	void Init(const WinApp *window,const DXDevice *dxDevice,const DXSwapChain *dxSwapChain);
	void Finalize();

	void Begin();
	void End();

	void Draw();
private:
#ifdef _DEBUG
	// SRV用ヒープ
	ID3D12DescriptorHeap *srvHeap_;

	std::unique_ptr<DXCommand> dxCommand_;
#endif // _DEBUG

	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager &) = delete;
	const ImGuiManager &operator=(const ImGuiManager &) = delete;
};