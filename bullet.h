//=============================================================================
//
// �o���b�g���� [bullet.h]
// Author : GP11B132 33 ���D
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

#include "enemy.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define BULLET_MAX		(200)		// �����ő�o���b�g��

// �o���b�g�^�C�v
enum {

	BULLET_TYPE_PLAYER_NORMAL,
	BULLET_TYPE_ENEMY_NORMAL,
	BULLET_TYPE_ENEMY_SLOW,

#ifdef BEZIER
	BULLET_TYPE_ENEMY_BEZIER,
#endif

	BULLET_TYPE_MAX,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

// �o���b�g�\����
struct BULLET
{
	BOOL			isActive;			// true:�g���Ă���  false:���g�p
	float			w, h;				// ���ƍ���

	XMFLOAT3		pos;				// �o���b�g�̍��W
	XMFLOAT3		rot;				// �o���b�g�̉�]��

	int				type;				// �o���b�g�^�C�v
	XMFLOAT3		speed;				// �X�s�[�h

	int				animFrameCnt;		// �A�j���[�V�����J�E���^
	int				animPattern;		// �\�����p�^�[��

	int				colliderW, colliderH;		// �����蔻��T�C�Y

	BOOL			isDelayed;			// �`��x���H(�����͂���Ǖ`��͂���Ȃ�)
	int				delayCnt;			// �x���J�E���^

	XMFLOAT3		start;				// �N�_�i�x�W�F�p�j
	XMFLOAT3		end;				// �I�_�i�x�W�F�p�j
	float			bezierT;			// ��s���ԁi�x�W�F�p�j
	float			bezierCnt;			// �ړ��J�E���^�i�x�W�F�p�j
};

// �o���b�g�f�[�^�\����
struct BULLET_DATA {
	
	char*			textureName;		// �e�N�X�`����

	XMINT2			size;				// �f�t�H���g�L�����T�C�Y
	int				mode;				// �`�惂�[�h
	float			alpha;				// ��

	XMFLOAT3		speed;				// �f�t�H���g�X�s�[�h

	XMINT2			patternDivide;		// �A�j���p�^�[���̃e�N�X�`����������
	int				patternNum;			// 1���[�v�p�^�[����
	int				wait;				// 1�p�^�[�����ێ�����t���[����

	int				colliderW, colliderH;		// �����蔻��T�C�Y
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET* GetBullet(void);
BULLET_DATA* GetBulletData(void);

void SetBullet(int type, XMFLOAT3 pos);							// �f�t�H���g�e
void SetBullet(int type, XMFLOAT3 pos, XMFLOAT3 speed);			// �����w��e

void SetBarrage(int type, XMFLOAT3 pos, float degree, float startDegree);					// �~�`�e��
void SetBarrage(int type, XMFLOAT3 pos, float degree, float startDegree, float endDegree);	// ��`�e��


/*
* @brief �ǔ��x�W�F�e��
* @param [in] type �e�̃^�C�v
* @param [in] start ���ˌ��̍��W
* @param [in] end �ڕW���W
* @param [in] t ��s����
*/
void SetBezierBullet(int type, XMFLOAT3 start, XMFLOAT3 end, float t);