#pragma once

#include <wrl.h>

#include "d3d12.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

#include <string>
#include <memory>
#include <vector>

#include "PipelineStateObj.h"

class DirectXCommon;
class TextureManager {
public:
	static void Init(DirectXCommon* dxCommon);
	static void Finalize();

	static int LoadTexture(const std::string& filePath);

	static void SetPSO2CommandList(ID3D12GraphicsCommandList* cmdlist);

	class Texture {
	public:
		Texture() = default;
		~Texture() = default;

		void Init(const std::string& filePath, int textureIndex);
		void Finalize();
	private:
		DirectX::ScratchImage Load(const std::string& filePath);
		void CreateTextureResource(DirectXCommon* dxCommon, const DirectX::TexMetadata& metadata);
		void UploadTextureData(DirectX::ScratchImage& mipImg);
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;
	public:
		const D3D12_GPU_DESCRIPTOR_HANDLE& getGpuHandle() { return srvHandleGPU_; }
	};
private:
	static void InitPSO();
private:
	static DirectXCommon* dxCommon_;
	static std::vector<std::unique_ptr<Texture>> textures_;
	static std::unique_ptr<PipelineStateObj> pso_;
public:
	static const D3D12_GPU_DESCRIPTOR_HANDLE& getGpuHandle(int textureNum) { return textures_[textureNum]->getGpuHandle(); }
};