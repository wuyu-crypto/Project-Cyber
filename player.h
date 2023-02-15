//=============================================================================
//
// プレイヤー処理 [player.h]
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
#define		PLAYER_MAX				1			// プレイヤーのMax人数

#define		PLAYER_HP_MAX			10			// HP上限

enum
{
	CHAR_DIR_FORWARD,
	CHAR_DIR_DOWNWARD,
	CHAR_DIR_UPWARD,
	CHAR_DIR_BACKWARD,

	CHAR_DIR_MAX
};

enum {

	PLAYER_LEVEL00,
	PLAYER_LEVEL01,
	PLAYER_LEVEL02,
	PLAYER_LEVEL03,
	PLAYER_LEVEL04,

	PLAYER_LEVEL_MAX,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct PLAYER {

	XMFLOAT3	pos;						// ポリゴンの座標
	XMFLOAT3	rot;						// ポリゴンの回転量
	BOOL		isActive;					// true:使っている  false:未使用
	float		w, h;						// 幅と高さ
	int			animFrameCnt;				// アニメーションカウント
	int			animPattern;				// アニメーションパターンナンバー
	int			texNo;						// テクスチャ番号。Updateで制御
	
	int			dirH;						// 水平方向の向き
	int			dirV;						// 垂直方向の向き

	XMFLOAT3	speedOrg;					// デフォルト移動速度
	XMFLOAT3	speed;						// 移動速度

	int			hp;
	float		colliderW, colliderH;		// 当たり判定サイズ
	BOOL		isDamaged;					// 被弾グラグ(無敵状態)
	BOOL		isAlphaChanged;		// 被弾アニメーションでα切り替え用
	int			damagedCnt;					// 被弾アニメーションカウンタ

	BOOL		isPopped;					// 弾かれたか？
	float		poppedCnt;					// 弾かれカウンタ
	XMFLOAT3	poppedSpeed;				// 弾かれ最大スピード

	int			level;						// グライダーレベル
	int			powerUp;					// パワーアップ数

	int			shootCnt;					// 連射カウンタ
		
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER* GetPlayer(void);

BOOL IsPlayerInsideUI(void);

void AddPlayerHP(int player, int add);

void AddPlayerPower(int player, int add);

void StartDeadCnt(void);