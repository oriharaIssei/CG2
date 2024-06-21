#pragma once

#include <memory>
#include <wrl.h>

#include <unordered_map>

#include <d3d12.h>

#include <string>

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

class MaterialManager;
class Material {
	friend class MaterialManager;
public:
	void Finalize();

	void SetForRootParameter(ID3D12GraphicsCommandList *cmdList,UINT rootParameterNum)const;
private:
	void Init();
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	ConstBufferMaterial *mappingData_ = nullptr;
};

class MaterialManager {
public:
	std::shared_ptr<Material> Create(const std::string &materialName);
	std::shared_ptr<Material> Create(const std::string &materialName,const MaterialData &data);

	void Finalize();
private:
	std::unordered_map<std::string,std::shared_ptr<Material>> materialPallete_;
public:
	const Material *getMaterial(const std::string &materialName) const {
		auto it = materialPallete_.find(materialName);
		if(it != materialPallete_.end()) {
			return it->second.get();
		} else {
			// キーが存在しない場合の処理
			return nullptr; // または適切なエラー処理を行う
		}
	}
	void Edit(const std::string &materialName,const MaterialData &data);
	void EditColor(const std::string &materialName,const Vector4 &color);
	void EditUvTransform(const std::string &materialName,const Transform &transform);
	void EditEnableLighting(const std::string &materialName,bool enableLighting);
};