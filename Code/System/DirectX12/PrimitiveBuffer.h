#pragma once

#include <wrl.h>

#include <d3d12.h>

#include "Vector3.h"
#include "Vector4.h"

struct PosColor {
	Vector4 pos;
	Vector4 color;
};
class DirectXCommon;
struct PrimitiveBuffer {
	void Init(DirectXCommon* dxCommon,UINT vertexCount,int vertNum);
	void Finalize();

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	
	PosColor* vertData;

	int index;
	int vertexNum;
};