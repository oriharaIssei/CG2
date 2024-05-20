#pragma once

#include <memory>
#include <string>
#include <vector>

#include "DirectXCommon.h"
#include "PipelineStateObj.h"
#include "ShaderCompiler.h"
#include "WinApp.h"
#include <Input.h>

#include <Model.h>

#include <LightBuffer.h>
#include <Material.h>

#include <Matrix4x4.h>
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>

class System {
public:
	static System *getInstance();
public:
	void Init();
	void Finalize();
	bool ProcessMessage();
	void BeginFrame();
	void EndFrame();

	int LoadTexture(const std::string &filePath);
private:
	System() = default;
	~System() = default;
	System(const System &) = delete;
	const System &operator=(const System &) = delete;
	void CreatePrimitivePSO();
	void CreateTexturePSO();
private:
	std::unique_ptr<WinApp> window_;
	std::unique_ptr<Input> input_;
	std::unique_ptr<DirectXCommon> dxCommon_;
	std::unique_ptr<ShaderCompiler> shaderCompiler_;
	std::unique_ptr<PipelineStateObj> texturePso_;
	std::unique_ptr<PipelineStateObj> primitivePso_;

	std::unique_ptr<Material> standerdMaterial_;
	std::unique_ptr<LightBuffer> standerdLight_;
public:
	WinApp *getWinApp() { return window_.get(); }
	DirectXCommon *getDxCommon() { return dxCommon_.get(); }
	ShaderCompiler *getShaderCompiler() { return shaderCompiler_.get(); }
	PipelineStateObj *getPrimitivePso() { return primitivePso_.get(); }
	PipelineStateObj *getTexturePso() { return texturePso_.get(); }

	void SetStanderdForRootparameter(UINT materialRootparameter, UINT lightRootParameter);
};