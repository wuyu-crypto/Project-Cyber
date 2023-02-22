//=============================================================================
//
// ���C������ [main.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "debugproc.h"
#include "input.h"

#include "title.h"
#include "bg.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "score.h"
#include "result.h"
#include "sound.h"
#include "fade.h"
#include "UI.h"
#include "hpBar.h"
#include "item.h"
#include "slow.h"
#include "icon.h"
#include "progress.h"
#include "bossHp.h"
#include "exit.h"
#include "gameover.h"
#include "cutin.h"
#include "pause.h"
#include "damage.h"

#include "waveController.h"

#include "file.h"

#include "effect.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// �E�C���h�E�̃N���X��
#define WINDOW_NAME		"Cyber Glider"		// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);


//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_NAME;		// �f�o�b�O�����\���p

#endif

int	g_Mode = MODE_TITLE;					// �N�����̉�ʂ�ݒ�

BOOL g_LoadGame = FALSE;					// NewGame

BOOL g_IsExit;			// EXIT��ʃt���O
BOOL g_IsCutin;			// CUTIN��ʃt���O
BOOL g_IsPause;			// PAUSE��ʃt���O
BOOL g_IsGameEnd;		// �Q�[���I���t���O


//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// ���������[�N
	//new int;
	//return 0;

#endif // _DEBUG


	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// �E�B���h�E�̍����W
		CW_USEDEFAULT,																		// �E�B���h�E�̏���W
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// �E�B���h�E����
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
		NULL,
		NULL,
		hInstance,
		NULL);

	// �E�B���h�E���[�h���t���X�N���[�����[�h���̏���
	BOOL mode = TRUE;

	int id = MessageBox(NULL, "�t���X�N���[���ŋN�����܂����H", "�N�����[�h", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (id)
	{
	case IDYES:
		mode = FALSE;	// �t���X�N��
		break;
	case IDNO:
		mode = TRUE;	// Window��			// ���ɂ���ē����Ȃ���������
		break;
	case IDCANCEL:		// �I��
	default:
		return -1;
		break;
	}

	// ����������(�E�B���h�E���쐬���Ă���s��)
	if(FAILED(Init(hInstance, hWnd, mode)))
	{
		return -1;
	}

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	// ���b�Z�[�W���[�v
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��Ƒ��o
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / FPS))	// 1/FPS�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// �X�V����
				Draw();				// �`�揈��

#ifdef _DEBUG	// �f�o�b�O�ł̎������\������
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}

		if (g_IsGameEnd == TRUE) {		// �Q�[���I��
			break;
		}
	}

	timeEndPeriod(1);				// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			//DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// �`��̏�����
	InitRenderer(hInstance, hWnd, bWindow);

	// �J�����̏�����
	InitCamera();

	// ���C�g��L����
	SetLightEnable(FALSE);

	// �w�ʃ|���S�����J�����O
	SetCullingMode(CULL_MODE_BACK);

	// ���͏����̏�����
	InitInput(hInstance, hWnd);

	// �T�E���h�����̏�����
	InitSound(hWnd);

	// �t�F�[�h�����̏�����
	InitFade();

	// EXIT��ʂ̏�����
	InitExit();

	// �ŏ��̃��[�h���Z�b�g
	SetMode(g_Mode);	// ������SetMode�̂܂܂ŁI

	g_IsExit = FALSE;
	g_IsCutin = FALSE;
	g_IsPause = FALSE;
	g_IsGameEnd = FALSE;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	// �I���̃��[�h���Z�b�g
	SetMode(MODE_MAX);

	// �t�F�[�h�̏I������
	UninitFade();

	// �T�E���h�̏I������
	UninitSound();

	// ���͂̏I������
	UninitInput();

	// �J�����̏I������
	UninitCamera();

	// �����_���[�̏I������
	UninitRenderer();

	// EXIT��ʂ̏I������
	UninitExit();
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	// ���͂̍X�V����
	UpdateInput();

	// �J�����X�V
	UpdateCamera();

	if (g_IsExit == TRUE) {	// EXIT��ʂ̍X�V
		UpdateExit();
	}
	else if (g_IsPause == TRUE) {	// PAUSE��ʂ̍X�V
		UpdatePause();
	}
	else if (g_IsCutin == TRUE) {	// CUTIN��ʂ̍X�V
		UpdateCutin();
	}
	else {
		// ���[�h�ɂ���ď����𕪂���
		switch (g_Mode)
		{
		case MODE_TITLE:		// �^�C�g����ʂ̍X�V
			UpdateTitle();
			UpdateEffect();
			break;

		case MODE_GAME_SLOW:	// �X���[����
			UpdateSlow();
			if (IsSlowApplied() == TRUE) {			// �X���[�����s�H
				break;								// �Q�[�����[�v�I��
			}

		case MODE_GAME:			// �Q�[����ʂ̍X�V
			UpdateWaveController();
			UpdateBG();
			UpdatePlayer();
			UpdateEnemy();
			UpdateBullet();
			UpdateItem();
			UpdateUI();
			UpdateHPBar();
			UpdateScore();
			UpdateIcon();
			UpdateProgress();
			UpdateBossHp();
			UpdateEffect();
			UpdateDamage();
			break;

		case MODE_RESULT:		// ���U���g��ʂ̍X�V
			UpdateResult();
			UpdateEffect();
			break;

		case MODE_GAMEOVER:		// GAMEOVER��ʂ̍X�V
			UpdateGameover();
			UpdateEffect();
			break;
		}
	}

	UpdateFade();			// �t�F�[�h�̍X�V����
}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	// �o�b�N�o�b�t�@�N���A
	Clear();

	SetCamera();

	// 2D�̕���`�悷�鏈��
	SetViewPort(TYPE_FULL_SCREEN);

	// Z��r�Ȃ�
	SetDepthEnable(FALSE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);


	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:					// �^�C�g����ʂ̕`��
		DrawTitle();
		break;

	case MODE_GAME:						// �Q�[����ʂ̕`��
	case MODE_GAME_SLOW:				// �X���[����
		DrawBG();
		DrawEnemy();
		DrawItem();
		DrawEffect(EFFECT_LAYER_BEHIND_PLAYER);
		DrawPlayer();
		DrawBullet();
		DrawBossHp();
		DrawDamage();
		DrawUI();
		break;

	case MODE_RESULT:					// ���U���g��ʂ̕`��
		DrawResult();
		break;

	case MODE_GAMEOVER:		// GAMEOVER��ʂ̕`��
		DrawGameover();
		break;
	}

	DrawEffect(EFFECT_LAYER_FRONT_OF_PLAYER);		// �G�t�F�N�g�̕`��

	if (g_IsCutin == TRUE) {	// CUTIN��ʂ̕`��
		DrawCutin();
	}
	if (g_IsPause == TRUE) {	// PAUSE��ʂ̕`��
		DrawPause();
	}
	if (g_IsExit == TRUE) {	// EXIT��ʂ̕`��
		DrawExit();
	}

	DrawFade();				// �t�F�[�h��ʂ̕`��


