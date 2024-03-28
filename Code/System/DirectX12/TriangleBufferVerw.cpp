#include "TriangleBufferVerw.h"

#include <System.h>

#include "DirectXCommon.h"

void TriangleBufferVerw::Init(DirectXCommon* dxCommon) {
	constexpr size_t triangleBufferSize = sizeof(VertexData) * 6;
	dxCommon->CreateBufferResource(vertBuff_, triangleBufferSize);

	vbView.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズは 頂点 3つ分
	vbView.SizeInBytes = triangleBufferSize;
	//1頂点当たりのサイズ
	vbView.StrideInBytes = sizeof(VertexData);
}

void TriangleBufferVerw::Finalize() {
	vertBuff_.Reset();
}