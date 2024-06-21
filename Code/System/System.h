#pragma once

#include <string>

#include <vector>

#include <memory>

#include "DXCommand.h"
#include "DXDepthStencil.h"
#include "DXDevice.h"
#include "DXFence.h"
#include "DXRenterTarget.h"
#include "DXSwapChain.h"

#include "Input.h"
#include "PipelineStateObj.h"
#include "ShaderCompiler.h"
#include "WinApp.h"

#include <Model.h>

#include <LightBuffer.h>
#include <Material.h>

#include <Matrix4x4.h>
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>

class System {
	friend class PrimitiveDrawer;
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
	~System() {};
	System(const System &) = delete;
	const System &operator=(const System &) = delete;
	void CreatePrimitivePSO(std::unique_ptr<PipelineStateObj> &pso,D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType);
	void CreateTexturePSO();
private:
	std::unique_ptr<WinApp> window_;
	Input *input_;

	std::unique_ptr<DXDevice> dxDevice_;

	std::unique_ptr<DXCommand> dxCommand_;
	std::unique_ptr<DXSwapChain> dxSwapChain_;
	std::unique_ptr<DXRenterTarget> dxRenderTarget_;
	std::unique_ptr<DXFence> dxFence_;

	std::unique_ptr<DXDepthStencil> dxDepthStencil_;

	std::unique_ptr<ShaderCompiler> shaderCompiler_;
	std::unique_ptr<PipelineStateObj> texturePso_;
	std::unique_ptr<PipelineStateObj> primitivePso_;

	std::unique_ptr<MaterialManager> materialManager_;
	std::unique_ptr<LightBuffer> standerdLight_;
public:
	WinApp *getWinApp() { return window_.get(); }

	DXDevice *getDXDevice()const { return dxDevice_.get(); }
	DXSwapChain *getDXSwapChain()const { return dxSwapChain_.get(); }
	DXFence *getDXFence()const { return dxFence_.get(); }
	DXDepthStencil *getDXDepthStencil()const { return dxDepthStencil_.get(); }

	MaterialManager *getMaterialManager()const { return materialManager_.get(); }
	
	PipelineStateObj *getPrimitivePso() { return primitivePso_.get(); }
	PipelineStateObj *getTexturePso() { return texturePso_.get(); }

	ShaderCompiler *getShaderCompiler() { return shaderCompiler_.get(); }

	const LightBuffer *getStanderdLight()const { return standerdLight_.get(); }

};