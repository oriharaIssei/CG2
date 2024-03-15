#include "TriangleBufferVerw.h"

#include <Vector4.h>

#include "DirectXCommon.h"

void TriangleBufferVerw::Init(DirectXCommon* dxCommon) {
	constexpr size_t triangleBufferSize = sizeof(Vector4) * 3;
	dxCommon->CreateBufferResource(vertBuff_, triangleBufferSize);

	vbView.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズは 頂点 3つ分
	vbView.SizeInBytes = triangleBufferSize;
	//1頂点当たりのサイズ
	vbView.StrideInBytes = sizeof(Vector4);
}

void TriangleBufferVerw::Finalize() {
	vertBuff_->Release();
}