//=============================================================================
//
// �E�F�[�u�R���g���[�� [waveController.cpp]
// Author : GP11B132 33 ���D
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
// �}�N����`
//*****************************************************************************
#define	STAGE_MAX						30				// �X�e�[�W�����
#define	ENEMY_SPEED_PREHAB_MAX			10				// �X�s�[�h�v���n�u�����
#define	ENEMY_SPOWN_PREHAB_MAX			10				// �X�|�[�����W�����
#define	SPOWN_POS_PER_X					1.1f			// �X�|�[���n�_��X���W(�p�[�Z���e�[�W�\�L)

#define	FPS_WAVE						FPS				// WAVE����p

#define	START_TIME						0				// �Q�[�����[�h�����ĉ��b��ɃJ�E���g���n�߂�̂�

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void SkipGame(int mode);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOOL	g_Load = FALSE;		// ���������s�������̃t���O

static int g_FrameCnt;			// �t���[���J�E���^
static int g_Time;				// �Q�[�����[�h�o�ߎ���(�P�ʁF�b)

static BOOL g_IsGameSet;		// �Q�[���I���t���O(�Ō�ɂ��낢�떳�����Ɏg��)

///////////////////////////////////////////////////////////////////////////////
// �G�l�~�[�X�s�[�h�v���n�u
///////////////////////////////////////////////////////////////////////////////
static XMFLOAT3 speed[ENEMY_SPEED_PREHAB_MAX] = {

	XMFLOAT3(-1.8f, 0.0f, 0.0f),
	XMFLOAT3(-2.1f, 0.0f, 0.0f),	// �f�t�H���g
	XMFLOAT3(-2.6f, 0.0f, 0.0f),
	XMFLOAT3(-2.9f, 0.0f, 0.0f),
};

static XMFLOAT3 pos[ENEMY_SPOWN_PREHAB_MAX] = {

	XMFLOAT3(1.1f, 0.1f, 0.0f),		// 0
	XMFLOAT3(1.1f, 0.2f, 0.0f),
	XMFLOAT3(1.1f, 0.3f, 0.0f),
	XMFLOAT3(1.1f, 0.4f, 0.0f),
	XMFLOAT3(1.1f, 0.5f, 0.0f),		// ����
	XMFLOAT3(1.1f, 0.6f, 0.0f),
	XMFLOAT3(1.1f, 0.7f, 0.0f),
	XMFLOAT3(1.1f, 0.8f, 0.0f),
	XMFLOAT3(1.1f, 0.9f, 0.0f),		// 8

	XMFLOAT3(2.0f, 0.52f, 0.0f),		// �{�X�p

};

static int g_BossTime;

