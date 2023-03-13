//=============================================================================
//
// �v���C���[���� [player.h]
// Author : GP11B132 33 ���D
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define		PLAYER_MAX				1			// �v���C���[��Max�l��

#define		PLAYER_HP_MAX			8			// HP���

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
// �\���̒�`
//*****************************************************************************

struct PLAYER {

	XMFLOAT3	pos;						// �|���S���̍��W
	XMFLOAT3	rot;						// �|���S���̉�]��
	BOOL		isActive;					// true:�g���Ă���  false:���g�p
	float		w, h;						// ���ƍ���
	int			animFrameCnt;				// �A�j���[�V�����J�E���g
	int			animPattern;				// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;						// �e�N�X�`���ԍ��BUpdate�Ő���
	
	int			dirH;						// ���������̌���
	int			dirV;						// ���������̌���

	XMFLOAT3	speedOrg;					// �f�t�H���g�ړ����x
	XMFLOAT3	speed;						// �ړ����x

	int			hp;
	float		colliderW, colliderH;		// �����蔻��T�C�Y
	BOOL		isDamaged;					// ��e�O���O(���G���)
	BOOL		isAlphaChanged;		// ��e�A�j���[�V�����Ń��؂�ւ��p
	int			damagedCnt;					// ��e�A�j���[�V�����J�E���^

	BOOL		isPopped;					// �e���ꂽ���H
	float		poppedCnt;					// �e����J�E���^
	XMFLOAT3	poppedSpeed;				// �e����ő�X�s�[�h

	int			level;						// �O���C�_�[���x��
	int			powerUp;					// �p���[�A�b�v��

	int			shootCnt;					// �A�˃J�E���^
		
};


//*****************************************************************************
// �v���g�^�C�v�錾
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