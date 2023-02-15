//=============================================================================
//
// スローエフェクト [slow.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "slow.h"
#include "sound.h"
#include "enemy.h"
#include "bullet.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define			SLOW_FRAME_CNT				12		// 遅くなるフレーム分

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ

static int		g_Label;				// スローラベル

static int		g_FrameCnt;				// フレームカウンタ
static int		g_FrameNum;				// 実行するフレーム数

static BOOL		g_isSlow;				// スロー中？
static BOOL		g_isSlowApplied;		// スロー中で、スローを実行するフレーム？



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSlow(void)
{
	g_FrameCnt = 0;
	g_FrameNum = 0;

	g_isSlow = FALSE;
	g_isSlowApplied = FALSE;

	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSlow(void)
{
	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSlow(void) {

	g_FrameCnt++;

	// スローを実行
	g_isSlowApplied = TRUE;

	// スローするフレーム分経過したら
	if (g_FrameCnt % SLOW_FRAME_CNT == 0) {

		// スローを実行しない
		g_isSlowApplied = FALSE;
	}

	// スロー終了？
	if (g_FrameCnt < g_FrameNum) {
		return;
	}

	// ラベルによって終了後処理をする
	switch (g_Label) {
	case SLOW_PLAYER_DIE:
		// 爆死SE
		PlaySound(SOUND_LABEL_SE_EXPLOSION002);
		PlaySound(SOUND_LABEL_SE_IMPACT);
		break;

	case SLOW_BOSS_DIE:
		// 爆散SE
		PlaySound(SOUND_LABEL_SE_EXPLOSION002);
		PlaySound(SOUND_LABEL_SE_IMPACT);

		ENEMY* enemy = GetEnemy();
		for (int i = 0; i < ENEMY_MAX; i++) {

			// 無効なら実行しない
			if (enemy[i].isActive == FALSE) {
				continue;
			}

			if (enemy[i].type == ENEMY_TYPE_BOSS) {

				// 被弾アニメーションを消す
				enemy[i].isDamaged = FALSE;

				// ボスを撃沈モードにする
				SetBossAction(BOSS_SINKING);
			}
			else {
				// 既存エネミーを皆殺し
				KillEnemy(i);
			}

		}
		break;
	}

	// モードを戻す
	SetModeWithoutReload(MODE_GAME);
}

//=============================================================================
// ストップエフェクトをセット
//=============================================================================
void SetSlow(int label, int frameNum) {

	g_Label = label;
	g_FrameCnt = 0;

	// NULLならデフォルトフレーム数をセット
	if (frameNum == NULL) {
		g_FrameNum = SLOW_FRAME_NUM_DEFAULT;
	}

	// フレーム数をセット
	else {
		g_FrameNum = frameNum;
	}

	SetModeWithoutReload(MODE_GAME_SLOW);
}

//=============================================================================
// スロー状態を取得
//=============================================================================
BOOL IsSlowApplied(void) {
	return g_isSlowApplied;
}