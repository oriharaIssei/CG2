#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "Input.h"
#include "WinApp.h"

#include "DXCommand.h"
#include "DXDevice.h"
#include "DXDsv.h"
#include "DXFence.h"
#include "DXRtvArray.h"
#include "DXSrvArray.h"
#include "DXSwapChain.h"
#include "PipelineStateObj.h"
#include "ShaderCompiler.h"
#include "ShaderManager.h"

#include "DirectionalLight.h"
#include "Material.h"
#include "Model.h"
#include "PointLight.h"
#include "SpotLight.h"

#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

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

	std::unique_ptr<DXDsv> dxDsv_;

	std::array<PipelineStateObj *,kBlendNum> texturePso_;
	std::array<std::string,kBlendNum> texturePsoKeys_;

	std::unique_ptr<MaterialManager> materialManager_;

	std::unique_ptr<DirectionalLight> directionalLight_;
	std::unique_ptr<PointLight> pointLight_;
	std::unique_ptr<SpotLight> spotLight_;
public:
	WinApp *getWinApp(){ return window_.get(); }

	DXDevice *getDXDevice()const{ return dxDevice_.get(); }
	DXSwapChain *getDXSwapChain()const{ return dxSwapChain_.get(); }
	DXFence *getDXFence()const{ return dxFence_.get(); }

	DXDsv *getDsv()const{ return dxDsv_.get(); }

	MaterialManager *getMaterialManager()const{ return materialManager_.get(); }

	PipelineStateObj *getTexturePso(BlendMode blend)const{ return texturePso_[static_cast<size_t>(blend)]; }

	const std::array<std::string,kBlendNum> &getTexturePsoKeys()const{ return texturePsoKeys_; }

	DirectionalLight *getDirectionalLight()const{ return directionalLight_.get(); }
	PointLight *getPointLight()const{ return pointLight_.get(); }
	SpotLight *getSpotLight()const{ return spotLight_.get(); }
};

const std::string defaultReosurceFolder = "./resource";