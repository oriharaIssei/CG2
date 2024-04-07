#include "PrimitiveBuffer.h"

#include <System.h>

#include "DirectXCommon.h"

void PrimitiveBuffer::Init(DirectXCommon* dxCommon,UINT vertexCount, int vertNum) {
	size_t triangleBufferSize = sizeof(PosColor) * vertexCount;

	dxCommon->CreateBufferResource(vertBuff_, triangleBufferSize);
	vertBuff_->Map(
		0,
		nullptr,
		reinterpret_cast<void**>( &vertData )
	);

	vbView.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズは 頂点分
	vbView.SizeInBytes = triangleBufferSize;
	//1頂点当たりのサイズ
	vbView.StrideInBytes = sizeof(PosColor);

	vertexNum = vertNum;
	index = 0;
}

void PrimitiveBuffer::Finalize() {
	vertBuff_.Reset();
}