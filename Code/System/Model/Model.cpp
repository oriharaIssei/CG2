#include "Model.h"

#include <cassert>

#include "TextureManager.h"
#include <System.h>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <stdint.h>

std::unique_ptr<Matrix4x4> Model::fovMa_ = nullptr;
std::unique_ptr<ModelManager> Model::manager_ = nullptr;

#pragma region"ModelManager"
class ModelManager {
public:
	void Create(Model *model,const std::string &directoryPath,const std::string &filename);
	void Init();
	void Finalize();
private:
	void LoadLoop();

	void LoadObjFile(std::vector<std::unique_ptr<ModelData>> &data,const std::string &directoryPath,const std::string &filename);
	ModelMtl LoadMtlFile(const std::string &directoryPath,const std::string &filename,const std::string &materialName);
	void ProcessMeshData(std::unique_ptr<ModelData> &modelData,const std::vector<TextureVertexData> &vertices);
private:
	std::thread loadingThread_;
	std::queue<std::tuple<Model *,std::string,std::string>> loadingQueue_;
	std::mutex queueMutex_;
	std::condition_variable queueCondition_;
	bool stopLoadingThread_;
};

void ModelManager::Create(Model *model,const std::string &directoryPath,const std::string &filename) {
	{
		std::unique_lock<std::mutex> lock(queueMutex_);
		loadingQueue_.emplace(model,directoryPath,filename);
	}
	queueCondition_.notify_one();//threadに通知
}

void ModelManager::Init() {
	stopLoadingThread_ = false;
	loadingThread_ = std::thread(&ModelManager::LoadLoop,this);
}

void ModelManager::Finalize() {
	{
		std::unique_lock<std::mutex> lock(queueMutex_);
		stopLoadingThread_ = true;
	}
	queueCondition_.notify_all();
	if(loadingThread_.joinable()) {
		loadingThread_.join();
	}
}

void ModelManager::LoadLoop() {
	while(true) {
		std::tuple<Model *,std::string,std::string> task;
		{
			std::unique_lock<std::mutex> lock(queueMutex_);
			queueCondition_.wait(lock,[this] { return !loadingQueue_.empty() || stopLoadingThread_; });

			if(stopLoadingThread_ && loadingQueue_.empty()) {
				return;
			}

			task = loadingQueue_.front();
			loadingQueue_.pop();
		}

		Model *model = std::get<0>(task);
		model->currentState_ = Model::LoadState::Loading;
		LoadObjFile(model->data_,std::get<1>(task),std::get<2>(task));
		model->currentState_ = Model::LoadState::Loaded;
	}
}

