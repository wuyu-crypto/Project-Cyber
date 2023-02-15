//=============================================================================
//
// サウンド処理 [sound.h]
// Author : GP11B132 33 呉優
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_TITLE,	
	SOUND_LABEL_BGM_GAME,
	SOUND_LABEL_BGM_BOSS,
	SOUND_LABEL_BGM_RESULT,
	SOUND_LABEL_BGM_GAMEOVER,

	SOUND_LABEL_SE_SHOOT000,	// プレイヤー発射
	SOUND_LABEL_SE_SHOOT001,	// 
	SOUND_LABEL_SE_SHOOT002,	// 
	SOUND_LABEL_SE_SHOOT003,	// エネミー発射

	SOUND_LABEL_SE_EXPLOSION000,	// 敵死ぬ
	SOUND_LABEL_SE_EXPLOSION001,	// 
	SOUND_LABEL_SE_EXPLOSION002,	// デカい敵死ぬ
	SOUND_LABEL_SE_EXPLOSION003,	// プレイヤーかボス死ぬ

	SOUND_LABEL_SE_PLAYER_HIT,		// プレイヤー被弾
	SOUND_LABEL_SE_ENEMY_HIT,		// 敵被弾

	SOUND_LABEL_SE_HEAL,		// 回復
	SOUND_LABEL_SE_POWER_UP,	// パワーアップ

	SOUND_LABEL_SE_IMPACT,		// 衝撃

	SOUND_LABEL_SE_BUTTON_MOVE,		// ボタン移動
	SOUND_LABEL_SE_SELECT000,		// デカいボタン決定
	SOUND_LABEL_SE_SELECT001,		// 軽いボタン決定
	SOUND_LABEL_SE_CANCEL,			// キャンセル

	SOUND_LABEL_SE_CUTIN000,		// CUTIN
	SOUND_LABEL_SE_CUTIN001,		// 

	SOUND_LABEL_SE_PAUSE,			// PAUSE

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

