#pragma once

#include <Matrix4x4.h>
#include <Vector2.h>

#include "CommonBuffer.h"

#include "memory"
#include <string>

class Sprite {
public:
	static void Init();
	static Sprite *Create(const Vector2 &pos, const Vector2 &size, const std::string &textureFilePath);
	static void PreDraw();
public:
	static Matrix4x4 orthoMa_;
public:
	void Draw(const Matrix4x4 &world, const Matrix4x4 &view);
private:
	Vector2 pos_;
	Vector2 size_;

	std::unique_ptr<CommonBuffer> buffer_;

	uint32_t th_;
public:
	const Vector2 &getPos()const { return pos_; }
	void setPos(const Vector2 &pos) { pos_ = pos; }

	const Vector2 &getSize()const { return size_; }
	void setSize(const Vector2 &size) { size_ = size; }
};