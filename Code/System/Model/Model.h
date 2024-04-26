#pragma once
#include <memory>
#include <vector>

#include <fstream>
#include <sstream>

#include <CommonBuffer.h>
#include <DirectXCommon.h>
#include <PipelineStateObj.h>

struct MaterialData {
	/// <summary>
	/// mtl file などから読み込んだ情報を保存するためのもの
	/// </summary>
	std::unique_ptr<int> textureNumber = nullptr;
};
struct ModelData {
	std::vector<TextureVertexData> vertices;
	MaterialData materialData;
	size_t dataSize;
	size_t vertSize;
};

class Model {
	friend class ModelManager;
public:
	static Model *Create(const std::string &directoryPath, const std::string &filename);
public:
	Model() = default;

	void Draw(const Matrix4x4 &world, const Matrix4x4 &view);
private:
	PipelineStateObj *usePso_ = nullptr;
	std::unique_ptr<CommonBuffer> buffer_;
	ModelData data_;
	bool isTexture_;
public:
	void setUv(const Matrix4x4 &uv);
};