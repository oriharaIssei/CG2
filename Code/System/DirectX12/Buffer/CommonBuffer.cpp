#include "CommonBuffer.h"

#include "Matrix4x4.h"

#include "DirectXCommon.h"
#include <System.h>

TransformMatrix *CommonBuffer::matrixData = nullptr;
Material *CommonBuffer::materialData = nullptr;
DirectionalLight *CommonBuffer::lightData = nullptr;
Microsoft::WRL::ComPtr<ID3D12Resource> CommonBuffer::matrixBuff = nullptr;
Microsoft::WRL::ComPtr<ID3D12Resource> CommonBuffer::materialBuff = nullptr;
Microsoft::WRL::ComPtr<ID3D12Resource> CommonBuffer::lightBuff = nullptr;

CommonBuffer::~CommonBuffer() {
	vertBuff.Reset();
	indexBuff.Reset();
}

void CommonBuffer::Init(DirectXCommon *dxCommon) {
	dxCommon->CreateBufferResource(materialBuff, sizeof(Material));
	materialBuff->Map(0, nullptr, reinterpret_cast<void **>(&materialData));
	materialData[0].color = { 1.0f,1.0f,1.0f,1.0f };
	materialData[0].enableLighting = false;
	materialData[0].uvTransform = MakeMatrix::Identity();

	dxCommon->CreateBufferResource(lightBuff, sizeof(DirectionalLight));
	lightBuff->Map(0, nullptr, reinterpret_cast<void **>(&lightData));
	*lightData = { { 1.0f,1.0f,1.0f,1.0f },{ 0.0f,-1.0f,0.0f },1.0f };

	dxCommon->CreateBufferResource(matrixBuff, sizeof(TransformMatrix));
	matrixBuff->Map(0, nullptr, reinterpret_cast<void **>(&matrixData));
	*matrixData = { MakeMatrix::Identity(),MakeMatrix::Identity() };
}

void CommonBuffer::Create(DirectXCommon *dxCommon, bool isTexture, ShapeType type) {

	size_t vertexDataSize = isTexture ? sizeof(TextureVertexData) : sizeof(PrimitiveVertexData);

	switch(type) {
	case ShapeType::NONE:
	break;
	case ShapeType::TRIANGLE:

	dxCommon->CreateBufferResource(vertBuff, vertexDataSize * 3);
	if(isTexture) {
		vertBuff->Map(0, nullptr, reinterpret_cast<void **>(&textureVertData));
	} else {
		vertBuff->Map(0, nullptr, reinterpret_cast<void **>(&primitiveVertData));
	}
	
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = vertexDataSize * 3;
	vbView.StrideInBytes = vertexDataSize;

	indexBuff = nullptr;
	break;
	case ShapeType::QUAD:
	dxCommon->CreateBufferResource(vertBuff, vertexDataSize * 6);

	if(isTexture) {
		vertBuff->Map(0, nullptr, reinterpret_cast<void **>(&textureVertData));
	} else {
		vertBuff->Map(0, nullptr, reinterpret_cast<void **>(&primitiveVertData));
	}

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = vertexDataSize * 6;
	vbView.StrideInBytes = vertexDataSize;

	dxCommon->CreateBufferResource(indexBuff, sizeof(uint32_t) * 6);
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.SizeInBytes = sizeof(uint32_t) * 6;
	ibView.Format = DXGI_FORMAT_R32_UINT;
	indexBuff->Map(0, nullptr, reinterpret_cast<void **>(&indexData));
	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;

	indexData[3] = 1;
	indexData[4] = 3;
	indexData[5] = 2;
	break;
	case ShapeType::SPHERE:
	dxCommon->CreateBufferResource(vertBuff, vertexDataSize * (16 * 16 * 6));

	if(isTexture) {
		vertBuff->Map(0, nullptr, reinterpret_cast<void **>(&textureVertData));
	} else {
		vertBuff->Map(0, nullptr, reinterpret_cast<void **>(&primitiveVertData));
	}

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = vertexDataSize * (16 * 16 * 6);
	vbView.StrideInBytes = vertexDataSize;

	dxCommon->CreateBufferResource(indexBuff, sizeof(uint32_t) * (16 * 16 * 6));
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.SizeInBytes = sizeof(uint32_t) * (16 * 16 * 6);
	ibView.Format = DXGI_FORMAT_R32_UINT;
	indexBuff->Map(0, nullptr, reinterpret_cast<void **>(&indexData));
	break;
	default:
	break;
	}
}

void CommonBuffer::Finalize() {
	matrixBuff.Reset();
	materialBuff.Reset();
	lightBuff.Reset();
}