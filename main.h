//=============================================================================
//
// メイン処理 [main.h]
// Author : GP11B132 33 呉優
//
//=============================================================================
#pragma once


#pragma warning(push)
#pragma warning(disable:4005)

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC				// メモリリーク
#endif // _DEBUG

#define _CRT_SECURE_NO_WARNINGS			// scanf のwarning防止
#include <stdio.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <math.h>

#include <DirectXMath.h>

#ifdef _DEBUG							// メモリリーク
#include <crtdbg.h>
#define new  ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


// 本来はヘッダに書かない方が良い
using namespace DirectX;


#define DIRECTINPUT_VERSION 0x0800		// 警告対処
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)




#define	BEZIER	// ベジェ弾道
#undef BEZIER



//*****************************************************************************
// ライブラリのリンク
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
// マクロ定義
//*****************************************************************************
#define SCREEN_WIDTH	(1920)			// ウインドウの幅
#define SCREEN_HEIGHT	(1080)			// ウインドウの高さ
#define SCREEN_CENTER_X	(SCREEN_WIDTH / 2)	// ウインドウの中心Ｘ座標
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT / 2)	// ウインドウの中心Ｙ座標

#define	MAP_W			(1280.0f)
#define	MAP_H			(1280.0f)
#define	MAP_TOP			(MAP_H/2)
#define	MAP_DOWN		(-MAP_H/2)
#define	MAP_LEFT		(-MAP_W/2)
#define	MAP_RIGHT		(MAP_W/2)

#define	FPS				(60)			// 理想FPS

// 補間用のデータ構造体を定義
struct INTERPOLATION_DATA
{
	XMFLOAT3	pos;		// 頂点座標
	float		frame;		// 実行フレーム数 ( dt = 1.0f/frame )
	BOOL		isTerminal;	// 終点フラグ
};

enum
{
	MODE_TITLE = 0,				// タイトル画面
	MODE_TUTORIAL,				// ゲーム説明画面
	MODE_GAME,					// ゲーム画面
	MODE_GAME_STOP,				// ストップ処理
	MODE_GAME_SLOW,				// スロー処理
	MODE_RESULT,				// リザルト画面
	MODE_GAMEOVER,				// GAMEOVER画面
	MODE_MAX
};

//*****************************************************************************
// プロトタイプ宣言
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