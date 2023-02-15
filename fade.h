//=============================================================================
//
// �t�F�[�h���� [fade.h]
// Author : GP11B132 33 ���D
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************

// �t�F�[�h�̏��
typedef enum
{
	FADE_NONE = 0,		// �t�F�[�h���Ȃ��i�Z�b�g���[�h�����j
	FADE_IN,			// �t�F�[�h�C������
	FADE_OUT,			// �t�F�[�h�A�E�g����
	FADE_MAX
} FADE;



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitFade(void);
void UninitFade(void);
void UpdateFade(void);
void DrawFade(void);

void SetFade(FADE fade, int modeNext);
FADE GetFade(void);


