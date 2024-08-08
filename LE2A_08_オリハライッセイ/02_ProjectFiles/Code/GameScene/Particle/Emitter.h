#pragma once

#include "Particle.h"

#include <random>

#include <memory>

#include <vector>

#include "PipelineStateObj.h"
#include "DXSrvArray.h"
#include "DXCommand.h"

#include "Object3dMesh.h"
#include "Material.h"
#include "ViewProjection.h"

#include "Vector3.h"
#include <stdint.h>

class Emitter{
public:
	~Emitter(){ Finalize(); }
	void Init(uint32_t instanceValue,MaterialManager *materialManager);
	void Update();
	void Draw(const ViewProjection &viewProjection);
	void Finalize();
private:
	void CreatePso();
private:
	static std::unique_ptr<PipelineStateObj> pso_;

	Vector3 originPos_;

	std::mt19937 randomEngine_;

	std::vector<std::unique_ptr<Particle>> particles_;

	ParticleStructuredBuffer *mappingData_;

	uint32_t srvIndex_;
	uint32_t particleSize_;

	Material *material_;

	std::shared_ptr<DXSrvArray> dxSrvArray_;
	std::unique_ptr<TextureObject3dMesh> meshBuff_;
	std::unique_ptr<DXCommand> dxCommand_;

};