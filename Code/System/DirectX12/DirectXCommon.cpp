#include "DirectXCommon.h"

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

#include <WinApp.h>
#include <Logger.h>

#include <Vector4.h>

//.hに書いてはいけない
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

DirectXCommon::~DirectXCommon() {
	CheckIsAliveInstance();
}

void DirectXCommon::Init() {
	//HRESULT : windows系のエラーコード。
	//SUCCEEDEDマクロによって成功かどうかを判定できる

	InitDXGIDevice();

	InitCommand();

	CreateSwapChain();

	CreateRenderTarget();

	CreatDepthBuffer();

	CreateFence();

	Logger::OutputLog("Complete create D3D12Device \n");
}

void DirectXCommon::Finalize() {
	device_.Reset();
	dxgiFactory_.Reset();
	useAdapter_.Reset();
	commandQueue_.Reset();
	commandAllocator_.Reset();
	commandList_.Reset();
	swapChain_.Reset();
	rtvDescriptorHeap_.Reset();
	srvDescriptorHeap_.Reset();
	fence_.Reset();

	for(int i = 0; i < swapChainResources_.size(); i++) {
		swapChainResources_[i].Reset();
	}
	swapChainResources_.clear();

	dsvDescriptorHeap_.Reset();
	depthStencilResource_.Reset();

	debugController_.Reset();
}

void DirectXCommon::ClearRenderTarget() {
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	commandList_->OMSetRenderTargets(
		1, &rtvH_[backBufferIndex], false, &dsvHandle
	);

	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//RGBA
	commandList_->ClearRenderTargetView(
		rtvH_[backBufferIndex], clearColor, 0, nullptr
	);

	commandList_->ClearDepthStencilView(
		dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr
	);
}

void DirectXCommon::ExecuteCommandList() {
	///===============================================================
	/// コマンドリストの実行
	///===============================================================
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);

	HRESULT hr = swapChain_->Present(1, 0);
	///===============================================================
}

void DirectXCommon::Wait4ExecuteCommand() {
	// Fenceの値を更新
	// //イベントを待つ
	// // 指定したSignalにたどりついていないのでたどり着くまで待つようにイベントを設置

	// GPUがここまでたどり着いたとき、FenceVal_ を指定した値に代入するように Signal を送る
	commandQueue_->Signal(fence_.Get(), ++fenceVal_);
	if(fence_->GetCompletedValue() < fenceVal_) {
		HANDLE fenceEvent = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceVal_, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
		CloseHandle(fenceEvent);
	}
}

void DirectXCommon::ResetCommand() {
	HRESULT hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(ID3D12DescriptorHeap* heap, uint32_t descriptorSize, uint32_t index) {
	return D3D12_CPU_DESCRIPTOR_HANDLE(heap->GetCPUDescriptorHandleForHeapStart().ptr + ( descriptorSize * index ));
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(ID3D12DescriptorHeap* heap, uint32_t descriptorSize, uint32_t index) {
	return D3D12_GPU_DESCRIPTOR_HANDLE(heap->GetGPUDescriptorHandleForHeapStart().ptr + ( descriptorSize * index ));
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>descriptorHeap = nullptr;

	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};

	rtvDescriptorHeapDesc.Type = heapType;
	rtvDescriptorHeapDesc.NumDescriptors = numDescriptors; //swapChainBufferCountに合わせる(多い分には問題ない)
	rtvDescriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	hr = device_->CreateDescriptorHeap(
		&rtvDescriptorHeapDesc,
		IID_PPV_ARGS(&descriptorHeap)
	);
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}


void DirectXCommon::CreateGraphicsPipelineState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, ID3D12PipelineState* pipelineState) {
	HRESULT hr = device_->CreateGraphicsPipelineState(
		&psoDesc,
		IID_PPV_ARGS(&pipelineState)
	);
	assert(SUCCEEDED(hr));
}

void DirectXCommon::CreateBufferResource(ID3D12Resource* resource, size_t sizeInBytes) {
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	//バッファのリソース(テクスチャの場合は別の設定をする)
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes;
	//バッファの場合、これらは 1 にする
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr = device_->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));
}

void DirectXCommon::CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Resource>& resource, size_t sizeInBytes) {
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	//バッファのリソース(テクスチャの場合は別の設定をする)
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes;
	//バッファの場合、これらは 1 にする
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr = device_->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));
}

void DirectXCommon::CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Resource>& resource, int32_t width, int32_t height) {
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//heap の設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;// 最大値でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// Resource と合わせる

	HRESULT hr = device_->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));
}

void DirectXCommon::CheckIsAliveInstance() {
	IDXGIDebug1* debug;
	if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
}

