//=============================================================================
//
// math [math.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "math.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define			VECTOR_DIMENSION		3

//*****************************************************************************
// グローバル変数
//*****************************************************************************


//=============================================================================
// 投影ベクトルを求める
//=============================================================================
XMVECTOR RotateVector(XMVECTOR xmvTarget, XMVECTOR xmvAngle) {

	float x = xmvTarget.m128_f32[0];
	float y = xmvTarget.m128_f32[1];

	x = x * cosf(-xmvAngle.m128_f32[0]) - y * sinf(-xmvAngle.m128_f32[0]);
	y = x * sinf(-xmvAngle.m128_f32[0]) + y * cosf(-xmvAngle.m128_f32[0]);

	XMVECTOR result = XMLoadFloat3(&XMFLOAT3(x, y, xmvTarget.m128_f32[2]));

	return result;
}



XMFLOAT3 Bezier(XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, float t) {

	XMVECTOR xmvp0 = XMLoadFloat3(&p0);
	XMVECTOR xmvp1 = XMLoadFloat3(&p1);
	XMVECTOR xmvp2 = XMLoadFloat3(&p2);

	XMVECTOR xmvp0p1, xmvp1p2;

	// p0とp1を補間
	xmvp0p1 = (1.0 - t) * xmvp0 + t * xmvp1;

	// p1とp2を補間
	xmvp1p2 = (1.0 - t) * xmvp1 + t * xmvp2;

	// p0-p1とp1-p2を補間
	XMVECTOR xmvRes = (1.0 - t) * xmvp0p1 + t * xmvp1p2;

	XMFLOAT3 res;
	XMStoreFloat3(&res, xmvRes);

	return res;
}