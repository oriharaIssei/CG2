#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "Vector3.h"
#include "Vector4.h"

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;// ライトの向き
	float intensity;// 輝度
};

class LightBuffer {
public:
	void Init();
	void Finalize();
	void SetForRootParameter(ID3D12GraphicsCommandList *cmdList, UINT rootParameterNum)const;
	void ConvertToBuffer();

	Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 direction = { 1.0f,1.0f,1.0f };
	float intensity = 1.0f;
private:
	DirectionalLight *directionalLigh_;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
};