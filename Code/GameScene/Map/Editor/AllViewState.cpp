#include "AllViewState.h"
#include "ChipEditState.h"

#include "MapEditor.h"

#include "imgui.h"

void AllViewState::Update() {
	ImGui::DragFloat2("ChipSize",&MapEditor::EditChip::size.x);

	preMapSize_ = {host_->getChipList().size() - 1,host_->getChipList()[0].size() - 1};
	mapSize_ = preMapSize_;

	ImGui::InputInt("Row",(int *)&mapSize_.first,1,0);

	ImGui::InputInt("Col",(int *)&mapSize_.second,1,0);

	if(mapSize_.first != preMapSize_.first) {
		EditRow();
	}
	if(mapSize_.second != preMapSize_.second) {
		EditCol();
	}

	ImGui::InputInt("Focus Row :",(int *)&currentAddress_.first);
	ImGui::InputInt("Focus Col :",(int *)&currentAddress_.second);
	currentAddress_ = {
		(std::clamp<uint32_t>)(currentAddress_.first,0,host_->getChipList().size() - 1),
		(std::clamp<uint32_t>)(currentAddress_.second,0,host_->getChipList()[0].size() - 1)
	};

	if(ImGui::Button("Focus currentAddress")) {
		host_->TransitionState(new ChipEditState(host_,currentAddress_));
	}
}

void AllViewState::Draw(const ViewProjection &viewProj) {
	for(auto &editChipCol : host_->getChipList()) {
		for(auto &editChip : editChipCol) {
			editChip->Draw(viewProj);
		}
	}
}

void AllViewState::EditRow() {
	if(mapSize_.first > preMapSize_.first) {
		for(uint32_t newRow = preMapSize_.first + 1; newRow <= mapSize_.first; ++newRow) {
			host_->PushBackChipList(std::vector<std::unique_ptr<MapEditor::EditChip>>());
			for(uint32_t col = 0; col <= mapSize_.second; ++col) {
				host_->PushBackChip(newRow,std::make_unique<MapEditor::EditChip>());
				host_->getChipList()[newRow].back()->Init(host_,{newRow,col});
			}
		}
		// 行を減らす場合
	} else if(mapSize_.first < preMapSize_.first) {
		for(uint32_t row = preMapSize_.first; row > mapSize_.first; --row) {
			if(row < host_->getChipList().size()) {
				host_->PopBackChipList();
			}
		}
	}
}

void AllViewState::EditCol() {
	// 列を増やす場合
	if(mapSize_.second > preMapSize_.second) {
		for(uint32_t row = 0; row < host_->getChipList().size(); ++row) {
			for(uint32_t col = preMapSize_.second + 1; col <= mapSize_.second; ++col) {
				host_->PushBackChip(row,std::make_unique<MapEditor::EditChip>());
				host_->getChipList()[row].back()->Init(host_,{row,col});
			}
		}
	// 列を減らす場合
	} else if(mapSize_.second < preMapSize_.second) {
		for(uint32_t row = 0; row < host_->getChipList().size(); ++row) {
			for(uint32_t col = preMapSize_.second; col > mapSize_.second; --col) {
				if(col < host_->getChipList()[row].size()) {
					host_->PopBackChip(row);
				}
			}
		}
	}
}