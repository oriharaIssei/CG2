#include "ImGuiManager.h"

#ifdef _DEBUG
#include "System.h"
#include "WinApp.h"

#include "DXCommand.h"
#include "DXDevice.h"
#include "DXSwapChain.h"
#include "DXHeap.h"

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#endif // _DEBUG

ImGuiManager *ImGuiManager::getInstance() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Init(const WinApp *window,const DXDevice *dxDevice,const DXSwapChain *dxSwapChain) {
#ifdef _DEBUG
	srvHeap_ = DXHeap::getInstance()->getSrvHeap();

	dxCommand_ = std::make_unique<DXCommand>();
	dxCommand_->Init(System::getInstance()->getDXDevice()->getDevice(),"main","main");

	///=============================================
	/// imgui の初期化
	///=============================================
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window->getHwnd());
	ImGui_ImplDX12_Init(
		dxDevice->getDevice(),
		dxSwapChain->getBufferCount(),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvHeap_,
		srvHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvHeap_->GetGPUDescriptorHandleForHeapStart()
	);
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

#endif // _DEBUG
}

void ImGuiManager::Finalize() {
#ifdef _DEBUG
	dxCommand_->Finalize();

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
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

void ImGuiManager::Draw() {
#ifdef _DEBUG
	ID3D12DescriptorHeap *ppHeaps[] = {srvHeap_};
	dxCommand_->getCommandList()->SetDescriptorHeaps(1,ppHeaps);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(),dxCommand_->getCommandList());
#endif // _DEBUG
}