#pragma once

#include <wrl.h>

#include "d3d12.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"

#include "stdint.h"

#include <memory>
#include <string>
#include <vector>

#include "PipelineStateObj.h"

class DirectXCommon;
class TextureManager {
public:
	static void Init(DirectXCommon *dxCommon);
	static void Finalize();

	static int LoadTexture(const std::string &filePath);

	class Texture {
	public:
		Texture() = default;
		~Texture() = default;

		void Init(const std::string &filePath, int textureIndex);
		void Finalize();
	private:
		DirectX::ScratchImage Load(const std::string &filePath);
		void CreateTextureResource(DirectXCommon *dxCommon, const DirectX::TexMetadata &metadata);
		void UploadTextureData(DirectX::ScratchImage &mipImg);
	private:
		std::string path_;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;
	public:
		const D3D12_GPU_DESCRIPTOR_HANDLE &getGpuHandle() const { return srvHandleGPU_; }
		const std::string &getPath()const { return path_; }
	};

private:
	static DirectXCommon *dxCommon_;
	static std::vector<std::unique_ptr<Texture>> textures_;
public:
	static const D3D12_GPU_DESCRIPTOR_HANDLE &getGpuHandle(uint32_t textureNum) { return textures_[textureNum]->getGpuHandle(); }

};