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

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>

class DirectXCommon;
class TextureManager {
public:
    static void Init(DirectXCommon *dxCommon);
    static void Finalize();

    static uint32_t LoadTexture(const std::string &filePath);
    static void UnloadTexture(uint32_t id);

    static void LoadLoop();

private:
    struct Texture {
        void Init(const std::string &filePath,int textureIndex);
        void Finalize();

        std::string path_;
        Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;
        D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;
    private:
        DirectX::ScratchImage Load(const std::string &filePath);
        void CreateTextureResource(DirectXCommon *dxCommon,const DirectX::TexMetadata &metadata);
        void UploadTextureData(DirectX::ScratchImage &mipImg);
    };

private:
    static const uint32_t maxTextureSize_ = 512;

    static uint64_t cpuDescriptorHandleStart_;
    static uint64_t gpuDescriptorHandleStart_;
    static uint32_t handleIncrementSize_;

    static DirectXCommon *dxCommon_;
    static std::array<std::unique_ptr<Texture>,maxTextureSize_> textures_;

    static std::thread loadingThread_;
    static std::queue<std::pair<std::string,uint32_t>> loadingQueue_;
    static std::mutex queueMutex_;
    static std::condition_variable queueCondition_;
    static bool stopLoadingThread_;

public:
    static const D3D12_GPU_DESCRIPTOR_HANDLE &getDescriptorGpuHandle(uint32_t handleId) {
        return textures_[handleId]->srvHandleGPU_;
    }
};