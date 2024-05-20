#pragma once

#include <d3d12.h>
#include <PipelineStateObj.h>

#include <Matrix4x4.h>
#include <Vector2.h>
#include <Vector4.h>

#include <memory>
#include <wrl.h>

#include <string>

#include <Object3dMesh.h>

class Sprite {
public:
	static void Init();
	static Sprite *Create(const Vector2 &pos, const Vector2 &size, const std::string &textureFilePath);
	static void PreDraw();
private:
	struct SpriteVertexData {
		Vector4 pos;
		Vector2 texcoord;
	};
	struct SpritConstBuffer {
		Vector4 color_;
		Matrix4x4 mat_;
	};
	struct SpriteMesh {
		void Init();
		SpriteVertexData *vertexData = nullptr;
		uint32_t *indexData = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff = nullptr;

		D3D12_INDEX_BUFFER_VIEW ibView {};
		D3D12_VERTEX_BUFFER_VIEW vbView {};
	};
	static Matrix4x4 ViewPortMat_;
	static void CreatePSO();
	static std::unique_ptr<PipelineStateObj> pso_;
public:
	void Draw();
private:
	std::unique_ptr<SpriteMesh> meshBuff_;

	Matrix4x4 worldMat_;

	SpritConstBuffer *mappingConstBufferData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;

	uint32_t th_;
public:
};