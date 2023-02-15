//=============================================================================
//
// �r������ [building.h]
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
#define		BUILDING_MAX			5			// �r���̃��C���[��

enum {
	BUILDING_LABEL_00,
	BUILDING_LABEL_01,

	BUILDING_LABEL_MAX,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct BUILDING
{
	XMFLOAT3	pos;		// �|���S���̍��W
	BOOL		isActive;	// �L�����t���O
	float		w, h;		// ���ƍ���
	int			texNo;		// �g�p���Ă���e�N�X�`���ԍ�

	float		scrlSpeed;	// �X�N���[���X�s�[�h

	float		wait;		// �o���Ԋu
	float		waitCnt;	// �o���Ԋu�J�E���^
};

struct BUILDING_DATA {

	char*		textureName;

	float		w, h;
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBuilding(void);
void UninitBuilding(void);
void UpdateBuilding(void);
void DrawBuilding(void);