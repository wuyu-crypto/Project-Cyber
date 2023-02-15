//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : GP11B132 33 ���D
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_TITLE,	
	SOUND_LABEL_BGM_GAME,
	SOUND_LABEL_BGM_BOSS,
	SOUND_LABEL_BGM_RESULT,
	SOUND_LABEL_BGM_GAMEOVER,

	SOUND_LABEL_SE_SHOOT000,	// �v���C���[����
	SOUND_LABEL_SE_SHOOT001,	// 
	SOUND_LABEL_SE_SHOOT002,	// 
	SOUND_LABEL_SE_SHOOT003,	// �G�l�~�[����

	SOUND_LABEL_SE_EXPLOSION000,	// �G����
	SOUND_LABEL_SE_EXPLOSION001,	// 
	SOUND_LABEL_SE_EXPLOSION002,	// �f�J���G����
	SOUND_LABEL_SE_EXPLOSION003,	// �v���C���[���{�X����

	SOUND_LABEL_SE_PLAYER_HIT,		// �v���C���[��e
	SOUND_LABEL_SE_ENEMY_HIT,		// �G��e

	SOUND_LABEL_SE_HEAL,		// ��
	SOUND_LABEL_SE_POWER_UP,	// �p���[�A�b�v

	SOUND_LABEL_SE_IMPACT,		// �Ռ�

	SOUND_LABEL_SE_BUTTON_MOVE,		// �{�^���ړ�
	SOUND_LABEL_SE_SELECT000,		// �f�J���{�^������
	SOUND_LABEL_SE_SELECT001,		// �y���{�^������
	SOUND_LABEL_SE_CANCEL,			// �L�����Z��

	SOUND_LABEL_SE_CUTIN000,		// CUTIN
	SOUND_LABEL_SE_CUTIN001,		// 

	SOUND_LABEL_SE_PAUSE,			// PAUSE

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

