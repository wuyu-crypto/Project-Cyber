//=============================================================================
//
// ウェーブコントローラ [waveController.cpp]
// Author : GP11B132 33 呉優
// 
//
//=============================================================================
#include "waveController.h"
#include "enemy.h"
#include "pause.h"
#include "input.h"
#include "sound.h"
#include "effect.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	STAGE_MAX						30				// ステージ上限数
#define	ENEMY_SPEED_PREHAB_MAX			10				// スピードプレハブ上限数
#define	ENEMY_SPOWN_PREHAB_MAX			10				// スポーン座標上限数
#define	SPOWN_POS_PER_X					1.1f			// スポーン地点のX座標(パーセンテージ表記)

#define	FPS_WAVE						FPS				// WAVE制御用

#define	START_TIME						0				// ゲームモード入って何秒後にカウントし始めるのか

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SkipGame(int mode);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ

static int g_FrameCnt;			// フレームカウンタ
static int g_Time;				// ゲームモード経過時間(単位：秒)

static BOOL g_IsGameSet;		// ゲーム終了フラグ(最後にいろいろ無効化に使う)

///////////////////////////////////////////////////////////////////////////////
// エネミースピードプレハブ
///////////////////////////////////////////////////////////////////////////////
static XMFLOAT3 speed[ENEMY_SPEED_PREHAB_MAX] = {

	XMFLOAT3(-1.8f, 0.0f, 0.0f),
	XMFLOAT3(-2.1f, 0.0f, 0.0f),	// デフォルト
	XMFLOAT3(-2.6f, 0.0f, 0.0f),
	XMFLOAT3(-2.9f, 0.0f, 0.0f),
};

static XMFLOAT3 pos[ENEMY_SPOWN_PREHAB_MAX] = {

	XMFLOAT3(1.1f, 0.1f, 0.0f),		// 0
	XMFLOAT3(1.1f, 0.2f, 0.0f),
	XMFLOAT3(1.1f, 0.3f, 0.0f),
	XMFLOAT3(1.1f, 0.4f, 0.0f),
	XMFLOAT3(1.1f, 0.5f, 0.0f),		// 中央
	XMFLOAT3(1.1f, 0.6f, 0.0f),
	XMFLOAT3(1.1f, 0.7f, 0.0f),
	XMFLOAT3(1.1f, 0.8f, 0.0f),
	XMFLOAT3(1.1f, 0.9f, 0.0f),		// 8

	XMFLOAT3(2.0f, 0.52f, 0.0f),		// ボス用

};

static int g_BossTime;

