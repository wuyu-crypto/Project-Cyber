//=============================================================================
//
// エフェクト処理 [effect.h]
// Author : GP11B132 33 呉優
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define		EFFECT_MAX				(30)			// 同時最大エフェクト数

#define		EFFECT_LOOP_FOREVER		-1				// 持続アニメフラグ

// エフェクトタイプ番号
enum {
	EFFECT_TYPE_PLAYER_SHOT,

	EFFECT_TYPE_PLAYER_HIT,
	EFFECT_TYPE_ENEMY_HIT,

	EFFECT_TYPE_ENEMY_EXPLOSION,

	EFFECT_TYPE_PLAYER_BACKFIRE,
	EFFECT_TYPE_PLAYER_BACKGLOW,
	EFFECT_TYPE_PLAYER_BACKPARTICLES,

	EFFECT_TYPE_PLAYER_FLASH,

	EFFECT_TYPE_HEAL,
	EFFECT_TYPE_POWERUP,

	EFFECT_TYPE_PLAYER_POPPED00,
	EFFECT_TYPE_PLAYER_POPPED01,

	EFFECT_TYPE_BOSS_EXPLOSION,

	EFFECT_TYPE_MANUAL00,
	EFFECT_TYPE_MANUAL01,
	EFFECT_TYPE_MANUAL02,
	EFFECT_TYPE_MANUAL03,

	EFFECT_TYPE_MAX,
};

// エフェクト描画レイヤー
enum {

	EFFECT_LAYER_BEHIND_PLAYER,			// プレイヤーより後ろに描画
	EFFECT_LAYER_FRONT_OF_PLAYER,		// プレイヤーより前に描画

	EFFECT_LAYER_MAX,
};

//*****************************************************************************
// 構造体宣言
//*****************************************************************************

// エフェクト構造体
struct EFFECT {

	BOOL			isActive;
	int				w, h;					// キャラサイズ
	XMFLOAT3		pos;
	int				type;
	int				roop;					// ループ数(デフォが1, ANIM_ROOP_ALWAYSが永久)

	int				animFrameCnt;			// アニメーションカウンタ
	int				animPattern;			// 表示中パターン
	int				animTotalFrame;			// 全ループの合計フレーム数

	int				trackTarget;			// 追従対象(あれば)
};

// エフェクトデータ構造体
struct EFFECT_DATA {

	char*			textureName;			// テクスチャ名

	XMINT2			size;					// デフォルトキャラサイズ
	int				mode;					// 描画モード
	float			alpha;					// α

	int				trackLabel;				// 追従ラベル
	XMFLOAT3		diff;					// 追従対象からのズレ

	BOOL			isOnly;					// 排他フラグ(既存の同一エフェクトをすべて中止)

	XMINT2			patternDivide;			// アニメパターンのテクスチャ内分割数
	int				patternNum;				// 1ループパターン数
	int				wait;					// 1パターンが維持するフレーム数

	int				layer;					// 描画レイヤー
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEffect(void);
void UninitEffect(void);
void UpdateEffect(void);
void DrawEffect(int layer);

EFFECT* GetEffect(void);
EFFECT_DATA* GetEffectData(void);

void SetEffect(int type, int target, int loop);
void SetEffect(int type, XMFLOAT3 pos, int loop);

void StopEffect(int type);