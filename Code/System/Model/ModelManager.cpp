#include "ModelManager.h"

#include "Model.h"

#include "memory"

#include "CommonBuffer.h"
#include "System.h"
#include "TextureManager.h"

void ModelManager::Create(Model *model, const std::string &directoryPath, const std::string &filename) {
	DirectXCommon *dxCommon = System::getInstance()->getDxCommon();
	model->data_ = LoadObjFile(directoryPath, filename);

	model->data_.vertSize = model->data_.vertices.size();

	model->buffer_ = std::make_unique<CommonBuffer>();
	if(model->data_.materialData.textureNumber != nullptr) {
		model->isTexture_ = true;
		model->usePso_ = System::getInstance()->getTexturePso();

		model->data_.dataSize = sizeof(TextureVertexData) * model->data_.vertSize;
		dxCommon->CreateBufferResource(model->buffer_->vertBuff, model->data_.dataSize);

		model->buffer_->vbView.BufferLocation = model->buffer_->vertBuff->GetGPUVirtualAddress();
		model->buffer_->vbView.SizeInBytes = UINT(model->data_.dataSize);
		model->buffer_->vbView.StrideInBytes = sizeof(TextureVertexData);

		model->buffer_->vertBuff->Map(
			0,
			nullptr,
			reinterpret_cast<void **>(&model->buffer_->textureVertData)
		);

		memcpy(
			model->buffer_->textureVertData,
			model->data_.vertices.data(),
			model->data_.dataSize
		);
	} else {
		model->isTexture_ = false;
		std::vector<PrimitiveVertexData> primVert;
		for(auto &texVert : model->data_.vertices) {
			primVert.push_back(texVert);
		}

		model->usePso_ = System::getInstance()->getPrimitivePso();

		model->data_.dataSize = sizeof(PrimitiveVertexData) * model->data_.vertSize;
		dxCommon->CreateBufferResource(model->buffer_->vertBuff, model->data_.dataSize);

		model->buffer_->vbView.BufferLocation = model->buffer_->vertBuff->GetGPUVirtualAddress();
		model->buffer_->vbView.SizeInBytes = UINT(model->data_.dataSize);
		model->buffer_->vbView.StrideInBytes = sizeof(PrimitiveVertexData);

		model->buffer_->vertBuff->Map(
			0,
			nullptr,
			reinterpret_cast<void **>(&model->buffer_->primitiveVertData)
		);

		memcpy(
			model->buffer_->primitiveVertData,
			primVert.data(),
			model->data_.dataSize
		);
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

MaterialData ModelManager::LoadMtlFile(const std::string &directoryPath, const std::string &filename) {
	MaterialData data {};

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
