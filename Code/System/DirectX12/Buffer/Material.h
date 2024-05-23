#pragma once

#include <DirectXCommon.h>

#include <wrl.h>

#include "Matrix4x4.h"
#include "stdint.h"
#include "Vector4.h"


struct ConstBufferMaterial {
	Vector4 color;
	uint32_t enableLighting;
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

struct MaterialData {
	Vector4 color;
	uint32_t enableLighting;
	Matrix4x4 uvTransform;
};

class Material {
public:
	void Init();
	void Finalize();
	void ConvertToBuffer();
	void Update();
	void UpdateMatrix();

	void SetForRootParameter(ID3D12GraphicsCommandList *cmdList,UINT rootParameterNum)const;

	Transform uvTransformData = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	MaterialData materialData;
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	ConstBufferMaterial *mappingData_ = nullptr;
};