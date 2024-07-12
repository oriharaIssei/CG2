#include "DXSrvArrayManager.h"

#include "System.h"

#include "DXHeap.h"
#include "DXSrvArray.h"

DXSrvArrayManager *DXSrvArrayManager::getInstance(){
	static DXSrvArrayManager isntance;
	return &isntance;
}

void DXSrvArrayManager::Init(){
	heapCondition_.push_back({nullptr,DXHeap::srvHeapSize,0});
}

void DXSrvArrayManager::Finalize(){
	for(size_t i = 0; i < heapCondition_.size(); i++){
		if(heapCondition_[i].dxSrvArray_ == nullptr){
			continue;
		}
		heapCondition_[i].dxSrvArray_.reset();
	}
}

std::shared_ptr<DXSrvArray> DXSrvArrayManager::Create(uint32_t size){
	std::shared_ptr<DXSrvArray> dxSrvArray = std::make_shared<DXSrvArray>();
	dxSrvArray->Init(size,SearchEmptyLocation(size,dxSrvArray));
	return dxSrvArray;
}

uint32_t DXSrvArrayManager::SearchEmptyLocation(uint32_t size,std::shared_ptr<DXSrvArray> dxSrvArray){
	DXHeap *dxHeap = DXHeap::getInstance();
	std::vector<std::pair<D3D12_CPU_DESCRIPTOR_HANDLE,uint32_t>> usedArrays_;
	uint32_t currentLocation = 0;

	for(uint32_t i = 0; i < heapCondition_.size(); i++){
		if(heapCondition_[i].dxSrvArray_ != nullptr){
			usedArrays_.push_back({dxHeap->getSrvCpuHandle(i),heapCondition_[i].arraySize});
			currentLocation += heapCondition_[i].arraySize;
			continue;
		}
		if(heapCondition_[i].arraySize < size){
			currentLocation += heapCondition_[i].arraySize;
			continue;
		}

		if(static_cast<int32_t>(heapCondition_[i].arraySize -= size) == 0){
			// sizeがぴったりなら そこを使う
			heapCondition_[i] = {dxSrvArray,size,i};
		} else{
			// size が違ったら 使う分だけ前詰めする
			std::vector<ArrayCondition>::iterator itr = heapCondition_.begin() + i;
			heapCondition_.insert(itr,{dxSrvArray,size,currentLocation});
			heapCondition_[i + 1].arrayLocation = currentLocation + size;
		}
		return heapCondition_[i].arrayLocation;
	}

	// 前詰め処理
	DXHeap::getInstance()->CompactSrvHeap(System::getInstance()->getDXDevice()->getDevice(),
										  usedArrays_);

	///使われていないものを一箇所にまとめる
	uint32_t endLocation = 0;
	uint32_t emptySize = 0;
	auto it = heapCondition_.begin();
	while(it != heapCondition_.end()){
		if(it->dxSrvArray_ != nullptr){
			// 使用されていない場合
			emptySize += it->arraySize;
			it = heapCondition_.erase(it);
		} else{
			it->dxSrvArray_->arrayStartLocation_ = endLocation;
			it->arrayLocation = endLocation;
			endLocation += it->arraySize;
			++it;
		}
	}

	if(endLocation + size >= DXHeap::srvHeapSize){
		assert(false);
		return 0;
	}

	heapCondition_.push_back({dxSrvArray,size,endLocation,});

	heapCondition_.push_back({nullptr,emptySize,endLocation + size});

	return heapCondition_[heapCondition_.size() - 2].arrayLocation;
}