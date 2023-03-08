//=============================================================================
//
// エネミー処理 [enemy.h]
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
#define ENEMY_MAX		20				// エネミーのMax人数

#ifndef DEMO
#define	BOSS_HP_MAX		1000.0f			// ボスHP
#else
#define BOSS_HP_MAX		300.0f
#endif

// エネミータイプ番号
enum {
	ENEMY_TYPE00,			// 前へしか撃たない
	ENEMY_TYPE01,			// プレイヤーへ向かって撃ってくる
	ENEMY_TYPE02,			// 散弾
	ENEMY_TYPE_BOSS,		// ボス
	ENEMY_TYPE03,			// ボス戦で呼ばれる雑魚			

	ENEMY_TYPE_MAX,
};

// 軌道番号
enum {

	PATH00,
	PATH01,
	PATH02,
	PATH03,
	PATH04,
	PATH05,
	PATH06,

	PATH_MAX,
};


// BOSS行動パターン
enum {
	BOSS_SPAWNING,			// 登場

	BOSS_BARRAGE,			// 回転弾幕
	BOSS_HOMING,			// 必殺ホーミング

	BOSS_IDLING,			// アイドリング

	BOSS_CALLING,			// 雑魚を呼ぶ

	BOSS_SINKING,			// 撃沈

	BOSS_MAX,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

// エネミー構造体
struct ENEMY {

	BOOL		isActive;			// アクティブフラグ
	float		w, h;				// 幅と高さ

	XMFLOAT3	pos;				// ポリゴンの座標
	XMFLOAT3	rot;				// ポリゴンの回転量
	XMFLOAT3	scl;				// ポリゴンの拡大縮小

	int			type;				// エネミータイプ(かつテクスチャ番号)

	XMFLOAT3	speed;				// スピード

	int			shootCnt;			// 射撃カウンタ
	int			shootPattern;		// 攻撃パターン切り替え用

	float		damagedAnimFrameCnt;	// アニメーションカウント
	int			damagedAnimPattern;		// アニメーションパターンナンバー

	int			colliderW, colliderH;			//当たり判定サイズ
	BOOL		isDamaged;						// 被弾グラグ

	int			hp;					// HP
	int			killScore;			// 撃破得点
	int			hitScore;			// 命中得点

	int			dropRate;			// ドロップ率
};

// エネミーデータ構造体
struct ENEMY_DATA {

	char*			textureName;		// テクスチャ名

	XMINT2			size;				// デフォルトキャラサイズ

	int				bulletType;			// バレットタイプ
	int				shootType;			// 弾発射パターン
	int				shootWait;			// 弾発射のウェイト値

	XMINT2			patternDivide;		// アニメパターンのテクスチャ内分割数
	int				patternNum;			// 1ループパターン数
	int				wait;				// 1パターンが維持するフレーム数

	int				colliderW, colliderH;			//当たり判定サイズ

	int				hp;					// HP
	int				killScore;			// 撃破得点
	int				hitScore;			// 命中得点

	int				dropRate;			// ドロップ率
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY* GetEnemy(void);

void SetEnemy(int type, XMFLOAT3 pos, XMFLOAT3 speed);

BOOL IsEnemyInsideUI(void);

void AddEnemyHP(int enemy, int add);
void KillEnemy(int enemy);

void SetBossAction(int mode);