void ModelManager::LoadObjFile(std::vector<std::unique_ptr<ModelData>> &data,const std::string &directoryPath,const std::string &filename) {
	// 変数の宣言
	std::vector<Vector4> poss;
	std::vector<Vector3> normals;
	std::vector<Vector2> texCoords;
	std::string line;
	std::vector<TextureVertexData> vertices;
	std::string currentMaterial;
	bool firstObject = true;

	// ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	// ファイル読み込み
	while(std::getline(file,line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if(identifier == "v") {
			Vector4 pos;
			s >> pos.x >> pos.y >> pos.z;
			pos.x *= -1.0f;
			pos.w = 1.0f;
			poss.push_back(pos);
		} else if(identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texCoords.push_back(texcoord);
		} else if(identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if(identifier == "f") {
			TextureVertexData triangle[3];
			for(int32_t faceVert = 0; faceVert < 3; ++faceVert) {
				std::string vertDefinition;
				s >> vertDefinition;
				std::istringstream v(vertDefinition);
				uint32_t elementIndices[3];

				for(int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v,index,'/');
					if(index.empty()) {
						elementIndices[element] = 0;
						continue;
					}
					elementIndices[element] = std::stoi(index);
				}

				Vector4 position = poss[elementIndices[0] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				Vector2 texCoord;
				if(elementIndices[1] == 0) {
					texCoord = {0.0f,0.0f};
				} else {
					texCoord = texCoords[elementIndices[1] - 1];
				}

				triangle[faceVert] = {position,texCoord,normal};
			}
			vertices.push_back(triangle[2]);
			vertices.push_back(triangle[1]);
			vertices.push_back(triangle[0]);
		} else if(identifier == "mtllib") {
			std::string materialFileName;
			s >> currentMaterial;
		} else if(identifier == "usemtl") {
			std::string materialName;
			s >> materialName;
			if(!data.empty()) {
				data.back()->materialData = LoadMtlFile(directoryPath,currentMaterial,materialName);
			}
		} else if(identifier == "o") {
			if(!firstObject) {
				ProcessMeshData(data.back(),vertices);
				vertices.clear();
			}
			firstObject = false;
			data.push_back(std::make_unique<ModelData>());
		}
	}
	// 最後のメッシュデータを処理
	if(!vertices.empty()) {
		ProcessMeshData(data.back(),vertices);
	}
}

void ModelManager::ProcessMeshData(std::unique_ptr<ModelData> &modelData,const std::vector<TextureVertexData> &vertices) {
	if(modelData->materialData.textureNumber != nullptr) {
		TextureObject3dMesh *textureMesh = new TextureObject3dMesh();
		modelData->usePso_ = System::getInstance()->getTexturePso();

		modelData->dataSize = sizeof(TextureVertexData) * vertices.size();

		textureMesh->Create(vertices.size());
		memcpy(textureMesh->vertData,vertices.data(),vertices.size() * sizeof(TextureVertexData));
		modelData->meshBuff_.reset(textureMesh);
	} else {
		PrimitiveObject3dMesh *primitiveMesh = new PrimitiveObject3dMesh();
		modelData->usePso_ = System::getInstance()->getPrimitivePso();

		std::vector<PrimitiveVertexData> primVert;
		for(auto &texVert : vertices) {
			primVert.push_back(PrimitiveVertexData(texVert));
		}

		modelData->dataSize = sizeof(PrimitiveVertexData) * primVert.size();

		primitiveMesh->Create(primVert.size());
		memcpy(primitiveMesh->vertData,primVert.data(),primVert.size() * sizeof(PrimitiveVertexData));
		modelData->meshBuff_.reset(primitiveMesh);
	}
	modelData->vertSize = vertices.size();
}

ModelMtl ModelManager::LoadMtlFile(const std::string &directoryPath,const std::string &filename,const std::string &materialName) {
	ModelMtl data {};

	bool isMatchingName = false;

	std::ifstream file(directoryPath + "/" + filename);

	assert(file.is_open());

	std::string line;
	while(std::getline(file,line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		if(identifier == "newmtl") {
			std::string mtlName;
			s >> mtlName;
			isMatchingName = mtlName == materialName ? true : false;
		} else if(identifier == "map_Kd") {
			if(!isMatchingName) {
				continue;
			}
			std::string textureFilename;
			s >> textureFilename;
			data.textureNumber = std::make_unique<int>(TextureManager::LoadTexture(directoryPath + "/" + textureFilename));
		}
	}
	return data;
}
#pragma endregion

#pragma region"Model"
Model *Model::Create(const std::string &directoryPath,const std::string &filename) {
	Model *model = new Model();
	manager_->Create(model,directoryPath,filename);
	return model;
}

void Model::Init() {
	manager_ = std::make_unique<ModelManager>();
	manager_->Init();

	fovMa_ = std::make_unique<Matrix4x4>();
	Matrix4x4 *maPtr = new Matrix4x4();
	*maPtr = MakeMatrix::PerspectiveFov(
		0.45f,
		static_cast<float>(System::getInstance()->getWinApp()->getWidth())
		/
		static_cast<float>(System::getInstance()->getWinApp()->getHeight()),
		0.1f,
		100.0f);
	fovMa_.reset(
		maPtr
	);
}

void Model::Finalize() {
	manager_->Finalize();
}

void Model::DrawThis(const WorldTransform &world,const ViewProjection &view) {
	DirectXCommon *dxCommon_ = System::getInstance()->getDxCommon();
	for(auto &model : data_) {
		dxCommon_->getCommandList()->SetGraphicsRootSignature(model->usePso_->rootSignature.Get());
		dxCommon_->getCommandList()->SetPipelineState(model->usePso_->pipelineState.Get());

		dxCommon_->getCommandList()->IASetVertexBuffers(0,1,&model->meshBuff_->vbView);

		world.SetForRootParameter(dxCommon_->getCommandList(),0);
		view.SetForRootParameter(dxCommon_->getCommandList(),1);

		System::getInstance()->SetStanderdForRootparameter(2,3);

		if(model->materialData.textureNumber != nullptr) {
			ID3D12DescriptorHeap *ppHeaps[] = {dxCommon_->getSrv()};
			dxCommon_->getCommandList()->SetDescriptorHeaps(1,ppHeaps);
			dxCommon_->getCommandList()->SetGraphicsRootDescriptorTable(
				4,
				TextureManager::getDescriptorGpuHandle(*model->materialData.textureNumber.get())
			);
		}

		// 描画!!!
		dxCommon_->getCommandList()->DrawInstanced((UINT)(model->vertSize),1,0,0);
	}
}

void Model::Draw(const WorldTransform &world,const ViewProjection &view) {
	drawFuncTable_[(size_t)currentState_](world,view);
}
#pragma endregion