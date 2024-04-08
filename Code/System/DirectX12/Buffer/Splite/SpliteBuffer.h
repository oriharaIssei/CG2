#pragma once

#include "wrl.h"
#include "d3d12.h"

#include "stdint.h"
#include "Light.h"

#include "Matrix4x4.h"
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"

#include "PrimitiveBuffer.h"

class DirectXCommon;
struct VertexData {
	Vector4 pos;
	Vector2 texCoord;
	Vector3 normal;
};
struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];// 下記を参照
	Matrix4x4 uvTransform;
	/*
		< パディング >
	機会に都合のいいような
	c++とhlslのメモリ配置の違いによる誤差のようなもの。

	c++ :
	color			: [][][][]
	enableLighting	: []
	uvTransform		: [][][]float1
					  [][][]float2
					  [][][]float3
	しかし、hlslでは
	hlsl :
	color			: [][][][]
	enableLighting	: []<><><>
	uvTransform		: [][][]<>float1
					  [][][]<>float2
					  [][][]<>float3
	(<>は実際には使われないメモリ)
	となっているらしい。
	この誤差を埋めるためにc++側で隙間のメモリを上手く埋める。
	*/
	
};
struct TransformMatrix {
	Matrix4x4 wvp;
	Matrix4x4 world;
};
struct SpliteBuffer {
	void Init(DirectXCommon* dxCommon,PrimitiveType type);
	void Finalize();

	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuff = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> matrixBuff = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> lightBuff = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff = nullptr;
	VertexData* vertData = nullptr;
	TransformMatrix* matrixData = nullptr;
	Material* materialData = nullptr;
	DirectionalLight* lightData = nullptr;
	uint32_t* indexData = nullptr;

	D3D12_INDEX_BUFFER_VIEW ibView{};
	D3D12_VERTEX_BUFFER_VIEW vbView{};
};