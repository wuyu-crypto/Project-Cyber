//=============================================================================
//
// ���C������ [main.h]
// Author : GP11B132 33 ���D
//
//=============================================================================
#pragma once


#pragma warning(push)
#pragma warning(disable:4005)

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC				// ���������[�N
#endif // _DEBUG

#define _CRT_SECURE_NO_WARNINGS			// scanf ��warning�h�~
#include <stdio.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <math.h>

#include <DirectXMath.h>

#ifdef _DEBUG							// ���������[�N
#include <crtdbg.h>
#define new  ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


// �{���̓w�b�_�ɏ����Ȃ������ǂ�
using namespace DirectX;


#define DIRECTINPUT_VERSION 0x0800		// �x���Ώ�
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)




#define	BEZIER	// �x�W�F�e��
#undef BEZIER


#define	DEMO	// �f���p�팸�Łi�X�e�[�W�팸�j
//#undef DEMO


//*****************************************************************************
// ���C�u�����̃����N
//*****************************************************************************
#pragma comment (lib, "d3d11.lib")		
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3dx11.lib")	
#pragma comment (lib, "d3dx9.lib")	
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SCREEN_WIDTH	(1920)			// �E�C���h�E�̕�
#define SCREEN_HEIGHT	(1080)			// �E�C���h�E�̍���
#define SCREEN_CENTER_X	(SCREEN_WIDTH / 2)	// �E�C���h�E�̒��S�w���W
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT / 2)	// �E�C���h�E�̒��S�x���W

#define	MAP_W			(1280.0f)
#define	MAP_H			(1280.0f)
#define	MAP_TOP			(MAP_H/2)
#define	MAP_DOWN		(-MAP_H/2)
#define	MAP_LEFT		(-MAP_W/2)
#define	MAP_RIGHT		(MAP_W/2)

#define	FPS				(60)			// ���zFPS

// ��ԗp�̃f�[�^�\���̂��`
struct INTERPOLATION_DATA
{
	XMFLOAT3	pos;		// ���_���W
	float		frame;		// ���s�t���[���� ( dt = 1.0f/frame )
	BOOL		isTerminal;	// �I�_�t���O
};

enum
{
	MODE_TITLE = 0,				// �^�C�g�����
	MODE_TUTORIAL,				// �Q�[���������
	MODE_GAME,					// �Q�[�����
	MODE_GAME_STOP,				// �X�g�b�v����
	MODE_GAME_SLOW,				// �X���[����
	MODE_RESULT,				// ���U���g���
	MODE_GAMEOVER,				// GAMEOVER���
	MODE_MAX
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
long GetMousePosX(void);
long GetMousePosY(void);
char* GetDebugStr(void);

void SetMode(int mode);
void SetModeWithoutReload(int mode);

int GetMode(void);

void SetLoadGame(BOOL flg);

void EndGame(void);
void SetExit(BOOL mode);
void SetCutin(BOOL mode);
void SetPause(BOOL mode);