//=============================================================================
//
// ビル処理 [building.h]
// Author : GP11B132 33 呉優
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define		BUILDING_MAX			5			// ビルのレイヤー数

enum {
	BUILDING_LABEL_00,
	BUILDING_LABEL_01,

	BUILDING_LABEL_MAX,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct BUILDING
{
	XMFLOAT3	pos;		// ポリゴンの座標
	BOOL		isActive;	// 有効化フラグ
	float		w, h;		// 幅と高さ
	int			texNo;		// 使用しているテクスチャ番号

	float		scrlSpeed;	// スクロールスピード

	float		wait;		// 出現間隔
	float		waitCnt;	// 出現間隔カウンタ
};

struct BUILDING_DATA {

	char*		textureName;

	float		w, h;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBuilding(void);
void UninitBuilding(void);
void UpdateBuilding(void);
void DrawBuilding(void);