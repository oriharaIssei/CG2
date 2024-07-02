#pragma once

#include <wrl.h>

#include <d3d12.h>

class DXDebug {
public:
	DXDebug();
	~DXDebug();
private:
	Microsoft::WRL::ComPtr <ID3D12Debug1> debugController_ = nullptr;
};