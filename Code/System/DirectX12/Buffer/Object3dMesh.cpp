#include "Object3dMesh.h"

#include <System.h>

void TextureObject3dMesh::Create(UINT vertexSize) {
	UINT vertDataSize= sizeof(TextureVertexData);
	System::getInstance()->getDxCommon()->CreateBufferResource(vertBuff, vertDataSize * vertexSize);

	System::getInstance()->getDxCommon()->CreateBufferResource(vertBuff, sizeof(TextureVertexData) * vertexSize);
	vertBuff->Map(0, nullptr, reinterpret_cast<void **>(&vertData));
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = vertDataSize * vertexSize;
	vbView.StrideInBytes = vertDataSize;

	System::getInstance()->getDxCommon()->CreateBufferResource(indexBuff, sizeof(uint32_t) * vertexSize);
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.SizeInBytes = sizeof(uint32_t) * vertexSize;
	ibView.Format = DXGI_FORMAT_R32_UINT;
	indexBuff->Map(0, nullptr, reinterpret_cast<void **>(&indexData));
}

void PrimitiveObject3dMesh::Create(UINT vertexSize) {
	UINT vertDataSize = sizeof(PrimitiveVertexData);

	System::getInstance()->getDxCommon()->CreateBufferResource(vertBuff, sizeof(PrimitiveVertexData) * vertexSize);

	vertBuff->Map(0, nullptr, reinterpret_cast<void **>(&vertData));
	
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = vertDataSize * vertexSize;
	vbView.StrideInBytes = vertDataSize;

	System::getInstance()->getDxCommon()->CreateBufferResource(indexBuff, sizeof(uint32_t) * vertexSize);
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.SizeInBytes = sizeof(uint32_t) * vertexSize;
	ibView.Format = DXGI_FORMAT_R32_UINT;
	indexBuff->Map(0, nullptr, reinterpret_cast<void **>(&indexData));
}
