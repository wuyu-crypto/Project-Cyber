//=============================================================================
//
// �G�t�F�N�g���� [effect.h]
// Author : GP11B132 33 ���D
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define		EFFECT_MAX				(30)			// �����ő�G�t�F�N�g��

#define		EFFECT_LOOP_FOREVER		-1				// �����A�j���t���O

// �G�t�F�N�g�^�C�v�ԍ�
enum {
	EFFECT_TYPE_PLAYER_SHOT,

	EFFECT_TYPE_PLAYER_HIT,
	EFFECT_TYPE_ENEMY_HIT,

	EFFECT_TYPE_ENEMY_EXPLOSION,

	EFFECT_TYPE_PLAYER_BACKFIRE,
	EFFECT_TYPE_PLAYER_BACKGLOW,
	EFFECT_TYPE_PLAYER_BACKPARTICLES,

	EFFECT_TYPE_PLAYER_FLASH,

	EFFECT_TYPE_HEAL,
	EFFECT_TYPE_POWERUP,

	EFFECT_TYPE_PLAYER_POPPED00,
	EFFECT_TYPE_PLAYER_POPPED01,

	EFFECT_TYPE_BOSS_EXPLOSION,

	EFFECT_TYPE_MANUAL00,
	EFFECT_TYPE_MANUAL01,
	EFFECT_TYPE_MANUAL02,
	EFFECT_TYPE_MANUAL03,

	EFFECT_TYPE_MAX,
};

// �G�t�F�N�g�`�惌�C���[
enum {

	EFFECT_LAYER_BEHIND_PLAYER,			// �v���C���[�����ɕ`��
	EFFECT_LAYER_FRONT_OF_PLAYER,		// �v���C���[���O�ɕ`��

	EFFECT_LAYER_MAX,
};

//*****************************************************************************
// �\���̐錾
//*****************************************************************************

// �G�t�F�N�g�\����
struct EFFECT {

	BOOL			isActive;
	int				w, h;					// �L�����T�C�Y
	XMFLOAT3		pos;
	int				type;
	int				roop;					// ���[�v��(�f�t�H��1, ANIM_ROOP_ALWAYS���i�v)

	int				animFrameCnt;			// �A�j���[�V�����J�E���^
	int				animPattern;			// �\�����p�^�[��
	int				animTotalFrame;			// �S���[�v�̍��v�t���[����

	int				trackTarget;			// �Ǐ]�Ώ�(�����)
};

// �G�t�F�N�g�f�[�^�\����
struct EFFECT_DATA {

	char*			textureName;			// �e�N�X�`����

	XMINT2			size;					// �f�t�H���g�L�����T�C�Y
	int				mode;					// �`�惂�[�h
	float			alpha;					// ��

	int				trackLabel;				// �Ǐ]���x��
	XMFLOAT3		diff;					// �Ǐ]�Ώۂ���̃Y��

	BOOL			isOnly;					// �r���t���O(�����̓���G�t�F�N�g�����ׂĒ��~)

	XMINT2			patternDivide;			// �A�j���p�^�[���̃e�N�X�`����������
	int				patternNum;				// 1���[�v�p�^�[����
	int				wait;					// 1�p�^�[�����ێ�����t���[����

	int				layer;					// �`�惌�C���[
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEffect(void);
void UninitEffect(void);
void UpdateEffect(void);
void DrawEffect(int layer);

EFFECT* GetEffect(void);
EFFECT_DATA* GetEffectData(void);

void SetEffect(int type, int target, int loop);
void SetEffect(int type, XMFLOAT3 pos, int loop);

void StopEffect(int type);