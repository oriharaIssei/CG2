#pragma once

#include "d3d12.h"
#include "wrl.h"
#include <variant>

#include "Light.h"

#include "Matrix4x4.h"
#include "stdint.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

class DirectXCommon;
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
enum class ShapeType {
	NONE,
	TRIANGLE,
	QUAD,
	SPHERE
};
struct TextureVertexData {
	Vector4 pos;
	Vector2 texCoord;
	Vector3 normal;
	TextureVertexData *operator=(const TextureVertexData &vert) {
		this->pos = vert.pos;
		this->texCoord = vert.texCoord;
		this->normal = vert.normal;
		return this;
	}
};
struct PrimitiveVertexData {
	Vector4 pos;
	Vector3 normal;
	PrimitiveVertexData(const TextureVertexData &vert) {
		this->pos = vert.pos;
		this->normal = vert.normal;
	}
	PrimitiveVertexData *operator=(const PrimitiveVertexData &vert) {
		this->pos = vert.pos;
		this->normal = vert.normal;
		return this;
	}
	PrimitiveVertexData *operator=(const TextureVertexData &vert) {
		this->pos = vert.pos;
		this->normal = vert.normal;
		return this;
	}
};

/// <summary>
/// 基本的なバッファ
/// </summary>
class CommonBuffer {
public:
	~CommonBuffer();
	static void Init(DirectXCommon *dxCommon);
	static void Finalize();

	/// <summary>
	/// ShapeType による 初期化( None は 何もしない )
	/// </summary>
	void Create(DirectXCommon *dxCommon,bool isTexture, ShapeType type);

	static TransformMatrix *matrixData;
	static Material *materialData;
	static DirectionalLight *lightData;

	static Microsoft::WRL::ComPtr<ID3D12Resource> matrixBuff;
	static Microsoft::WRL::ComPtr<ID3D12Resource> materialBuff;
	static Microsoft::WRL::ComPtr<ID3D12Resource> lightBuff;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff = nullptr;

	TextureVertexData *textureVertData = nullptr;
	PrimitiveVertexData *primitiveVertData = nullptr;

	uint32_t *indexData = nullptr;

	D3D12_INDEX_BUFFER_VIEW ibView {};
	D3D12_VERTEX_BUFFER_VIEW vbView {};

};