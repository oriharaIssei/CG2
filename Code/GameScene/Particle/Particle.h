#pragma once

#include <d3d12.h>
#include "PipelineStateObj.h"

#include <memory>
#include <wrl.h>

#include "DXCommand.h"
#include "DXSrvArray.h"
#include "Object3dMesh.h"

#include "Material.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

#include "Matrix4x4.h"
#include "Vector3.h"
#include <stdint.h>

#include <vector>

class Particle{
public:
	void Init(uint32_t instanceValue);
	void Draw(const ViewProjection &viewProjection,const Material *material);
private:
	void CreatePso();
private:
	static std::unique_ptr<PipelineStateObj> pso_;

	std::vector<Matrix4x4 *> transforms_;

	Matrix4x4 *mappingData_;

	uint32_t srvIndex_;
	uint32_t particleSize_;

	std::shared_ptr<DXSrvArray> dxSrvArray_;
	std::unique_ptr<TextureObject3dMesh> meshBuff_;
	std::unique_ptr<DXCommand> dxCommand_;
public:
	void setTransform(uint32_t instanceValue,const Vector3 &scale,const Vector3 &rotate,const Vector3 &translate);
	void setTransform(uint32_t instanceValue,const Matrix4x4 &transformMat);
};