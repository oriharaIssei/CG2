#include "DXFunctionHelper.h"

#include <cassert>

void DXFunctionHelper::CreateBufferResource(const DXDevice* device,Microsoft::WRL::ComPtr<ID3D12Resource>& resource,size_t sizeInBytes){
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

	HRESULT hr = device->getDevice()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr<ID3D12Resource> DXFunctionHelper::CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device,uint32_t width,uint32_t height,DXGI_FORMAT format,const Vector4& clearColor){
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource;
	D3D12_RESOURCE_DESC resourceDesc;
	// RenderTarget として 利用可能に
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_HEAP_PROPERTIES heapProps{};
	// VRAM 上に 生成
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = format;
	clearValue.Color[0] = clearColor.x;
	clearValue.Color[1] = clearColor.y;
	clearValue.Color[2] = clearColor.z;
	clearValue.Color[3] = clearColor.w;

	device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET, // 描画すること を 前提とした テクスチャ なので RenderTarget として 扱う
		&clearValue,
		IID_PPV_ARGS(&renderTextureResource)
	);

	return renderTextureResource;
}

void DXFunctionHelper::ClearRenderTarget(const DXCommand* command,const DXSwapChain* swapChain){
	UINT backBufferIndex = swapChain->getCurrentBackBufferIndex();

	DXHeap* heap = DXHeap::getInstance();

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = heap->getDsvHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRtvHandle = heap->getRtvCpuHandle(backBufferIndex);
	command->getCommandList()->OMSetRenderTargets(
		1,
		&backBufferRtvHandle,
		false,
		&dsvHandle
	);

	float clearColor[] = {0.1f,0.25f,0.5f,1.0f};
	command->getCommandList()->ClearRenderTargetView(
		backBufferRtvHandle,clearColor,0,nullptr
	);

	command->getCommandList()->ClearDepthStencilView(
		dsvHandle,D3D12_CLEAR_FLAG_DEPTH,1.0f,0,0,nullptr
	);
}

void DXFunctionHelper::SetViewportsAndScissor(const DXCommand* dxCommand,const WinApp* window){
	ID3D12GraphicsCommandList* commandList = dxCommand->getCommandList();
	//ビューポートの設定
	D3D12_VIEWPORT viewPort{};
	viewPort.Width = static_cast<float>(window->getWidth());
	viewPort.Height = static_cast<float>(window->getHeight());
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	commandList->RSSetViewports(1,&viewPort);

	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.right = window->getWidth();
	scissorRect.top = 0;
	scissorRect.bottom = window->getHeight();

	commandList->RSSetScissorRects(1,&scissorRect);
}

void DXFunctionHelper::SetRenderTargets(const DXCommand* dxCommand,const DXSwapChain* dxSwapChain){
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = DXHeap::getInstance()->getRtvCpuHandle(dxSwapChain->getCurrentBackBufferIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DXHeap::getInstance()->getDsvCpuHandle(0);
	dxCommand->getCommandList()->OMSetRenderTargets(1,&rtvHandle,FALSE,&dsvHandle);
}

void DXFunctionHelper::PreDraw(const DXCommand* command,const WinApp* window,const DXSwapChain* swapChain){
	///=========================================
	//	TransitionBarrierの設定
	///=========================================
	ID3D12GraphicsCommandList* commandList = command->getCommandList();

	D3D12_RESOURCE_BARRIER barrier{};
	barrier = ResourceBarrierManager::Barrier(
		swapChain->getCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	commandList->ResourceBarrier(1,&barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = DXHeap::getInstance()->getRtvCpuHandle(swapChain->getCurrentBackBufferIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DXHeap::getInstance()->getDsvCpuHandle(0);
	commandList->OMSetRenderTargets(1,&rtvHandle,FALSE,&dsvHandle);

	//ビューポートの設定
	D3D12_VIEWPORT viewPort{};
	viewPort.Width = static_cast<float>(window->getWidth());
	viewPort.Height = static_cast<float>(window->getHeight());
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	commandList->RSSetViewports(1,&viewPort);

	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.right = window->getWidth();
	scissorRect.top = 0;
	scissorRect.bottom = window->getHeight();

	commandList->RSSetScissorRects(1,&scissorRect);

	ClearRenderTarget(command,swapChain);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DXFunctionHelper::PostDraw(DXCommand* dxCommand,DXFence* fence,DXSwapChain* swapChain){
	HRESULT hr;
	///===============================================================
	///	バリアの更新(描画->表示状態)
	///===============================================================
	D3D12_RESOURCE_BARRIER barrier{};
	barrier = ResourceBarrierManager::Barrier(
		swapChain->getCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT
	);

	dxCommand->getCommandList()->ResourceBarrier(1,&barrier);
	///===============================================================

	// コマンドの受付終了 -----------------------------------
	hr = dxCommand->getCommandList()->Close();
	//----------------------------------------------------

	///===============================================================
	/// コマンドリストの実行
	///===============================================================
	ID3D12CommandList* ppHeaps[] = {dxCommand->getCommandList()};
	dxCommand->getCommandQueue()->ExecuteCommandLists(1,ppHeaps);
	///===============================================================

	swapChain->Present();

	///===============================================================
	/// コマンドリストの実行を待つ
	///===============================================================
	fence->Signal(dxCommand->getCommandQueue());
	fence->WaitForFence();
	///===============================================================

	///===============================================================
	/// リセット
	///===============================================================
	dxCommand->CommandReset();
	///===============================================================
}