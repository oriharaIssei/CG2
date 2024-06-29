#pragma once

#include <memory>

#include "Input.h"

#include "WorldTransform.h"

#include "Vector3.h"

class Player {
public:
	void Init(const Vector3 &pos);
	void Update();
	/// <summary>
	/// fps 視点なので描画しない
	/// </summary>
	// void Draw
private:
	Input *input_;

	float speed_;
	WorldTransform transform_;
public:
	const WorldTransform &getTransform()const { return transform_; }
};