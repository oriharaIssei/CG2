#include "DirectXCommon.h"

#include <WinApp.h>
#include <Logger.h>

//.hに書いてはいけない
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

DirectXCommon::~DirectXCommon() {
}

void DirectXCommon::Init() {
	//HRESULT : windows系のエラーコード。
	//SUCCEEDEDマクロによって成功かどうかを判定できる

	InitDXGIDevice();

	InitCommand();

	CreateSwapChain();

	CreateRenderTarget();

	Logger::OutputLog("Complete create D3D12Device \n");
}

void DirectXCommon::ClearRenderTarget() {
	HRESULT hr;

	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	//rtvを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

	commandList_->OMSetRenderTargets(
		1, &rtvHandles_[backBufferIndex], false, nullptr
	);

	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//RGBA

	commandList_->ClearRenderTargetView(
		rtvHandles_[backBufferIndex], clearColor, 0, nullptr
	);

	hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	ID3D12CommandList* commandList[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandList);

	swapChain_->Present(1, 0);

	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

void DirectXCommon::InitDXGIDevice() {
	HRESULT hr;
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	//デバッグレイヤーをオンに
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif // DEBUG

	///================================================
	///	IDXGIFactoryの初期化
	///================================================
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));

	assert(SUCCEEDED(hr), "FAILED CreateDXGIFactory");
	//=================================================

	///================================================
	///	IDXGIAdapterの初期化
	///================================================
	for (UINT i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(
			i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&useAdapter_))
		!= DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプターの情報取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter_->GetDesc3(&adapterDesc);

		assert(SUCCEEDED(hr), "FAILED GET AdapterDesc");

		//ソフトウェアアダプタは弾く
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			Logger::OutputLog(std::format(L"Use Adapter : {}\n", adapterDesc.Description));
			break;
		}
		//ソフトウェアアダプタの場合は見つからなかったことにする
		useAdapter_ = nullptr;
	}
	// "適切な" アダプターが見つからない場合はエラーを出す
	assert(useAdapter_ != nullptr);
	///================================================

	///================================================
	///	D3D12Device の生成
	///================================================
	//機能レベルとログ出力用文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featrueLevelStrings[] = { "12.2","12.1","12.0" };

	//レベルの高い順に生成できるか試す
	for (size_t i = 0; i < _countof(featureLevels); i++) {
		//採用したアダプターを生成
		hr = D3D12CreateDevice(
			useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_)
		);
		//生成できたか確認
		if (SUCCEEDED(hr)) {
			Logger::OutputLog(std::format("FeatureLevel : {}\n", featrueLevelStrings[i]));
			break;//生成出来たらループを抜ける
		}
	}
	assert(device_ != nullptr);
	///================================================
}

void DirectXCommon::InitCommand() {
	HRESULT hr;
	///================================================
	///	CommandQueue の生成
	///================================================
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(hr));
	///================================================

	///================================================
	///	CommandList & CommandAllocator の生成
	///================================================
	hr = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator_)
	);
	assert(SUCCEEDED(hr));

	hr = device_->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator_.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList_)
	);
	assert(SUCCEEDED(hr));
	///================================================
}

void DirectXCommon::CreateSwapChain() {
	HRESULT hr;
	///================================================
	///	SwapChain の生成
	///================================================
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = window_->getWidht();  //画面の幅。windowと同じにする
	swapchainDesc.Height = window_->getHeight(); //画面の高さ。windowと同じにする
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //色の形式
	swapchainDesc.SampleDesc.Count = 1; //マルチサンプルしない
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //描画のターゲットとして利用する
	swapchainDesc.BufferCount = 2; //ダブルバッファ
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //モニタに移したら中身を破棄する

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	hr = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(),
		window_->getHwnd(), //描画対象のWindowのハンドル
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapChain1
	);
	assert(SUCCEEDED(hr));

	// SwapChain4を得る
	swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));
	assert(SUCCEEDED(hr));
	///================================================
}

void DirectXCommon::CreateRenderTarget() {
	HRESULT hr;
	///================================================
	///	DescriptorHeap の生成
	///================================================
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};

	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; //レンダーターゲットビュー用
	rtvDescriptorHeapDesc.NumDescriptors = 2; //swapChainBufferCountに合わせる(多い分には問題ない)

	hr = device_->CreateDescriptorHeap(
		&rtvDescriptorHeapDesc,
		IID_PPV_ARGS(&rtvDescriptorHeap_)
	);
	assert(SUCCEEDED(hr));
	///================================================

	///================================================
	///	Resource の初期化
	///================================================
	hr = swapChain_.Get()->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	assert(SUCCEEDED(hr));

	hr = swapChain_.Get()->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));
	///================================================

	///================================================
	///	rtv の 作成
	///================================================
	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	//rtvを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	//ディスクリプタの先頭を取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	rtvHandles_[0] = rtvStartHandle;
	device_->CreateRenderTargetView(
		swapChainResources[0].Get(),
		&rtvDesc,
		rtvHandles_[0]
	);
	//2つ目はディスクリプタハンドルを自力で得る必要がある
	rtvHandles_[1].ptr = rtvHandles_[0].ptr
		+ device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	device_->CreateRenderTargetView(
		swapChainResources[1].Get(),
		&rtvDesc,
		rtvHandles_[1]
	);
	///================================================
}