#ifdef _DEBUG
	// �f�o�b�O�\��
	DrawDebugProc();
#endif

	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
	Present();
}


long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif



//=============================================================================
// ���[�h�̐ݒ�
//=============================================================================
void SetMode(int mode)
{
	// ���[�h��ς���O�ɑS������������������Ⴄ
	StopSound();			// �܂��Ȃ��~�߂�

	UninitTitle();

	UninitWaveController();
	UninitBG();
	UninitHPBar();
	UninitUI();
	UninitPlayer();
	UninitEnemy();
	UninitBullet();
	UninitItem();
	UninitScore();
	UninitSlow();
	UninitIcon();
	UninitProgress();
	UninitBossHp();
	UninitCutin();
	UninitPause();
	UninitDamage();

	UninitResult();
	UninitGameover();

	UninitEffect();


	g_Mode = mode;	// ���̃��[�h���Z�b�g

	switch (g_Mode)
	{
	case MODE_TITLE:
		// �^�C�g����ʂ̏�����
		InitTitle();
		InitEffect();
		PlaySound(SOUND_LABEL_BGM_TITLE);
		break;

	case MODE_GAME:
		// �Q�[����ʂ̏�����
		InitWaveController();
		InitBG();
		InitUI();
		InitPlayer();
		InitEnemy();
		InitBullet();
		InitItem();
		InitHPBar();
		InitScore();
		InitSlow();
		InitIcon();
		InitProgress();
		InitBossHp();
		InitCutin();
		InitPause();
		InitEffect();
		InitDamage();

		PlaySound(SOUND_LABEL_BGM_GAME);
		break;

	case MODE_RESULT:
		InitEffect();
		// RECORD��ǂݍ���
		LoadData();
		InitResult();
		break;

	case MODE_GAMEOVER:
		InitEffect();
		InitGameover();
		// �Q�[���I�[�o�[�T�E���h

		break;
	}
}

//=============================================================================
// �����[�h�𔺂�Ȃ����[�h�̐ݒ�
//=============================================================================
void SetModeWithoutReload(int mode) {
	g_Mode = mode;
}

//=============================================================================
// ���[�h�̎擾
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}

//=============================================================================
// �j���[�Q�[�������[�h�Q�[�������Z�b�g����
//=============================================================================
void SetLoadGame(BOOL flg)
{
	g_LoadGame = flg;
}

//=============================================================================
// �Q�[���I���t���O���Z�b�g
//=============================================================================
void EndGame(void) {
	g_IsGameEnd = TRUE;
}

//=============================================================================
// EXIT��ʂ��Z�b�g
//=============================================================================
void SetExit(BOOL mode) {
	g_IsExit = mode;
}

//=============================================================================
// CUTIN��ʂ��Z�b�g
//=============================================================================
void SetCutin(BOOL mode) {
	g_IsCutin = mode;
}

//=============================================================================
// PAUSE��ʂ��Z�b�g
//=============================================================================
void SetPause(BOOL mode) {
	g_IsPause = mode;
}