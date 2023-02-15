//=============================================================================
//
// �G�l�~�[���� [enemy.h]
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
#define ENEMY_MAX		20				// �G�l�~�[��Max�l��

#define	BOSS_HP_MAX		1000.0f			// �{�XHP

// �G�l�~�[�^�C�v�ԍ�
enum {
	ENEMY_TYPE00,			// �O�ւ��������Ȃ�
	ENEMY_TYPE01,			// �v���C���[�֌������Č����Ă���
	ENEMY_TYPE02,			// �U�e
	ENEMY_TYPE_BOSS,		// �{�X
	ENEMY_TYPE03,			// �{�X��ŌĂ΂��G��			

	ENEMY_TYPE_MAX,
};

// �O���ԍ�
enum {

	PATH00,
	PATH01,
	PATH02,
	PATH03,
	PATH04,
	PATH05,
	PATH06,

	PATH_MAX,
};


// BOSS�s���p�^�[��
enum {
	BOSS_SPAWNING,			// �o��

	BOSS_BARRAGE,			// ��]�e��
	BOSS_HOMING,			// �K�E�z�[�~���O

	BOSS_IDLING,			// �A�C�h�����O

	BOSS_CALLING,			// �G�����Ă�

	BOSS_SINKING,			// ����

	BOSS_MAX,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

// �G�l�~�[�\����
struct ENEMY {

	BOOL		isActive;			// �A�N�e�B�u�t���O
	float		w, h;				// ���ƍ���

	XMFLOAT3	pos;				// �|���S���̍��W
	XMFLOAT3	rot;				// �|���S���̉�]��
	XMFLOAT3	scl;				// �|���S���̊g��k��

	int			type;				// �G�l�~�[�^�C�v(���e�N�X�`���ԍ�)

	XMFLOAT3	speed;				// �X�s�[�h

	int			shootCnt;			// �ˌ��J�E���^
	int			shootPattern;		// �U���p�^�[���؂�ւ��p

	float		damagedAnimFrameCnt;	// �A�j���[�V�����J�E���g
	int			damagedAnimPattern;		// �A�j���[�V�����p�^�[���i���o�[

	int			colliderW, colliderH;			//�����蔻��T�C�Y
	BOOL		isDamaged;						// ��e�O���O

	int			hp;					// HP
	int			killScore;			// ���j���_
	int			hitScore;			// �������_

	int			dropRate;			// �h���b�v��
};

// �G�l�~�[�f�[�^�\����
struct ENEMY_DATA {

	char*			textureName;		// �e�N�X�`����

	XMINT2			size;				// �f�t�H���g�L�����T�C�Y

	int				bulletType;			// �o���b�g�^�C�v
	int				shootType;			// �e���˃p�^�[��
	int				shootWait;			// �e���˂̃E�F�C�g�l

	XMINT2			patternDivide;		// �A�j���p�^�[���̃e�N�X�`����������
	int				patternNum;			// 1���[�v�p�^�[����
	int				wait;				// 1�p�^�[�����ێ�����t���[����

	int				colliderW, colliderH;			//�����蔻��T�C�Y

	int				hp;					// HP
	int				killScore;			// ���j���_
	int				hitScore;			// �������_

	int				dropRate;			// �h���b�v��
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY* GetEnemy(void);

void SetEnemy(int type, XMFLOAT3 pos, XMFLOAT3 speed);

BOOL IsEnemyInsideUI(void);

void AddEnemyHP(int enemy, int add);
void KillEnemy(int enemy);

void SetBossAction(int mode);