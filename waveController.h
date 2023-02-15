//=============================================================================
//
// ウェーブコントローラ [waveController.h]
// Author : GP11B132 33 呉優
//
//=============================================================================
#pragma once

#include "main.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
enum {
	SKIP_GAME,
	SKIP_BOSS,
	SKIP_MAX,
};

// ゲーム段階
enum {
	PHASE_GAME,
	PHASE_BOSS,
	PHASE_ENTRO,	// ボスかプレイヤー死亡後
	PHASE_MAX,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct WAVE_DATA {

	int			launchTime;			// ウェーブの開始時間(ゲームモードに入ってから経過した秒数)(ミリ秒)
	XMFLOAT3	perLaunchPos;			// ローンチ座標(パーセンテージ表記)
	int			enemyType;			// エネミーの種類
	XMFLOAT3	enemySpeed;			// エネミーのスピード
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitWaveController(void);
void UninitWaveController(void);
void UpdateWaveController(void);

int GetTime(void);
int GetBossTime(void);

void SetGamePhase(int mode);
int GetGamePhase(void);