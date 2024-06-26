#pragma once
#include "IMapEditState.h"
#include "MapEditor.h"

#include "unordered_map"

#include "ImGuiFileSystem.h"

#include "stdint.h"

class ChipEditState :
	public IMapEditState {
public:
	ChipEditState(MapEditor *host,std::pair<uint32_t,uint32_t> address);

	void Update()override;
	void Draw(const ViewProjection &viewProj)override;
private:
	MapEditor::EditChip *chip;
	std::unordered_map<GameObject *,bool> isActiveEditWindow;

	std::unique_ptr<ImGuiFileSystem> fileSystem_;
};