///////////////////////////////////////////////////////////////////////////////
// �E�F�[�u�f�[�^�e�[�u��
///////////////////////////////////////////////////////////////////////////////
#ifndef DEMO	// �ʏ��
static WAVE_DATA g_WaveTbl[] = {

	  // �J�n����	// �X�|�[�����W		// �G�l�~�[�^�C�v	// �X�s�[�h

	// 00����
	{ 3,			pos[4],			ENEMY_TYPE00,		speed[3],	},
	{ 5,			pos[4],			ENEMY_TYPE00,		speed[3],	},
	{ 7,			pos[4],			ENEMY_TYPE00,		speed[3],	},
	
	// 00�̕�
	{ 10,			pos[1],			ENEMY_TYPE00,		speed[3],	},
	{ 10,			pos[3],			ENEMY_TYPE00,		speed[2],	},
	{ 12,			pos[5],			ENEMY_TYPE00,		speed[3],	},
	{ 12,			pos[7],			ENEMY_TYPE00,		speed[2],	},

	{ 14,			pos[2],			ENEMY_TYPE00,		speed[3],	},
	{ 14,			pos[4],			ENEMY_TYPE00,		speed[2],	},
	{ 16,			pos[6],			ENEMY_TYPE00,		speed[3],	},
	{ 16,			pos[8],			ENEMY_TYPE00,		speed[2],	},

	// 01���o��
	{ 25,			pos[4],			ENEMY_TYPE01,		speed[2],	},

	{ 27,			pos[2],			ENEMY_TYPE01,		speed[1],	},
	{ 27,			pos[6],			ENEMY_TYPE01,		speed[1],	},

	{ 30,			pos[4],			ENEMY_TYPE01,		speed[2],	},

	// 00������01����납��X�i�C�v
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

	// 01�w�`
	{ 56,			pos[4],			ENEMY_TYPE01,		speed[2],	},
	{ 56,			pos[2],			ENEMY_TYPE01,		speed[1],	},
	{ 56,			pos[6],			ENEMY_TYPE01,		speed[2],	},
	{ 56,			pos[8],			ENEMY_TYPE01,		speed[1],	},

	{ 60,			pos[4],			ENEMY_TYPE01,		speed[1],	},
	{ 60,			pos[2],			ENEMY_TYPE01,		speed[1],	},
	{ 60,			pos[6],			ENEMY_TYPE01,		speed[1],	},
	{ 60,			pos[8],			ENEMY_TYPE01,		speed[1],	},

	// 02���o��
	{ 70,			pos[2],			ENEMY_TYPE02,		speed[2],	},
	{ 70,			pos[6],			ENEMY_TYPE02,		speed[2],	},

	// 02������00������
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

	// �嗐��A02��|�����悤��
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
#else
static WAVE_DATA g_WaveTbl[] = {

	// �J�n����	// �X�|�[�����W		// �G�l�~�[�^�C�v	// �X�s�[�h

	// 00����
	{ 3,			pos[2],			ENEMY_TYPE00,		speed[3],	},
	{ 3,			pos[4],			ENEMY_TYPE00,		speed[3],	},
	{ 5,			pos[3],			ENEMY_TYPE00,		speed[3],	},
	{ 5,			pos[5],			ENEMY_TYPE00,		speed[3],	},
	{ 7,			pos[4],			ENEMY_TYPE00,		speed[3],	},
	{ 7,			pos[6],			ENEMY_TYPE00,		speed[3],	},

	{ 10,			pos[4],			ENEMY_TYPE01,		speed[2],	},
	{ 12,			pos[2],			ENEMY_TYPE01,		speed[1],	},
	{ 12,			pos[6],			ENEMY_TYPE01,		speed[1],	},

	{ 16,			pos[4],			ENEMY_TYPE02,		speed[2],	},
	{ 19,			pos[2],			ENEMY_TYPE02,		speed[2],	},
	{ 19,			pos[6],			ENEMY_TYPE02,		speed[2],	},
	{ 23,			pos[0],			ENEMY_TYPE02,		speed[2],	},
	{ 23,			pos[8],			ENEMY_TYPE02,		speed[2],	},

	{ 35,			pos[9],			ENEMY_TYPE_BOSS,	speed[0],	},

};
#endif

static WAVE_DATA* g_WaveTblAdr = g_WaveTbl;



static int g_WaveNo;			// ���s���E�F�[�u
static int g_WaveMax;			// �E�F�[�u���R�[�h�̐�

static int g_Phase;				// �Q�[���t�F�[�X

//=============================================================================
// ����������
//=============================================================================
HRESULT InitWaveController(void) {

	g_FrameCnt = 0;
	g_Time = -START_TIME;

	g_WaveNo = 0;				// ���Ɏ��s����E�F�[�u�����Z�b�g
	g_WaveMax = sizeof(g_WaveTbl) / sizeof(WAVE_DATA);	// �E�F�[�u���R�[�h�̐����擾

	// BossTime���擾
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
// �I������
//=============================================================================
void UninitWaveController(void) {

	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateWaveController(void) {

	// PAUSE����
	if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonPressed(0, BUTTON_X)) {
		SetPause(TRUE);
		PlaySound(SOUND_LABEL_SE_PAUSE);
	}

	// SKIP����
	if (GetKeyboardTrigger(DIK_1)) {
		SkipGame(SKIP_GAME);
	}
	if (GetKeyboardTrigger(DIK_2)) {
		SkipGame(SKIP_BOSS);
	}

	// �ݒ�ς݂̃E�F�[�u���𒴂�����Z�b�g���Ȃ�
	if (g_WaveNo >= g_WaveMax) {
		return;
	}

	if (g_Time >= g_WaveTbl[g_WaveNo].launchTime) {			// ���Ԃ������Ă����

		// �E�F�[�u���Z�b�g
		SetEnemy(g_WaveTbl[g_WaveNo].enemyType, g_WaveTbl[g_WaveNo].perLaunchPos, g_WaveTbl[g_WaveNo].enemySpeed);
		
		// �E�F�[�u�ԍ���i�߂�(���s�ς݃E�F�[�u���`�F�b�N���Ȃ��ėǂ�����)
		g_WaveNo++;	
	}
	
	// ���Ԃ�i�߂�(DEBUG��~���ł��i��)
	g_FrameCnt++;
	if (g_FrameCnt == FPS_WAVE) {
		g_FrameCnt = 0;
		g_Time++;
	}
}

//=============================================================================
// Time���擾(START_TIME�܂�)
//=============================================================================
int GetTime(void) {
	return g_Time + START_TIME;
}

//=============================================================================
// �{�X�̏o�����Ԃ��擾(START_TIME�܂�)
//=============================================================================
int GetBossTime(void) {
	return g_BossTime + START_TIME;
}

//=============================================================================
// �Q�[���X�L�b�v
//=============================================================================
void SkipGame(int mode) {

	switch (mode) {
	case SKIP_GAME:
	{
		// ���łɃX�L�b�v����Ă��Ȃ����
		if (g_Phase == PHASE_GAME) {
			// ���X�g�E�F�[�u�̏����擾
			int waveNo = g_WaveMax - 1;
			// ���Ԃ��΂�
			g_Time = g_WaveTbl[waveNo].launchTime;
			// �E�F�[�u�ԍ����΂�
			g_WaveNo = waveNo;
			// �G�l�~�[�����ׂĎE��
			for (int i = 0; i < ENEMY_MAX; i++) {
				ENEMY* enemy = GetEnemy();
				
				// �����Ă���G�l�~�[�̂�
				if (enemy[i].isActive == TRUE) {
				KillEnemy(i);
				}
			}
		}
	}
		break;

	case SKIP_BOSS:
	{
		// ���łɃX�L�b�v����Ă��Ȃ����
		if (g_Phase == PHASE_BOSS) {
			// ���X�g�E�F�[�u�̏����擾
			int waveNo = g_WaveMax - 1;
			// �S�G�l�~�[���E��
			for (int i = 0; i < ENEMY_MAX; i++) {
				ENEMY* enemy = GetEnemy();

				// �����Ă���G�l�~�[�̂�
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
// �Q�[���t�F�[�X���Z�b�g
//=============================================================================
void SetGamePhase(int mode) {
	g_Phase = mode;

	// �t�F�[�X�ɂ���ď���
	if (mode == PHASE_BOSS) {
		EFFECT* effect = GetEffect();

		// �����q���g������
		for (int i = 0; i < EFFECT_MAX; i++) {
			if (effect[i].type == EFFECT_TYPE_MANUAL01) {
				effect[i].isActive = FALSE;
			}
		}
	}
	else if (mode == PHASE_ENTRO) {
		EFFECT* effect = GetEffect();
		
		// �����q���g������
		for (int i = 0; i < EFFECT_MAX; i++) {
			if (effect[i].type == EFFECT_TYPE_MANUAL01) {
				effect[i].isActive = FALSE;
			}
		}

		// �X�L�b�v�q���g���o��
		SetEffect(EFFECT_TYPE_MANUAL03, XMFLOAT3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.95, 0.0f), EFFECT_LOOP_FOREVER);
	}
}

//=============================================================================
// �Q�[���t�F�[�X���Q�b�g
//=============================================================================
int GetGamePhase(void) {
	return g_Phase;
}