void DirectXCommon::InitDXGIDevice() {
	HRESULT hr;
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	//デバッグレイヤーをオンに
	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバッグレイヤーの有効化
		debugController->EnableDebugLayer();
		//GPU側でもデバッグさせる
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // DEBUG

	///================================================
	///	IDXGIFactoryの初期化
	///================================================
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));

	assert(SUCCEEDED(hr));
	//=================================================

	///================================================
	///	IDXGIAdapterの初期化
	///================================================
	for(UINT i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(
			i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&useAdapter_))
		!= DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプターの情報取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter_->GetDesc3(&adapterDesc);

		assert(SUCCEEDED(hr));

		//ソフトウェアアダプタは弾く
		if(!( adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE )) {
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
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0
	};
	const char* featrueLevelStrings[] = {
		"12.2","12.1","12.0"
	};

	//レベルの高い順に生成できるか試す
	for(size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアダプターを生成
		hr = D3D12CreateDevice(
			useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_)
		);
		//生成できたか確認
		if(SUCCEEDED(hr)) {
			Logger::OutputLog(std::format("FeatureLevel : {}\n", featrueLevelStrings[i]));
			break;//生成出来たらループを抜ける
		}
	}
	assert(device_ != nullptr);
	///================================================

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if(SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//やばいエラーの時に止める
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//=========================================
		//エラーと警告の抑制
		//=========================================
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11 でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用によるエラーメッセージ
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY serverities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(serverities);
		filter.DenyList.pSeverityList = serverities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		infoQueue->Release();//解放
	}
#endif // _DEBUG
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
	swapchainDesc.Width = window_->getWidth();  //画面の幅。windowと同じにする
	swapchainDesc.Height = window_->getHeight(); //画面の高さ。windowと同じにする
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
	hr = swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));
	assert(SUCCEEDED(hr));
	///================================================

	///================================================
	///	Resource の初期化
	///================================================
	swapChainResources_.resize(2);
	for(int i = 0; i < 2; ++i) {
		hr = swapChain_->GetBuffer(
			i, IID_PPV_ARGS(&swapChainResources_[i]));
		assert(SUCCEEDED(hr));
	}
	///================================================
}

void DirectXCommon::CreateRenderTarget() {
	///================================================
	///	DescriptorHeap の生成
	///================================================
	rtvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	srvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	///================================================

	///================================================
	///	rtv の 作成
	///================================================
	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	//ディスクリプタの先頭を取得
	rtvH_[0] = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	device_->CreateRenderTargetView(
		swapChainResources_[0].Get(),
		&rtvDesc,
		rtvH_[0]
	);
	//2つ目はディスクリプタハンドルを自力で得る必要がある
	rtvH_[1].ptr = rtvH_[0].ptr
		+ device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	device_->CreateRenderTargetView(
		swapChainResources_[1].Get(),
		&rtvDesc,
		rtvH_[1]
	);
	///================================================
}

void DirectXCommon::CreateFence() {
	//初期値0でFenceを生成
	HRESULT hr = device_->CreateFence(
		fenceVal_,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&fence_)
	);
	assert(SUCCEEDED(hr));
}

void DirectXCommon::CreatDepthBuffer() {
	CreateDepthStencilTextureResource(
		depthStencilResource_,
		static_cast<int32_t>( window_->getWidth() ),
		static_cast<int32_t>( window_->getHeight() )
	);

	dsvDescriptorHeap_ = CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		1,
		false
	);

	// DSV の設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// resourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;// 2d Texture

	// DsvHeap の先頭に DSV を作る
	device_->CreateDepthStencilView(
		depthStencilResource_.Get(),
		&dsvDesc,
		dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart()
	);
}

void DirectXCommon::PreDraw() {
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	///=========================================
	//	TransitionBarrierの設定
	///=========================================
	D3D12_RESOURCE_BARRIER barrier{};
	barrier = barrierTransition_(
		D3D12_RESOURCE_STATE_RENDER_TARGET, swapChainResources_[backBufferIndex].Get()
	);

	commandList_->ResourceBarrier(1, &barrier);

	ClearRenderTarget();

	//ビューポートの設定
	D3D12_VIEWPORT viewPort{};
	viewPort.Width = static_cast<float>( window_->getWidth() );
	viewPort.Height = static_cast<float>( window_->getHeight() );
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	commandList_->RSSetViewports(1, &viewPort);

	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.right = window_->getWidth();
	scissorRect.top = 0;
	scissorRect.bottom = window_->getHeight();

	commandList_->RSSetScissorRects(1, &scissorRect);
}

void DirectXCommon::PostDraw() {
	HRESULT hr;

	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	///===============================================================
	///	バリアの更新(描画->表示状態)
	///===============================================================
	D3D12_RESOURCE_BARRIER barrier{};
	barrier = barrierTransition_(
		D3D12_RESOURCE_STATE_PRESENT, swapChainResources_[backBufferIndex].Get()
	);

	commandList_->ResourceBarrier(1, &barrier);
	//----------------------------------------------------
	///===============================================================

	// コマンドの受付終了 -----------------------------------
	hr = commandList_->Close();
	//----------------------------------------------------

	ExecuteCommandList();

	///===============================================================
	/// コマンドリストの実行を待つ
	///===============================================================
	Wait4ExecuteCommand();

	///===============================================================

	///===============================================================
	/// リセット
	///===============================================================
	ResetCommand();
	///===============================================================
}

D3D12_RESOURCE_BARRIER DirectXCommon::ResourceBarrierTransition::operator()(D3D12_RESOURCE_STATES stateAfter, ID3D12Resource* barrierTarget) {
	D3D12_RESOURCE_BARRIER barrier{};
	//Barrier を Transition に変更
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// バリアを張る対象のリソース
	barrier.Transition.pResource = barrierTarget;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る前(現在)のResourceState
	barrier.Transition.StateBefore = currentState_;
	// 遷移後のResourceState
	barrier.Transition.StateAfter = stateAfter;
	currentState_ = stateAfter;

	return barrier;
}