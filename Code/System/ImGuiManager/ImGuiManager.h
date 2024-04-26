#pragma once

#include <wrl.h>

#include <d3d12.h>

class WinApp;
class DirectXCommon;

class ImGuiManager {
public:
	static ImGuiManager *getInstance();

	void Init(const WinApp *window, DirectXCommon *dxCommon);
	void Finalize();

	void Begin();
	void End();

	void Draw(DirectXCommon *dxCommon);

private:
#ifdef _DEBUG
	// SRV用ヒープ
	ID3D12DescriptorHeap *srvHeap_;
#endif // _DEBUG

	ImGuiManager()=default;
	~ImGuiManager()=default;
	ImGuiManager(const ImGuiManager &)=delete;
	const ImGuiManager &operator=(const ImGuiManager &)=delete;
};