#pragma once

#include <memory>
#include <vector>

#include <stdint.h>

class DXSrvArray;
class DXSrvArrayManager{
public:
	static DXSrvArrayManager *getInstance();
public:
	void Init();
	void Finalize();
	/// @brief 生成
	/// @param size Srv 格納可能数
	/// @return 
	std::shared_ptr<DXSrvArray> Create(uint32_t size);
	/// @brief size分 空いているメモリを検索
	/// @param size 格納したいsrv数
	/// @param dxSrvArray 
	/// @return 
	uint32_t SearchEmptyLocation(uint32_t size,std::shared_ptr<DXSrvArray> dxSrvArray);
private:
	DXSrvArrayManager() = default;
	DXSrvArrayManager(const DXSrvArrayManager &) = delete;
	DXSrvArrayManager *operator=(const DXSrvArrayManager &) = delete;
private:
	struct ArrayCondition{
		std::shared_ptr<DXSrvArray> dxSrvArray_;
		// nullptr用
		uint32_t arraySize;
		uint32_t arrayLocation;
	};
	std::vector<ArrayCondition> heapCondition_;
};