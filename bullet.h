//=============================================================================
//
// バレット処理 [bullet.h]
// Author : GP11B132 33 呉優
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

#include "enemy.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define BULLET_MAX		(200)		// 同時最大バレット数

// バレットタイプ
enum {

	BULLET_TYPE_PLAYER_NORMAL,
	BULLET_TYPE_ENEMY_NORMAL,
	BULLET_TYPE_ENEMY_SLOW,

#ifdef BEZIER
	BULLET_TYPE_ENEMY_BEZIER,
#endif

	BULLET_TYPE_MAX,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

// バレット構造体
struct BULLET
{
	BOOL			isActive;			// true:使っている  false:未使用
	float			w, h;				// 幅と高さ

	XMFLOAT3		pos;				// バレットの座標
	XMFLOAT3		rot;				// バレットの回転量

	int				type;				// バレットタイプ
	XMFLOAT3		speed;				// スピード

	int				animFrameCnt;		// アニメーションカウンタ
	int				animPattern;		// 表示中パターン

	int				colliderW, colliderH;		// 当たり判定サイズ

	BOOL			isDelayed;			// 描画遅延？(生成はされど描画はされない)
	int				delayCnt;			// 遅延カウンタ

	XMFLOAT3		start;				// 起点（ベジェ用）
	XMFLOAT3		end;				// 終点（ベジェ用）
	float			bezierT;			// 飛行時間（ベジェ用）
	float			bezierCnt;			// 移動カウンタ（ベジェ用）
};

// バレットデータ構造体
struct BULLET_DATA {
	
	char*			textureName;		// テクスチャ名

	XMINT2			size;				// デフォルトキャラサイズ
	int				mode;				// 描画モード
	float			alpha;				// α

	XMFLOAT3		speed;				// デフォルトスピード

	XMINT2			patternDivide;		// アニメパターンのテクスチャ内分割数
	int				patternNum;			// 1ループパターン数
	int				wait;				// 1パターンが維持するフレーム数

	int				colliderW, colliderH;		// 当たり判定サイズ
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET* GetBullet(void);
BULLET_DATA* GetBulletData(void);

void SetBullet(int type, XMFLOAT3 pos);							// デフォルト弾
void SetBullet(int type, XMFLOAT3 pos, XMFLOAT3 speed);			// 方向指定弾

void SetBarrage(int type, XMFLOAT3 pos, float degree, float startDegree);					// 円形弾幕
void SetBarrage(int type, XMFLOAT3 pos, float degree, float startDegree, float endDegree);	// 扇形弾幕


/*
* @brief 追尾ベジェ弾道
* @param [in] type 弾のタイプ
* @param [in] start 発射元の座標
* @param [in] end 目標座標
* @param [in] t 飛行時間
*/
void SetBezierBullet(int type, XMFLOAT3 start, XMFLOAT3 end, float t);