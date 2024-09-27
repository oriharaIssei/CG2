#pragma once

#include <wrl.h>

#include <stdint.h>
#include <string>

#include "DXRtvArray.h"
#include "DXSrvArray.h"
#include "DXCommand.h"

#include "Vector2.h"
#include "Vector4.h"

#include "PipelineStateObj.h"

class RenderTexture{
public:
	RenderTexture(DXCommand* dxCom,DXRtvArray* rtvArray,DXSrvArray* srvArray):dxCommand_(dxCom),rtvArray_(rtvArray),srvArray_(srvArray){}
	~RenderTexture() = default;

	static void Awake();
	void Init(const Vector2& textureSize,DXGI_FORMAT format,const Vector4& _clearColor);

	void PreDraw();
	void PostDraw();

	void DrawTexture();
private:
	static PipelineStateObj* pso_;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

	DXCommand* dxCommand_;

	uint32_t rtvIndex_;
	uint32_t srvIndex_;

	DXRtvArray* rtvArray_;
	DXSrvArray* srvArray_;

	Vector2 textureSize_;
	Vector4 clearColor_;
};