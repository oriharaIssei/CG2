#pragma once

#include "ViewProjection.h"

class MapEditor;
class IMapEditState {
public:
	IMapEditState(MapEditor *editor):host_(editor) {};
	virtual ~IMapEditState() {};

	virtual void Update() = 0;
	virtual void Draw(const ViewProjection &viewProj) = 0;

protected:
	MapEditor *host_;
};