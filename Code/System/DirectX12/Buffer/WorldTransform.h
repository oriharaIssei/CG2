#pragma once

#include <DirectXCommon.h>
#include <wrl.h>

#include <array>

#include <functional>

#include <Matrix4x4.h>
#include <Transform.h>

struct ConstantBufferWorldMatrix {
	Matrix4x4 world;
};
class WorldTransform {
public:
	void Init();
	void Finalize();
	void Update();
	void ConvertToBuffer();
	void SetForRootParameter(ID3D12GraphicsCommandList *cmdList, UINT rootParameterNum)const;
	Transform transformData;
	Matrix4x4 worldMat;

	WorldTransform *parent = nullptr;
private:
	ConstantBufferWorldMatrix *mappingWorldMat_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> buff_;
};