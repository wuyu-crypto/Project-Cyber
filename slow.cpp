//=============================================================================
//
// �X���[�G�t�F�N�g [slow.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "slow.h"
#include "sound.h"
#include "enemy.h"
#include "bullet.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define			SLOW_FRAME_CNT				12		// �x���Ȃ�t���[����

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOOL		g_Load = FALSE;			// ���������s�������̃t���O

static int		g_Label;				// �X���[���x��

static int		g_FrameCnt;				// �t���[���J�E���^
static int		g_FrameNum;				// ���s����t���[����

static BOOL		g_isSlow;				// �X���[���H
static BOOL		g_isSlowApplied;		// �X���[���ŁA�X���[�����s����t���[���H



//=============================================================================
// ����������
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
// �I������
//=============================================================================
void UninitSlow(void)
{
	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSlow(void) {

	g_FrameCnt++;

	// �X���[�����s
	g_isSlowApplied = TRUE;

	// �X���[����t���[�����o�߂�����
	if (g_FrameCnt % SLOW_FRAME_CNT == 0) {

		// �X���[�����s���Ȃ�
		g_isSlowApplied = FALSE;
	}

	// �X���[�I���H
	if (g_FrameCnt < g_FrameNum) {
		return;
	}

	// ���x���ɂ���ďI���㏈��������
	switch (g_Label) {
	case SLOW_PLAYER_DIE:
		// ����SE
		PlaySound(SOUND_LABEL_SE_EXPLOSION002);
		PlaySound(SOUND_LABEL_SE_IMPACT);
		break;

	case SLOW_BOSS_DIE:
		// ���USE
		PlaySound(SOUND_LABEL_SE_EXPLOSION002);
		PlaySound(SOUND_LABEL_SE_IMPACT);

		ENEMY* enemy = GetEnemy();
		for (int i = 0; i < ENEMY_MAX; i++) {

			// �����Ȃ���s���Ȃ�
			if (enemy[i].isActive == FALSE) {
				continue;
			}

			if (enemy[i].type == ENEMY_TYPE_BOSS) {

				// ��e�A�j���[�V����������
				enemy[i].isDamaged = FALSE;

				// �{�X���������[�h�ɂ���
				SetBossAction(BOSS_SINKING);
			}
			else {
				// �����G�l�~�[���F�E��
				KillEnemy(i);
			}

		}
		break;
	}

	// ���[�h��߂�
	SetModeWithoutReload(MODE_GAME);
}

//=============================================================================
// �X�g�b�v�G�t�F�N�g���Z�b�g
//=============================================================================
void SetSlow(int label, int frameNum) {

	g_Label = label;
	g_FrameCnt = 0;

	// NULL�Ȃ�f�t�H���g�t���[�������Z�b�g
	if (frameNum == NULL) {
		g_FrameNum = SLOW_FRAME_NUM_DEFAULT;
	}

	// �t���[�������Z�b�g
	else {
		g_FrameNum = frameNum;
	}

	SetModeWithoutReload(MODE_GAME_SLOW);
}

//=============================================================================
// �X���[��Ԃ��擾
//=============================================================================
BOOL IsSlowApplied(void) {
	return g_isSlowApplied;
}