///////////////////////////////////////////////////////////////////////////////
// ウェーブデータテーブル
///////////////////////////////////////////////////////////////////////////////
static WAVE_DATA g_WaveTbl[] = {

	  // 開始時間	// スポーン座標		// エネミータイプ	// スピード

	// 00だけ
	{ 3,			pos[4],			ENEMY_TYPE00,		speed[3],	},
	{ 5,			pos[4],			ENEMY_TYPE00,		speed[3],	},
	{ 7,			pos[4],			ENEMY_TYPE00,		speed[3],	},
	
	// 00の壁
	{ 10,			pos[1],			ENEMY_TYPE00,		speed[3],	},
	{ 10,			pos[3],			ENEMY_TYPE00,		speed[2],	},
	{ 12,			pos[5],			ENEMY_TYPE00,		speed[3],	},
	{ 12,			pos[7],			ENEMY_TYPE00,		speed[2],	},

	{ 14,			pos[2],			ENEMY_TYPE00,		speed[3],	},
	{ 14,			pos[4],			ENEMY_TYPE00,		speed[2],	},
	{ 16,			pos[6],			ENEMY_TYPE00,		speed[3],	},
	{ 16,			pos[8],			ENEMY_TYPE00,		speed[2],	},

	// 01初登場
	{ 25,			pos[4],			ENEMY_TYPE01,		speed[2],	},

	{ 27,			pos[2],			ENEMY_TYPE01,		speed[1],	},
	{ 27,			pos[6],			ENEMY_TYPE01,		speed[1],	},

	{ 30,			pos[4],			ENEMY_TYPE01,		speed[2],	},

	// 00を盾に01が後ろからスナイプ
	{ 40,			pos[2],			ENEMY_TYPE01,		speed[0],	},
	{ 40,			pos[4],			ENEMY_TYPE01,		speed[0],	},
	{ 40,			pos[6],			ENEMY_TYPE01,		speed[0],	},
	{ 40,			pos[8],			ENEMY_TYPE01,		speed[0],	},

	{ 43,			pos[1],			ENEMY_TYPE00,		speed[2],	},
	{ 43,			pos[3],			ENEMY_TYPE00,		speed[2],	},
	{ 43,			pos[5],			ENEMY_TYPE00,		speed[2],	},
	{ 43,			pos[7],			ENEMY_TYPE00,		speed[2],	},

	{ 46,			pos[1],			ENEMY_TYPE00,		speed[2],	},
	{ 46,			pos[3],			ENEMY_TYPE00,		speed[2],	},
	{ 46,			pos[5],			ENEMY_TYPE00,		speed[2],	},
	{ 46,			pos[7],			ENEMY_TYPE00,		speed[2],	},

	// 01陣形
	{ 56,			pos[4],			ENEMY_TYPE01,		speed[2],	},
	{ 56,			pos[2],			ENEMY_TYPE01,		speed[1],	},
	{ 56,			pos[6],			ENEMY_TYPE01,		speed[2],	},
	{ 56,			pos[8],			ENEMY_TYPE01,		speed[1],	},

	{ 60,			pos[4],			ENEMY_TYPE01,		speed[1],	},
	{ 60,			pos[2],			ENEMY_TYPE01,		speed[1],	},
	{ 60,			pos[6],			ENEMY_TYPE01,		speed[1],	},
	{ 60,			pos[8],			ENEMY_TYPE01,		speed[1],	},

	// 02初登場
	{ 70,			pos[2],			ENEMY_TYPE02,		speed[2],	},
	{ 70,			pos[6],			ENEMY_TYPE02,		speed[2],	},

	// 02を後ろに00が攪乱
	{ 78,			pos[2],			ENEMY_TYPE02,		speed[0], },
	{ 78,			pos[4],			ENEMY_TYPE02,		speed[1], },
	{ 78,			pos[6],			ENEMY_TYPE02,		speed[0], },

	{ 80,			pos[1],			ENEMY_TYPE00,		speed[2], },
	{ 80,			pos[3],			ENEMY_TYPE00,		speed[2], },
	{ 80,			pos[5],			ENEMY_TYPE00,		speed[2], },
	{ 80,			pos[7],			ENEMY_TYPE00,		speed[2], },

	{ 83,			pos[1],			ENEMY_TYPE00,		speed[2], },
	{ 83,			pos[3],			ENEMY_TYPE00,		speed[2], },
	{ 83,			pos[5],			ENEMY_TYPE00,		speed[2], },
	{ 83,			pos[7],			ENEMY_TYPE00,		speed[2], },

	// 大乱戦、02を倒させように
	{ 93,			pos[4],			ENEMY_TYPE02,		speed[0], },

	{ 95,			pos[2],			ENEMY_TYPE01,		speed[1], },
	{ 95,			pos[6],			ENEMY_TYPE01,		speed[1], },
	
	{ 97,			pos[1],			ENEMY_TYPE00,		speed[2], },
	{ 97,			pos[3],			ENEMY_TYPE00,		speed[2], },
	{ 97,			pos[5],			ENEMY_TYPE00,		speed[2], },

	{ 101,			pos[2],			ENEMY_TYPE00,		speed[2], },
	{ 101,			pos[4],			ENEMY_TYPE00,		speed[2], },
	{ 101,			pos[6],			ENEMY_TYPE00,		speed[2], },

	{ 103,			pos[2],			ENEMY_TYPE02,		speed[2], },
	{ 103,			pos[6],			ENEMY_TYPE02,		speed[2], },

	{ 106,			pos[2],			ENEMY_TYPE02,		speed[2], },
	{ 106,			pos[6],			ENEMY_TYPE02,		speed[2], },
	
	{ 109,			pos[2],			ENEMY_TYPE02,		speed[3], },
	{ 109,			pos[6],			ENEMY_TYPE02,		speed[3], },

	{ 123,			pos[9],			ENEMY_TYPE_BOSS,	speed[0], },

};



static WAVE_DATA* g_WaveTblAdr = g_WaveTbl;



static int g_WaveNo;			// 実行中ウェーブ
static int g_WaveMax;			// ウェーブレコードの数

