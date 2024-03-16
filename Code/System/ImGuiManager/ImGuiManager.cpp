#include "ImGuiManager.h"

#ifdef _DEBUG
#include <WinApp.h>
#include <DirectXCommon.h>

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#endif // _DEBUG

ImGuiManager* ImGuiManager::getInstance() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Init(const WinApp* window, DirectXCommon* dxCommon) {
#ifdef _DEBUG
	// デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// デスクリプタヒープ生成
	HRESULT hr = dxCommon->getDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(hr));
	srvHeap_->SetName(L"ImGui_DescriptorHeap");

	///=============================================
	/// imgui の初期化
	///=============================================
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window->getHwnd());
	ImGui_ImplDX12_Init(
		dxCommon->getDevice(),
		dxCommon->getSwapChainBufferCount(),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvHeap_.Get(),
		srvHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvHeap_->GetGPUDescriptorHandleForHeapStart()
	);
#endif // _DEBUG
}

void ImGuiManager::Finalize() {
#ifdef _DEBUG
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	srvHeap_.Reset();
#endif // _DEBUG
}

void ImGuiManager::Begin() {
#ifdef _DEBUG
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif // _DEBUG
}

void ImGuiManager::End() {
#ifdef _DEBUG
	// 描画前準備
	ImGui::Render();
#endif
}

void ImGuiManager::Draw(DirectXCommon* dxCommon) {
#ifdef _DEBUG
	ID3D12GraphicsCommandList* commandList = dxCommon->getCommandList();

	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap_.Get() };
	commandList->SetDescriptorHeaps(1, ppHeaps);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif // _DEBUG
}
