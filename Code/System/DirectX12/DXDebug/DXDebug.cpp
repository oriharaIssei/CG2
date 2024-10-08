#include "DXDebug.h"

#include <dxgi1_6.h>
#include <dxgidebug.h>

DXDebug::DXDebug(){
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	//デバッグレイヤーをオンに
	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))){
		//デバッグレイヤーの有効化
		debugController->EnableDebugLayer();
		//GPU側でもデバッグさせる
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // DEBUG 
}

DXDebug::~DXDebug(){
#ifdef _DEBUG
	IDXGIDebug1 *debug;
	if(SUCCEEDED(DXGIGetDebugInterface1(0,IID_PPV_ARGS(&debug)))){
		debug->ReportLiveObjects(DXGI_DEBUG_ALL,DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP,DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12,DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
#endif // DEBUG 
}