static int g_Phase;				// ゲームフェース

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitWaveController(void) {

	g_FrameCnt = 0;
	g_Time = -START_TIME;

	g_WaveNo = 0;				// 次に実行するウェーブをリセット
	g_WaveMax = sizeof(g_WaveTbl) / sizeof(WAVE_DATA);	// ウェーブレコードの数を取得

	// BossTimeを取得
	for (int i = 0; i < g_WaveMax; i++) {

		if (g_WaveTbl[i].enemyType == ENEMY_TYPE_BOSS) {
			g_BossTime = g_WaveTbl[i].launchTime;
		}
	}

	g_IsGameSet = FALSE;

	g_Phase = PHASE_GAME;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitWaveController(void) {

	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateWaveController(void) {

	// PAUSE処理
	if (GetKeyboardTrigger(DIK_ESCAPE)) {
		SetPause(TRUE);
		PlaySound(SOUND_LABEL_SE_PAUSE);
	}

	// SKIP処理
	if (GetKeyboardTrigger(DIK_1)) {
		SkipGame(SKIP_GAME);
	}
	if (GetKeyboardTrigger(DIK_2)) {
		SkipGame(SKIP_BOSS);
	}

	// 設定済みのウェーブ数を超えたらセットしない
	if (g_WaveNo >= g_WaveMax) {
		return;
	}

	if (g_Time >= g_WaveTbl[g_WaveNo].launchTime) {			// 時間が合っていれば

		// ウェーブをセット
		SetEnemy(g_WaveTbl[g_WaveNo].enemyType, g_WaveTbl[g_WaveNo].perLaunchPos, g_WaveTbl[g_WaveNo].enemySpeed);
		
		// ウェーブ番号を進める(実行済みウェーブをチェックしなくて良くする)
		g_WaveNo++;	
	}
	
	// 時間を進める(DEBUG停止中でも進む)
	g_FrameCnt++;
	if (g_FrameCnt == FPS_WAVE) {
		g_FrameCnt = 0;
		g_Time++;
	}
}

//=============================================================================
// Timeを取得(START_TIME含め)
//=============================================================================
int GetTime(void) {
	return g_Time + START_TIME;
}

//=============================================================================
// ボスの出現時間を取得(START_TIME含め)
//=============================================================================
int GetBossTime(void) {
	return g_BossTime + START_TIME;
}

//=============================================================================
// ゲームスキップ
//=============================================================================
void SkipGame(int mode) {

	switch (mode) {
	case SKIP_GAME:
	{
		// すでにスキップされていなければ
		if (g_Phase == PHASE_GAME) {
			// ラストウェーブの情報を取得
			int waveNo = g_WaveMax - 1;
			// 時間を飛ばす
			g_Time = g_WaveTbl[waveNo].launchTime;
			// ウェーブ番号を飛ばす
			g_WaveNo = waveNo;
			// エネミーをすべて殺す
			for (int i = 0; i < ENEMY_MAX; i++) {
				ENEMY* enemy = GetEnemy();
				
				// 生きているエネミーのみ
				if (enemy[i].isActive == TRUE) {
				KillEnemy(i);
				}
			}
		}
	}
		break;

	case SKIP_BOSS:
	{
		// すでにスキップされていなければ
		if (g_Phase == PHASE_BOSS) {
			// ラストウェーブの情報を取得
			int waveNo = g_WaveMax - 1;
			// 全エネミーを殺す
			for (int i = 0; i < ENEMY_MAX; i++) {
				ENEMY* enemy = GetEnemy();

				// 生きているエネミーのみ
				if (enemy[i].isActive == TRUE) {
					KillEnemy(i);
				}
			}
			SetGamePhase(PHASE_ENTRO);
		}
	}
			break;
	}
}

//=============================================================================
// ゲームフェースをセット
//=============================================================================
void SetGamePhase(int mode) {
	g_Phase = mode;

	// フェースによって処理
	if (mode == PHASE_BOSS) {
		EFFECT* effect = GetEffect();

		// 既存ヒントを消す
		for (int i = 0; i < EFFECT_MAX; i++) {
			if (effect[i].type == EFFECT_TYPE_MANUAL01) {
				effect[i].isActive = FALSE;
			}
		}
	}
	else if (mode == PHASE_ENTRO) {
		EFFECT* effect = GetEffect();
		
		// 既存ヒントを消す
		for (int i = 0; i < EFFECT_MAX; i++) {
			if (effect[i].type == EFFECT_TYPE_MANUAL01) {
				effect[i].isActive = FALSE;
			}
		}

		// スキップヒントを出す
		SetEffect(EFFECT_TYPE_MANUAL03, XMFLOAT3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.95, 0.0f), EFFECT_LOOP_FOREVER);
	}
}

//=============================================================================
// ゲームフェースをゲット
//=============================================================================
int GetGamePhase(void) {
	return g_Phase;
}