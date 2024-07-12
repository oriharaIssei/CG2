#pragma once

#include <d3d12.h>
#include <externals/DirectXTex/DirectXTex.h>

#include <wrl.h>

#include <vector>

#include <DXHeap.h>

class DXSrvArrayManager;
class DXSrvArray{
	friend class DXSrvArrayManager;
private:
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="textureSize">テクスチャーマネージャーで静的に確保されている数</param>
	void Finalize();

	uint32_t CreateView(ID3D12Device *device,
						D3D12_SHADER_RESOURCE_VIEW_DESC &viewDesc,
						Microsoft::WRL::ComPtr<ID3D12Resource> resource);
	void DestroyView(uint32_t srvIndex);
private:
	void Init(uint32_t size,uint32_t arrayLocation);
private:
	/// <summary>
	/// 変化することがあるため ポインター
	/// </summary>
	uint32_t arrayStartLocation_;
	uint32_t size_;
	/// <summary>
	/// コンパイル後にサイズを決めるためvectorにしているが，
	/// Init後サイズを変更してはいけない
	/// </summary>
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> resources_;
public:
	uint32_t getSize()const{ return size_; }

	ID3D12Resource *getSrv(uint32_t index)const{ return resources_[index].Get(); }
	Microsoft::WRL::ComPtr<ID3D12Resource> getSrvComPtr(uint32_t index)const{ return resources_[index]; }

	uint32_t getLocationOnHeap(uint32_t index)const{ return index + arrayStartLocation_; }
};