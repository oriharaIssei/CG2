#include "DirectXCommon.h"

#include <Logger.h>

DirectXCommon::~DirectXCommon() {
	if (dxgiFactory_!=nullptr) {
		dxgiFactory_->Release();
		dxgiFactory_ = nullptr;
	}
	
	if (useAdapter_ != nullptr) {
		useAdapter_->Release();
		useAdapter_ = nullptr;
	}
	if (device_ != nullptr) {
		device_->Release();
		device_ = nullptr;
	}
}

bool DirectXCommon::Init() {
	//HRESULT : windows系のエラーコード。
	//SUCCEEDEDマクロによって成功かどうかを判定できる
	HRESULT hr;
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
			useAdapter_, featureLevels[i], IID_PPV_ARGS(&device_)
		);
		//生成できたか確認
		if (SUCCEEDED(hr)) {
			Logger::OutputLog(std::format("FeatureLevel : {}\n", featrueLevelStrings[i]));
			break;//生成出来たらループを抜ける
		}
	}
	assert(device_ != nullptr);
	///================================================

	Logger::OutputLog("Complete create D3D12Device!!! \n");

	return true;
}