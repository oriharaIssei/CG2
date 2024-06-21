#pragma once

#include "d3d12.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"

#include <thread>

#include <stdint.h>

#include <string>

#include <array>
#include <queue>

#include <memory>
#include <wrl.h>

#include "PipelineStateObj.h"
#include "DXCommand.h"

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>

class TextureManager {
public:
	static void Init();
	static void Finalize();

	static uint32_t LoadTexture(const std::string &filePath);
	static void UnloadTexture(uint32_t id);

	static void LoadLoop();

private:
	struct Texture {
		enum class LoadState {
			Loading,
			Loaded,
			Error
		};
		void Init(const std::string &filePath,int textureIndex);
		void Finalize();

		std::string path_;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;

		LoadState loadState;
	private:
		DirectX::ScratchImage Load(const std::string &filePath);
		void CreateTextureResource(const DirectX::TexMetadata &metadata);
		void UploadTextureData(DirectX::ScratchImage &mipImg);
		void ExecuteCommnad();
	};

private:
	static const uint32_t maxTextureSize_ = 512;

	static uint64_t cpuDescriptorHandleStart_;
	static uint64_t gpuDescriptorHandleStart_;
	static uint32_t handleIncrementSize_;

	static std::array<std::unique_ptr<Texture>,maxTextureSize_> textures_;

	static std::thread loadingThread_;
	static std::queue<std::pair<std::string,uint32_t>> loadingQueue_;
	static std::mutex queueMutex_;
	static std::condition_variable queueCondition_;
	static bool stopLoadingThread_;

	// バックグラウンドスレッド用
	static std::unique_ptr<DXCommand> dxCommand_;
	/*static Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	static Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;*/

public:
	static const D3D12_GPU_DESCRIPTOR_HANDLE &getDescriptorGpuHandle(uint32_t handleId) {
		if(textures_[handleId]->loadState == Texture::LoadState::Loaded) {
			return textures_[handleId]->srvHandleGPU;
		}
		return textures_[0]->srvHandleGPU;
	}
};