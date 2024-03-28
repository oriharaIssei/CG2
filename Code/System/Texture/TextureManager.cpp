#include "TextureManager.h"

#include <combaseapi.h>
#include <cassert>

#include <Logger.h>
#include "DirectXCommon.h"

DirectXCommon* TextureManager::dxCommon_ = nullptr;
std::vector<std::unique_ptr<TextureManager::Texture>> TextureManager::textures_;

void TextureManager::Texture::Init(const std::string& filePath) {
	//==================================================
	// Textureを読み込んで転送する
	//==================================================
	DirectX::ScratchImage mipImages = Load(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	CreateTextureResource(dxCommon_, metadata);
	UploadTextureData(mipImages);

	//==================================================
	// ShaderResourceView を作成
	//==================================================
	/// metadataを基に SRV の設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	/// SRV を作成する DescriptorHeap の場所を決める
	srvHandleCPU_ = dxCommon_->getSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	srvHandleGPU_ = dxCommon_->getSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	/// 先頭は ImGui が使用しているので その次を使う
	srvHandleCPU_.ptr += dxCommon_->getDevice()->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
	);
	srvHandleGPU_.ptr += dxCommon_->getDevice()->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
	);

	/// SRV の作成
	dxCommon_->getDevice()->CreateShaderResourceView(
		resource_.Get(),
		&srvDesc,
		srvHandleCPU_
	);
}

void TextureManager::Texture::Finalize() {
	resource_.Reset();
}

DirectX::ScratchImage TextureManager::Texture::Load(const std::string& filePath) {
	DirectX::ScratchImage image{};

	// テクスチャファイルを読み込んでプログラムで使えるようにする
	std::wstring filePathW = Logger::ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(
		filePathW.c_str(),
		DirectX::WIC_FLAGS_FORCE_SRGB,
		nullptr,
		image
	);
	assert(SUCCEEDED(hr));

	// ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		DirectX::TEX_FILTER_SRGB,
		0,
		mipImages
	);
	return mipImages;
}

void TextureManager::Texture::CreateTextureResource(DirectXCommon* dxCommon, const DirectX::TexMetadata& metadata) {
	//================================================
	// 1. metadata を基に Resource を設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);// mipMap の数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行 or Texture[]の配列数
	resourceDesc.Format = metadata.format; //texture の Format
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント 1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	//================================================
	// 2. 利用する Heap の設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	
	//================================================
	// 3. Resource の作成
	HRESULT hr;
	hr = dxCommon->getDevice()->CreateCommittedResource(
		&heapProperties,// heap の設定
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,// Clear最適値
		IID_PPV_ARGS(&resource_)
	);
	assert(SUCCEEDED(hr));
}

void TextureManager::Texture::UploadTextureData(DirectX::ScratchImage& mipImg) {
	std::vector<D3D12_SUBRESOURCE_DATA> subResources;
	DirectX::PrepareUpload(
		dxCommon_->getDevice(),
		mipImg.GetImages(),
		mipImg.GetImageCount(),
		mipImg.GetMetadata(),
		subResources
	);

	uint64_t intermediateSize = GetRequiredIntermediateSize(
		resource_.Get(),
		0,
		UINT(subResources.size())
	);

	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = nullptr;
	dxCommon_->CreateBufferResource(
		intermediateResource,
		intermediateSize
	);

	UpdateSubresources(
		dxCommon_->getCommandList(),
		resource_.Get(),
		intermediateResource.Get(),
		0,
		0,
		UINT(subResources.size()),
		subResources.data()
	);

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource_.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	dxCommon_->getCommandList()->ResourceBarrier(1, &barrier);

	ID3D12GraphicsCommandList* commandList = dxCommon_->getCommandList();

	commandList->Close();
	dxCommon_->ExecuteCommandList();
	dxCommon_->Wait4ExecuteCommand();
	dxCommon_->ResetCommand();

}

void TextureManager::Init(DirectXCommon* dxCommon) {
	CoInitializeEx(0, COINIT_MULTITHREADED);
	dxCommon_ = dxCommon;
}

void TextureManager::Finalize() {
	CoUninitialize();
	for (auto& texture : textures_) {
		texture->Finalize();
	}
}

int TextureManager::LoadTexture(const std::string& filePath) {
	textures_.push_back(std::make_unique<Texture>());
	textures_.back()->Init(filePath);
	return static_cast<int>(textures_.size() - 1);
}