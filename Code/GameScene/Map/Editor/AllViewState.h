#pragma once

#include "IMapEditState.h"

class AllViewState
	:public IMapEditState {
public:
	AllViewState(MapEditor *editor):IMapEditState(editor) {};

	void Update()override;
	void Draw(const ViewProjection &viewProj);
private:
	void EditCol();
	void EditRow();
private:
	std::pair<uint32_t,uint32_t> currentAddress_;
	std::pair<uint32_t,uint32_t> preMapSize_;
	std::pair<uint32_t,uint32_t> mapSize_;
};