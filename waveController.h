//=============================================================================
//
// �E�F�[�u�R���g���[�� [waveController.h]
// Author : GP11B132 33 ���D
//
//=============================================================================
#pragma once

#include "main.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
enum {
	SKIP_GAME,
	SKIP_BOSS,
	SKIP_MAX,
};

// �Q�[���i�K
enum {
	PHASE_GAME,
	PHASE_BOSS,
	PHASE_ENTRO,	// �{�X���v���C���[���S��
	PHASE_MAX,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct WAVE_DATA {

	int			launchTime;			// �E�F�[�u�̊J�n����(�Q�[�����[�h�ɓ����Ă���o�߂����b��)(�~���b)
	XMFLOAT3	perLaunchPos;			// ���[���`���W(�p�[�Z���e�[�W�\�L)
	int			enemyType;			// �G�l�~�[�̎��
	XMFLOAT3	enemySpeed;			// �G�l�~�[�̃X�s�[�h
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitWaveController(void);
void UninitWaveController(void);
void UpdateWaveController(void);

int GetTime(void);
int GetBossTime(void);

void SetGamePhase(int mode);
int GetGamePhase(void);