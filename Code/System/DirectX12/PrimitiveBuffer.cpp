#include "PrimitiveBuffer.h"

#include <System.h>

#include "DirectXCommon.h"

void PrimitiveBuffer::Init(DirectXCommon* dxCommon, PrimitiveType type) {
	size_t buffSize;
	switch(type) {
	case Triangle:
		buffSize = sizeof(PosColor) * 3;

		dxCommon->CreateBufferResource(vertBuff, buffSize);
		vertBuff->Map(
			0,
			nullptr,
			reinterpret_cast<void**>( &vertData )
		);

		vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
		//使用するリソースのサイズは 頂点分
		vbView.SizeInBytes = buffSize;
		//1頂点当たりのサイズ
		vbView.StrideInBytes = sizeof(PosColor);

		dxCommon->CreateBufferResource(indexBuff, sizeof(uint32_t) * 3);
		ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibView.SizeInBytes = sizeof(uint32_t) * 3;
		ibView.Format = DXGI_FORMAT_R32_UINT;

		indexBuff->Map(0, nullptr, reinterpret_cast<void**>( &indexData ));
		indexData[0] = 0;
		indexData[1] = 1;
		indexData[2] = 2;
		vertexNum = 3;
		index = 0;

		break;
	case Quad:
		buffSize = sizeof(PosColor) * 6;

		dxCommon->CreateBufferResource(vertBuff, buffSize);
		vertBuff->Map(
			0,
			nullptr,
			reinterpret_cast<void**>( &vertData )
		);

		vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
		//使用するリソースのサイズは 頂点分
		vbView.SizeInBytes = buffSize;
		//1頂点当たりのサイズ
		vbView.StrideInBytes = sizeof(PosColor);

		dxCommon->CreateBufferResource(indexBuff, sizeof(uint32_t) * 6);
		ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibView.SizeInBytes = sizeof(uint32_t) * 6;
		ibView.Format = DXGI_FORMAT_R32_UINT;

		indexBuff->Map(0, nullptr, reinterpret_cast<void**>( &indexData ));
		indexData[0] = 0;
		indexData[1] = 1;
		indexData[2] = 2;

		indexData[3] = 1;
		indexData[4] = 3;
		indexData[5] = 2;

		vertexNum = 6;
		index = 0;
		break;
	default:
		break;
	}
}

void PrimitiveBuffer::Finalize() {
	vertBuff.Reset();
	indexBuff.Reset();
}