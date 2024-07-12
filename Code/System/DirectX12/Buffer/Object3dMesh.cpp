#include "Object3dMesh.h"

#include <System.h>
#include "DXFunctionHelper.h"

void TextureObject3dMesh::Create(UINT vertexSize,UINT indexSize) {
	if(vertexSize != 0) {
		UINT vertDataSize = sizeof(TextureVertexData);

		DXFH::CreateBufferResource(System::getInstance()->getDXDevice(),vertBuff,vertDataSize * vertexSize);
		vertBuff->Map(0,nullptr,reinterpret_cast<void **>(&vertData));
		vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
		vbView.SizeInBytes = vertDataSize * vertexSize;
		vbView.StrideInBytes = vertDataSize;
	}

	if(indexSize) {
		DXFH::CreateBufferResource(System::getInstance()->getDXDevice(),indexBuff,sizeof(uint32_t) * indexSize);
		ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibView.SizeInBytes = sizeof(uint32_t) * indexSize;
		ibView.Format = DXGI_FORMAT_R32_UINT;
		indexBuff->Map(0,nullptr,reinterpret_cast<void **>(&indexData));
	}
}

void PrimitiveObject3dMesh::Create(UINT vertexSize,UINT indexSize) {
	UINT vertDataSize = sizeof(PrimitiveVertexData);

	if(vertexSize != 0) {
		DXFH::CreateBufferResource(System::getInstance()->getDXDevice(),vertBuff,sizeof(PrimitiveVertexData) * vertexSize);

		vertBuff->Map(0,nullptr,reinterpret_cast<void **>(&vertData));

		vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
		vbView.SizeInBytes = vertDataSize * vertexSize;
		vbView.StrideInBytes = vertDataSize;
	}

	if(indexSize != 0) {
		DXFH::CreateBufferResource(System::getInstance()->getDXDevice(),indexBuff,sizeof(uint32_t) * vertexSize);
		ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibView.SizeInBytes = sizeof(uint32_t) * indexSize;
		ibView.Format = DXGI_FORMAT_R32_UINT;
		indexBuff->Map(0,nullptr,reinterpret_cast<void **>(&indexData));
	}
}