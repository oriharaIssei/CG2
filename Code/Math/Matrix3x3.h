﻿#pragma once
#include "Vector2.h"

struct Matrix3x3 final{
	float m[3][3];

	Matrix3x3 operator*(const Matrix3x3& another)const;
	void operator*=(const Matrix3x3& another);
	void operator=(const Matrix3x3& another);

	bool operator==(const Matrix3x3& other) const = default;

	Matrix3x3  Inverse();
};

Vector2 Transform(Vector2 target, Matrix3x3 translateMatrix);

namespace MakeMatrix {
	Matrix3x3 Scale(const Vector2& scale);
	Matrix3x3 Rotate(const float& theta);
	Matrix3x3 Trancelate(const Vector2& t);
	Matrix3x3 WorldMatrix(const Vector2& scale, const float& theta, const Vector2& localOrigin);

	Matrix3x3 OrthoProjectionMatrix(const float& left, const float& top, const float& right,  const float& bottom);
	Matrix3x3 ViewPortMatrix(float left, float top, float width, float height);

	Matrix3x3 WvpVpMatrix(const Vector2& scale,const float& theta, const Vector2& localOrigin,
		const Vector2& worldCameraPos, 
		const Vector2& windowLtPos,const Vector2& windowRbPos,
		const Vector2& viewPortOrigin,const Vector2& windowSize);

	Matrix3x3 WvpVpMatrix(
		const Matrix3x3& worldMatrix,
		const Matrix3x3& cameraMatrix,
		const Matrix3x3& orthoMatrix,
		const Matrix3x3& viewPortMatrix
	);
}