//=============================================================================
//
// BG���� [bg.h]
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
struct BG
{
	XMFLOAT3	pos;		// �|���S���̍��W
	BOOL		isActive;	// �L�����t���O
	float		w, h;		// ���ƍ���
	int			texNo;		// �g�p���Ă���e�N�X�`���ԍ�

	float		scrl;		// �X�N���[��
};


#define	SKY_H	(150.0f)	// ��̃|���S���̍���


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBG(void);
void UninitBG(void);
void UpdateBG(void);
void DrawBG(void);

BG* GetBG(void);

void SetVibration(float frame, float roop, float scale);