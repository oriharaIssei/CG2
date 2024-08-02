#pragma once

#include <string>

#include <array>
#include <vector>

#include <memory>

#include "DXDevice.h"

#include "DXCommand.h"
#include "DXSwapChain.h"
#include "DXFence.h"

#include "DXDepthStencilView.h"
#include "DXRenterTargetView.h"
#include "DXSrvArray.h"

#include "Input.h"
#include "PipelineStateObj.h"
#include "ShaderCompiler.h"
#include "ShaderManager.h"
#include "WinApp.h"

#include <Model.h>

#include <LightBuffer.h>
#include <Material.h>

#include <Matrix4x4.h>
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>

class System{
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
	~System(){};
	System(const System &) = delete;
	const System &operator=(const System &) = delete;
	void CreateTexturePSO();
private:
	std::unique_ptr<WinApp> window_;
	Input *input_;

	std::unique_ptr<DXDevice> dxDevice_;

	std::unique_ptr<DXCommand> dxCommand_;
	std::unique_ptr<DXSwapChain> dxSwapChain_;
	std::unique_ptr<DXFence> dxFence_;

	std::unique_ptr<DXRenterTargetView> dxRenderTarget_;
	std::unique_ptr<DXDepthStencilView> dxDepthStencil_;


	std::array<PipelineStateObj *,kBlendNum> texturePso_;
	std::array<std::string,kBlendNum> texturePsoKeys_;

	std::unique_ptr<MaterialManager> materialManager_;
	std::unique_ptr<LightBuffer> standerdLight_;
public:
	const std::string textureVsBlobKey_ = "Tex_VS";
	const std::string texturePsBlobKey_ = "Tex_PS";

	WinApp *getWinApp(){ return window_.get(); }

	DXDevice *getDXDevice()const{ return dxDevice_.get(); }
	DXSwapChain *getDXSwapChain()const{ return dxSwapChain_.get(); }
	DXFence *getDXFence()const{ return dxFence_.get(); }

	DXDepthStencilView *getDXDepthStencil()const{ return dxDepthStencil_.get(); }

	MaterialManager *getMaterialManager()const{ return materialManager_.get(); }

	PipelineStateObj *getTexturePso(BlendMode blend)const{ return texturePso_[static_cast<size_t>(blend)]; }

	const std::array<std::string,kBlendNum> &getTexturePsoKeys()const{ return texturePsoKeys_; }

	const LightBuffer *getStanderdLight()const{ return standerdLight_.get(); }
};