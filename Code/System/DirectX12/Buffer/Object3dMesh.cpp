#include "Object3dMesh.h"

#include <System.h>
#include "DXFunctionHelper.h"

void TextureObject3dMesh::Create(UINT vertexSize,UINT indexSize) {
	UINT vertDataSize = sizeof(TextureVertexData);
	UINT indexDataSize = sizeof(uint32_t);

	if(vertexSize != 0) {
		DXFH::CreateBufferResource(System::getInstance()->getDXDevice(),vertBuff,vertDataSize * vertexSize);

		vertBuff->Map(0,nullptr,reinterpret_cast<void **>(&vertData));

		vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
		vbView.SizeInBytes = vertDataSize * vertexSize;
		vbView.StrideInBytes = vertDataSize;
	}

	if(indexSize != 0) {
		DXFH::CreateBufferResource(System::getInstance()->getDXDevice(),indexBuff,indexDataSize * indexSize);
		ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibView.SizeInBytes = indexDataSize * indexSize;
		ibView.Format = DXGI_FORMAT_R32_UINT;
		indexBuff->Map(0,nullptr,reinterpret_cast<void **>(&indexData));
	}
}

void PrimitiveObject3dMesh::Create(UINT vertexSize,UINT indexSize) {
	UINT vertDataSize = sizeof(PrimitiveVertexData);
	UINT indexDataSize = sizeof(uint32_t);

	if(vertexSize != 0) {
		DXFH::CreateBufferResource(System::getInstance()->getDXDevice(),vertBuff,vertDataSize * vertexSize);

		vertBuff->Map(0,nullptr,reinterpret_cast<void **>(&vertData));

		vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
		vbView.SizeInBytes = vertDataSize * vertexSize;
		vbView.StrideInBytes = vertDataSize;
	}

	if(indexSize != 0) {
		DXFH::CreateBufferResource(System::getInstance()->getDXDevice(),indexBuff,indexDataSize * indexSize);
		ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibView.SizeInBytes = indexDataSize * indexSize;
		ibView.Format = DXGI_FORMAT_R32_UINT;
		indexBuff->Map(0,nullptr,reinterpret_cast<void **>(&indexData));
	}
}