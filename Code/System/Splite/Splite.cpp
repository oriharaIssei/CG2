#include "Splite.h"

#include "Matrix4x4.h"

#include <System.h>
#include "DirectXCommon.h"

void Splite::Init(DirectXCommon* dxCommon) {
	dxCommon->CreateBufferResource(vertBuff_, sizeof(VertexData) * 6);
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeof(VertexData) * 6;
	vbView_.StrideInBytes = sizeof(VertexData);

	dxCommon->CreateBufferResource(matrixBuff_, sizeof(Matrix4x4));
}

void Splite::Finalize() {
	vertBuff_.Reset();
	matrixBuff_.Reset();
}