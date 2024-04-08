#include "Matrix3x3.h"
#include "assert.h"
#define _USE_MATH_DEFINES
#include <math.h>

Matrix3x3 Matrix3x3::operator*(const Matrix3x3& another) const {
	Matrix3x3 result{};
	for (int r = 0; r < 3; r++) {
		for (int c = 0; c < 3; c++) {
			result.m[r][c] = 0.0f;
			for (int i = 0; i < 3; i++) {
				result.m[r][c] += this->m[r][i] * another.m[i][c];
			}
		}
	}
	return result;
}
void Matrix3x3::operator*=(const Matrix3x3& another) {
	Matrix3x3 result{ 0 };
	for (int r = 0; r < 3; r++) {
		for (int c = 0; c < 3; c++) {
			for (int i = 0; i < 3; i++) {
				result.m[r][c] += this->m[r][i] * another.m[i][c];
			}
		}
	}

	//結果の値をthisにコピー
	*this = result;
}

void Matrix3x3::operator=(const Matrix3x3& another) {
	for (int r = 0; r < 3; r++) {
		for (int c = 0; c < 3; c++) {
			this->m[r][c] = another.m[r][c];
		}
	}
}

Matrix3x3 Matrix3x3::Inverse() {
	float bottom =
		(this->m[0][0] * this->m[1][1] * this->m[2][2])
		+ (this->m[0][1] * this->m[1][2] * this->m[2][0])
		+ (this->m[0][2] * this->m[1][0] * this->m[2][1])
		- (this->m[0][2] * this->m[1][1] * this->m[2][0])
		- (this->m[0][1] * this->m[1][0] * this->m[2][2])
		- (this->m[0][0] * this->m[1][2] * this->m[2][1]);

	assert(bottom != 0);

	Matrix3x3 inverse{};
	Matrix3x3 adjoint{
		(this->m[1][1] * this->m[2][2]) - (this->m[1][2] * this->m[2][1]),-1 * ((this->m[0][1] * this->m[2][2]) - (this->m[0][2])),(this->m[0][1] * this->m[1][2]) - (this->m[0][2] * this->m[1][1]),
		(-1 * ((this->m[1][0] * this->m[2][2])) - (this->m[1][2] * this->m[2][0])),(this->m[0][0] * this->m[2][2]) - (this->m[0][2] * this->m[2][1]),-1 * ((this->m[0][0] * this->m[1][2]) - (this->m[0][2] * this->m[1][0])),
		(this->m[1][0] * this->m[2][1]) - (this->m[1][1] * this->m[2][0]),(-1 * ((this->m[0][0] * this->m[2][1]) - (this->m[0][1] * this->m[2][0]))),(this->m[0][0] * this->m[1][1]) - (this->m[0][1] * this->m[1][0])
	};
	for (int r = 0; r < 3; r++) {
		for (int c = 0; c < 3; c++) {
			inverse.m[r][c] = (1.0f / bottom) * adjoint.m[r][c];
		}
	}
	return inverse;
}

Matrix3x3 MakeMatrix::Scale(const Vector2& scale) {
	Matrix3x3 scaleMatrix{
		scale.x,0.0f,0.0f,
		0.0f,scale.y,0.0f,
		0.0f,0.0f,1.0f
	};
	return scaleMatrix;
}
Matrix3x3 MakeMatrix::Rotate(const float& theta) {
	Matrix3x3 rotateMatrix{
		cosf(theta),sinf(theta),0.0f,
		-sinf(theta),cosf(theta),0.0f,
		0.0f,0.0f,1.0f
	};
	return rotateMatrix;
}
Matrix3x3 MakeMatrix::Trancelate(const Vector2& t) {
	Matrix3x3 trancelateMatrix{
		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		t.x,t.y,1.0f
	};
	return trancelateMatrix;
}
Matrix3x3 MakeMatrix::WorldMatrix(const Vector2& scale, const float& theta, const Vector2& localOrigin) {
	Matrix3x3 worldMatrix{};
	worldMatrix = MakeMatrix::Scale(scale) * MakeMatrix::Rotate(theta);
	worldMatrix *= MakeMatrix::Trancelate(localOrigin); // localOrigin を考慮する
	return worldMatrix;
}

Matrix3x3 MakeMatrix::OrthoProjectionMatrix(const float& left, const float& top, const float& right, const float& bottom) {
	Matrix3x3 orthographic{
	   2.0f / (right - left), 0, 0,
	   0, 2.0f / (top - bottom), 0,
	   (left + right) / (left - right), (top + bottom) / (bottom - top), 1
	};
	return orthographic;
}
Matrix3x3 MakeMatrix::ViewPortMatrix(float left, float top, float width, float height) {
	Matrix3x3 viewPort{
		width / 2.0f,0,0,
		0,-height / 2.0f,0,
		left + width / 2.0f,top + height / 2.0f,1.0f
	};
	return viewPort;
}

Matrix3x3 MakeMatrix::WvpVpMatrix(const Vector2& scale, const float& theta, const Vector2& localOrigin, const Vector2& worldCameraPos, const Vector2& windowLtPos, const Vector2& windowRbPos, const Vector2& viewPortOrigin, const Vector2& windowSize) {
	Matrix3x3 wvpVpMatrix{};
	Matrix3x3 camera = Trancelate(worldCameraPos);
	Matrix3x3 worldMatrix = MakeMatrix::WorldMatrix(scale, theta, localOrigin);
	Matrix3x3 orthoMatrix = MakeMatrix::OrthoProjectionMatrix(windowLtPos.x, windowLtPos.y, windowRbPos.x, windowRbPos.y);
	Matrix3x3 viewPortMatrix = MakeMatrix::ViewPortMatrix(viewPortOrigin.x, viewPortOrigin.y, windowSize.x, windowSize.y);
	wvpVpMatrix = worldMatrix * camera.Inverse();
	wvpVpMatrix *= orthoMatrix;
	wvpVpMatrix *= viewPortMatrix;
	return wvpVpMatrix;
}

Matrix3x3 MakeMatrix::WvpVpMatrix(
	const Matrix3x3& worldMatrix,
	const Matrix3x3& cameraMatrix,
	const Matrix3x3& orthoMatrix,
	const Matrix3x3& viewPort) {
	Matrix3x3 wvpVp{};
	Matrix3x3 viewMatrix = cameraMatrix;
	viewMatrix = viewMatrix.Inverse();

	wvpVp = worldMatrix * viewMatrix;
	wvpVp *= orthoMatrix;
	wvpVp *= viewPort;

	return wvpVp;
}

Vector2 Transform(Vector2 target, Matrix3x3 translateMatrix) {

	double result[3]{ 0.0f };
	//0=x,1=y,2=w
	double tmp[3]{ target.x,target.y,1.0f };

	//============
	//移動計算
	//============
	for (int r = 0; r < 3; r++) {
		for (int c = 0; c < 3; c++) {
			result[r] += static_cast<double>(tmp[c] * translateMatrix.m[c][r]);
		}
	}

	assert(result[2] != 0.0f);
	return Vector2(static_cast<float>(result[0] / result[2]), static_cast<float>(result[1] / result[2]));
}