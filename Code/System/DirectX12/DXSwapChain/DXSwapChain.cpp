#include "DXSwapChain.h"

#include "DXCommand.h"
#include "DXDevice.h"
#include "WinApp.h"

#include <cassert>

void DXSwapChain::Init(const WinApp *winApp,const DXDevice *device,const DXCommand *command) {
	///================================================
	///	SwapChain の生成
	///================================================
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc {};
	swapchainDesc.Width = winApp->getWidth();  //画面の幅。windowと同じにする
	swapchainDesc.Height = winApp->getHeight(); //画面の高さ。windowと同じにする
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.SampleDesc.Count = 1; //マルチサンプルしない
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //描画のターゲットとして利用する
	swapchainDesc.BufferCount = 3; //ダブルバッファ
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //モニタに移したら中身を破棄する

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	HRESULT result = device->getDxgiFactory()->CreateSwapChainForHwnd(
		command->getCommandQueue(),
		winApp->getHwnd(), //描画対象のWindowのハンドル
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapChain1
	);
	assert(SUCCEEDED(result));

	// SwapChain4を得る
	result = swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));
	assert(SUCCEEDED(result));
	///================================================
	///	Resource の初期化
	///================================================
	bufferCount_ = 3;
	backBuffers_.resize(bufferCount_);
	for(int i = 0; i < (int)bufferCount_; ++i) {
		result = swapChain_->GetBuffer(
			i,IID_PPV_ARGS(&backBuffers_[i]));
		assert(SUCCEEDED(result));
	}
	///================================================
}

void DXSwapChain::Finalize() {
	for(auto &backBuffer : backBuffers_) {
		backBuffer.Reset();
	}
	swapChain_.Reset();
}

void DXSwapChain::Present() {
	swapChain_->Present(1,0);
}