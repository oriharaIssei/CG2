#pragma once

#include <wrl.h>

#include <d3d12.h>

#include <stdint.h>
#include "Vector3.h"
#include "Vector4.h"

struct PosColor {
	Vector4 pos;
	Vector4 color;
};

enum PrimitiveType {
	Triangle,
	Quad
};

class DirectXCommon;
struct PrimitiveBuffer {
	void Init(DirectXCommon* dxCommon,PrimitiveType type);
	void Finalize();

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;
	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView{};

	PosColor* vertData = nullptr;
	uint32_t* indexData = nullptr;

	int index;
	int vertexNum;
};