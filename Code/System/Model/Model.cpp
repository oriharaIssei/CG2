#include "Model.h"

#include <cassert>

#include "TextureManager.h"
#include <System.h>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <stdint.h>

std::unique_ptr<Matrix4x4> Model::fovMa_ = nullptr;

#pragma region"ModelManager"
class ModelManager {
public:
	void Create(Model *model, const std::string &directoryPath, const std::string &filename);
private:
	ModelData LoadObjFile(const std::string &directoryPath, const std::string &filename);
	ModelMtl LoadMtlFile(const std::string &directoryPath, const std::string &filename);
};

void ModelManager::Create(Model *model, const std::string &directoryPath, const std::string &filename) {
	model->data_ = LoadObjFile(directoryPath, filename);

	model->data_.vertSize = model->data_.vertices.size();

	if(model->data_.materialData.textureNumber != nullptr) {
		TextureObject3dMesh *textureMesh = new TextureObject3dMesh();
		model->isTexture_ = true;
		model->usePso_ = System::getInstance()->getTexturePso();

		textureMesh->Create(model->data_.vertSize);
		memcpy(
			textureMesh->vertData,
			model->data_.vertices.data(),
			model->data_.vertSize * sizeof(TextureVertexData)
		);
		model->meshBuff_.reset(textureMesh);
	} else {
		PrimitiveObject3dMesh *primitiveMesh = new PrimitiveObject3dMesh();

		model->isTexture_ = false;
		model->usePso_ = System::getInstance()->getPrimitivePso();

		std::vector<PrimitiveVertexData> primVert;
		for(auto &texVert : model->data_.vertices) {
			primVert.push_back(texVert);
		}

		model->data_.dataSize = sizeof(PrimitiveVertexData) * primVert.size();

		primitiveMesh->Create(primVert.size());
		memcpy(
			primitiveMesh->vertData,
			primVert.data(),
			model->data_.dataSize
		);
		model->meshBuff_.reset(primitiveMesh);
	}
}

ModelData ModelManager::LoadObjFile(const std::string &directoryPath, const std::string &filename) {
	//==========================================
	// 1. 中で必要となる変数の宣言
	/*     位置     */ std::vector<Vector4> poss;
	/*     法線     */ std::vector<Vector3> normals;
	/* テクスチャ座標 */ std::vector<Vector2> texCoords;
	/* 1行を保存する */ std::string line;
	ModelData data;
	//==========================================
	// 2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	//==========================================
	// 3. Model 読み込み
	while(std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		/* 先頭の識別子を読む */
		s >> identifier;

		if(identifier == "v") {
			/* 頂点 */
			Vector4 pos;
			s >> pos.x >> pos.y >> pos.z;
			pos.x *= -1.0f;
			pos.w = 1.0f;
			poss.push_back(pos);
		} else if(identifier == "vt") {
			/* テクスチャ座標 */
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texCoords.push_back(texcoord);
		} else if(identifier == "vn") {
			/* 法線 */
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if(identifier == "f") {
			/* 面 (三角形限定) */
			TextureVertexData triangle[3];
			for(int32_t faceVert = 0; faceVert < 3; ++faceVert) {
				std::string vertDefinition;
				s >> vertDefinition;
				/* 頂点の要素への Indexは
					位置,
					UV,
					法線,
					で格納されているので分解してIndexを取得する
				*/
				std::istringstream v(vertDefinition);
				uint32_t elementIndices[3];

				for(int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');/* '/' ごとに区切る */
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
					texCoord = { 0.0f,0.0f };
				} else {
					texCoord = texCoords[elementIndices[1] - 1];
				}

				triangle[faceVert] = { position,texCoord,normal };
			}
			data.vertices.push_back(triangle[2]);
			data.vertices.push_back(triangle[1]);
			data.vertices.push_back(triangle[0]);
		} else if(identifier == "mtllib") {
			std::string materialFileName;
			s >> materialFileName;
			data.materialData = LoadMtlFile(directoryPath, materialFileName);
		}
	}
	return data;
}

ModelMtl ModelManager::LoadMtlFile(const std::string &directoryPath, const std::string &filename) {
	ModelMtl data {};

	std::ifstream file(directoryPath + "/" + filename);

	assert(file.is_open());

	std::string line;
	while(std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if(identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			data.textureNumber = std::make_unique<int>(TextureManager::LoadTexture(directoryPath + "/" + textureFilename));
		}
	}
	return data;
}
#pragma endregion


#pragma region"Model"
Model *Model::Create(const std::string &directoryPath, const std::string &filename) {
	static ModelManager modelManager;
	Model *model = new Model();
	modelManager.Create(model, directoryPath, filename);
	return model;
}

void Model::Init() {
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

void Model::Draw(const WorldTransform &world, const ViewProjection &view) {
	DirectXCommon *dxCommon_ = System::getInstance()->getDxCommon();
	dxCommon_->getCommandList()->SetGraphicsRootSignature(usePso_->rootSignature.Get());
	dxCommon_->getCommandList()->SetPipelineState(usePso_->pipelineState.Get());

	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &meshBuff_->vbView);

	world.SetForRootParameter(dxCommon_->getCommandList(), 0);
	view.SetForRootParameter(dxCommon_->getCommandList(), 1);

	System::getInstance()->SetStanderdForRootparameter(2, 3);

	if(isTexture_ == true) {
		ID3D12DescriptorHeap *ppHeaps[] = { dxCommon_->getSrv() };
		dxCommon_->getCommandList()->SetDescriptorHeaps(1, ppHeaps);
		dxCommon_->getCommandList()->SetGraphicsRootDescriptorTable(
			4,
			TextureManager::getGpuHandle(*data_.materialData.textureNumber.get())
		);
	}

	// 描画!!!
	dxCommon_->getCommandList()->DrawInstanced((UINT)(data_.vertSize), 1, 0, 0);
}

#pragma endregion