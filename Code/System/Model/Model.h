#pragma once

#include <memory>
#include <vector>

#include <fstream>
#include <sstream>
#include <string>

#include "DXCommand.h"
#include <PipelineStateObj.h>

#include "Material.h"
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
	std::unique_ptr<IObject3dMesh> meshBuff_;
	PipelineStateObj *usePso_ = nullptr;

	ModelMtl materialData;

	size_t dataSize;
	size_t vertSize;
};

class ModelManager;
class Model {
	friend class ModelManager;
public:
	static std::shared_ptr<Model> Create(const std::string &directoryPath,const std::string &filename);
	static void Init();
	static void Finalize();
private:
	static uint32_t drawCount_;

	static std::unique_ptr<ModelManager> manager_;

	static std::unique_ptr<DXCommand> dxCommand_;

	static std::unique_ptr<Matrix4x4> fovMa_;
public:
	Model() = default;

	void Draw(const WorldTransform &world,const ViewProjection &view,const Material *material);
private:
	void NotDraw(const WorldTransform &world,const ViewProjection &view,const Material *material) {
		world; view; material;
	}
	void DrawThis(const WorldTransform &world,const ViewProjection &view,const Material *material);
private:
	std::vector<std::unique_ptr<ModelData>> data_;

	std::string directory_;
	std::string fileName_;

	enum class LoadState {
		Loading,
		Loaded,
	};
	LoadState currentState_;
	std::array<std::function<void(const WorldTransform &,const ViewProjection &,const Material *)>,2> drawFuncTable_ = {
		[this](const WorldTransform &world,const ViewProjection &view,const Material *material) { NotDraw(world,view,material); },
		[this](const WorldTransform &world,const ViewProjection &view,const Material *material) { DrawThis(world,view,material); }
	};
public:
	const std::string &getDirectory()const { return directory_; }
	const std::string &getName()const { return fileName_; }
};