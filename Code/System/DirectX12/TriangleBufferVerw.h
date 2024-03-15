#pragma once

#include <wrl.h>

#include <d3d12.h>

class DirectXCommon;
struct TriangleBufferVerw {
	void Init(DirectXCommon* dxCommon);
	void Finalize();

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// インデックスバッファ
	//Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// インデックスバッファビュー
	//D3D12_INDEX_BUFFER_VIEW ibView{};
};