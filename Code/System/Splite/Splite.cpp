#include "Splite.h"

#include "Matrix4x4.h"

#include <System.h>
#include "DirectXCommon.h"

void Splite::Init(DirectXCommon* dxCommon) {
	dxCommon->CreateBufferResource(vertBuff, sizeof(VertexData) * ( 16 * 16 * 6 ));
	vertBuff->Map(0, nullptr, reinterpret_cast<void**>( &vertData ));

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexData) * ( 16 * 16 * 6 );
	vbView.StrideInBytes = sizeof(VertexData);

	dxCommon->CreateBufferResource(indexBuff, sizeof(uint32_t) * ( 16 * 16 * 6 ));
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.SizeInBytes = sizeof(uint32_t) * ( 16 * 16 * 6 );
	ibView.Format = DXGI_FORMAT_R32_UINT;
	indexBuff->Map(0, nullptr, reinterpret_cast<void**>( &indexData ));

	dxCommon->CreateBufferResource(matrixBuff, sizeof(TransformMatrix));
	matrixBuff->Map(0, nullptr, reinterpret_cast<void**>( &matrixData ));
	*matrixData = { MakeMatrix::Identity(),MakeMatrix::Identity() };

	dxCommon->CreateBufferResource(materialBuff, sizeof(Material));
	materialBuff->Map(0, nullptr, reinterpret_cast<void**>( &materialData ));
	*materialData = { {1.0f,1.0f,1.0f,1.0f },true };

	dxCommon->CreateBufferResource(lightBuff, sizeof(DirectionalLight));
	lightBuff->Map(0, nullptr, reinterpret_cast<void**>( &lightData ));
	*lightData = { { 1.0f,1.0f,1.0f,1.0f },{ 0.0f,-1.0f,0.0f },1.0f };
}

void Splite::Finalize() {
	vertBuff.Reset();
	matrixBuff.Reset();
	materialBuff.Reset();
	lightBuff.Reset();
	indexBuff.Reset();
}