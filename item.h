//=============================================================================
//
// �A�C�e�� [item.h]
// Author : GP11B132 33 ���D
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"
#include "debugproc.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define			ITEM_MAX			10			// �����ő�A�C�e����

// �h���b�v�A�C�e���^�C�v
enum {

	ITEM_TYPE_HEART,		// ��
	ITEM_TYPE_POWER,		// �p���[�A�b�v

	ITEM_TYPE_MAX,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct ITEM
{
	BOOL			isActive;					// �L�����t���O
	float			w, h;						// ���ƍ���

	XMFLOAT3		pos;						// ���W
	int				type;						// �^�C�v

	XMFLOAT3		speed;						// �X�s�[�h

	int				animFrameCnt;				// �A�j���[�V�����J�E���^
	int				animPattern;				// �\�����p�^�[��

	int				colliderW, colliderH;		// �����蔻��T�C�Y
};


struct ITEM_DATA {

	char* textureName;		// �e�N�X�`����
	float			w, h;				// �f�t�H���g�L�����T�C�Y

	XMFLOAT3		speed;				// �f�t�H���g�X�s�[�h

	int				patternDivideX, patternDivideY;		// �A�j���p�^�[���̃e�N�X�`����������
	int				patternNum;							// �p�^�[����
	int				wait;								// �E�F�C�g�l

	int				colliderW, colliderH;				// �����蔻��T�C�Y
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitItem(void);
void UninitItem(void);
void UpdateItem(void);
void DrawItem(void);

void SetItem(int type, XMFLOAT3 pos);