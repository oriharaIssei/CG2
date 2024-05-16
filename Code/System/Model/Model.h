#pragma once
#include <memory>
#include <vector>

#include <fstream>
#include <sstream>

#include <DirectXCommon.h>
#include <PipelineStateObj.h>

#include <Object3dMesh.h>
#include <ViewProjection.h>
#include <WorldTransform.h>

struct ModelMtl {
	/// <summary>
	/// mtl file などから読み込んだ情報を保存するためのもの
	/// </summary>
	std::unique_ptr<int> textureNumber = nullptr;
};
struct ModelData {
	std::vector<TextureVertexData> vertices;
	ModelMtl materialData;
	size_t dataSize;
	size_t vertSize;
};

class ModelManager;
class Model {
	friend class ModelManager;
public:
	static Model *Create(const std::string &directoryPath, const std::string &filename);
	static void Init();
private:
	static std::unique_ptr<Matrix4x4> fovMa_;
public:
	Model() = default;

	void Draw(const WorldTransform &world, const ViewProjection &view);
private:
	PipelineStateObj *usePso_ = nullptr;
	ModelData data_;
	std::unique_ptr<IObject3dMesh> meshBuff_;
	bool isTexture_;
};