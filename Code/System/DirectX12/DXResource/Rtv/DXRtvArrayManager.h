#pragma once

#include <memory>
#include <vector>

#include <stdint.h>

class DXRtvArray;
class DXRtvArrayManager{
public:
	static DXRtvArrayManager *getInstance();
public:
	void Init();
	void Finalize();
	/// @brief 生成
	/// @param size Srv 格納可能数
	/// @return 
	std::shared_ptr<DXRtvArray> Create(uint32_t size);
	/// @brief size分 空いているメモリを検索
	/// @param size 格納したい view 数
	/// @param dxSrvArray 
	/// @return 
	uint32_t SearchEmptyLocation(uint32_t size,std::shared_ptr<DXRtvArray>dxRtvArray);
private:
	DXRtvArrayManager() = default;
	DXRtvArrayManager(const DXRtvArrayManager &) = delete;
	DXRtvArrayManager *operator=(const DXRtvArrayManager &) = delete;
private:
	struct ArrayCondition{
		std::shared_ptr<DXRtvArray> dxRtvArray_;
		// nullptr用
		uint32_t arraySize;
		uint32_t arrayLocation;
	};
	std::vector<ArrayCondition> heapCondition_;
};