//=============================================================================
//
// アイテム [item.h]
// Author : GP11B132 33 呉優
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"
#include "debugproc.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define			ITEM_MAX			10			// 同時最大アイテム数

// ドロップアイテムタイプ
enum {

	ITEM_TYPE_HEART,		// 回復
	ITEM_TYPE_POWER,		// パワーアップ

	ITEM_TYPE_MAX,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct ITEM
{
	BOOL			isActive;					// 有効化フラグ
	float			w, h;						// 幅と高さ

	XMFLOAT3		pos;						// 座標
	int				type;						// タイプ

	XMFLOAT3		speed;						// スピード

	int				animFrameCnt;				// アニメーションカウンタ
	int				animPattern;				// 表示中パターン

	int				colliderW, colliderH;		// 当たり判定サイズ
};


struct ITEM_DATA {

	char* textureName;		// テクスチャ名
	float			w, h;				// デフォルトキャラサイズ

	XMFLOAT3		speed;				// デフォルトスピード

	int				patternDivideX, patternDivideY;		// アニメパターンのテクスチャ内分割数
	int				patternNum;							// パターン数
	int				wait;								// ウェイト値

	int				colliderW, colliderH;				// 当たり判定サイズ
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitItem(void);
void UninitItem(void);
void UpdateItem(void);
void DrawItem(void);

void SetItem(int type, XMFLOAT3 pos);