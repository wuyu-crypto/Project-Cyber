//=============================================================================
//
// メイン処理 [main.cpp]
// Author : GP11B132 33 呉優
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
#include "stop.h"
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
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// ウインドウのクラス名
#define WINDOW_NAME		"Cyber Glider"		// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);


//*****************************************************************************
// グローバル変数:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用

#endif

int	g_Mode = MODE_TITLE;					// 起動時の画面を設定

BOOL g_LoadGame = FALSE;					// NewGame

BOOL g_StopEffect = FALSE;

BOOL g_IsExit;			// EXIT画面フラグ
BOOL g_IsCutin;			// CUTIN画面フラグ
BOOL g_IsPause;			// PAUSE画面フラグ
BOOL g_IsGameEnd;		// ゲーム終了フラグ


//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// メモリリーク
	//new int;
	//return 0;

#endif // _DEBUG


	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
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
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// ウィンドウの左座標
		CW_USEDEFAULT,																		// ウィンドウの上座標
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// ウィンドウ横幅
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
		NULL,
		NULL,
		hInstance,
		NULL);

	// ウィンドウモードかフルスクリーンモードかの処理
	BOOL mode = TRUE;

	int id = MessageBox(NULL, "フルスクリーンで起動しますか？", "起動モード", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (id)
	{
	case IDYES:
		mode = FALSE;	// フルスクで
		break;
	case IDNO:
		mode = TRUE;	// Windowで			// 環境によって動かない事がある
		break;
	case IDCANCEL:		// 終了
	default:
		return -1;
		break;
	}

	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, mode)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	// メッセージループ
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / FPS))	// 1/FPS秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// 更新処理
				Draw();				// 描画処理

#ifdef _DEBUG	// デバッグ版の時だけ表示する
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}

		if (g_IsGameEnd == TRUE) {		// ゲーム終了
			break;
		}
	}

	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
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
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// 描画の初期化
	InitRenderer(hInstance, hWnd, bWindow);

	// カメラの初期化
	InitCamera();

	// ライトを有効化
	SetLightEnable(FALSE);

	// 背面ポリゴンをカリング
	SetCullingMode(CULL_MODE_BACK);

	// 入力処理の初期化
	InitInput(hInstance, hWnd);

	// サウンド処理の初期化
	InitSound(hWnd);

	// フェード処理の初期化
	InitFade();

	// EXIT画面の初期化
	InitExit();

	// 最初のモードをセット
	SetMode(g_Mode);	// ここはSetModeのままで！

	g_IsExit = FALSE;
	g_IsCutin = FALSE;
	g_IsPause = FALSE;
	g_IsGameEnd = FALSE;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	// 終了のモードをセット
	SetMode(MODE_MAX);

	// フェードの終了処理
	UninitFade();

	// サウンドの終了処理
	UninitSound();

	// 入力の終了処理
	UninitInput();

	// カメラの終了処理
	UninitCamera();

	// レンダラーの終了処理
	UninitRenderer();

	// EXIT画面の終了処理
	UninitExit();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// 入力の更新処理
	UpdateInput();

	// カメラ更新
	UpdateCamera();

	if (g_IsExit == TRUE) {	// EXIT画面の更新
		UpdateExit();
	}
	else if (g_IsPause == TRUE) {	// PAUSE画面の更新
		UpdatePause();
	}
	else if (g_IsCutin == TRUE) {	// CUTIN画面の更新
		UpdateCutin();
	}
	else {
		// モードによって処理を分ける
		switch (g_Mode)
		{
		case MODE_TITLE:		// タイトル画面の更新
			UpdateTitle();
			UpdateEffect();
			break;

		case MODE_GAME_SLOW:	// スロー処理
			UpdateSlow();
			if (IsSlowApplied() == TRUE) {			// スローを実行？
				break;								// ゲームループ終了
			}

		case MODE_GAME:			// ゲーム画面の更新
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

		case MODE_GAME_STOP:		// ストップエフェクト
			UpdateStop();
			break;

		case MODE_RESULT:		// リザルト画面の更新
			UpdateResult();
			UpdateEffect();
			break;

		case MODE_GAMEOVER:		// GAMEOVER画面の更新
			UpdateGameover();
			UpdateEffect();
			break;
		}
	}

	UpdateFade();			// フェードの更新処理
}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファクリア
	Clear();

	SetCamera();

	// 2Dの物を描画する処理
	SetViewPort(TYPE_FULL_SCREEN);

	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);


	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE:					// タイトル画面の描画
		DrawTitle();
		break;

	case MODE_GAME:						// ゲーム画面の描画
	case MODE_GAME_STOP:				// ストップエフェクト
	case MODE_GAME_SLOW:				// スロー処理
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

	case MODE_RESULT:					// リザルト画面の描画
		DrawResult();
		break;

	case MODE_GAMEOVER:		// GAMEOVER画面の描画
		DrawGameover();
		break;
	}

	DrawEffect(EFFECT_LAYER_FRONT_OF_PLAYER);		// エフェクトの描画

	if (g_IsCutin == TRUE) {	// CUTIN画面の描画
		DrawCutin();
	}
	if (g_IsPause == TRUE) {	// PAUSE画面の描画
		DrawPause();
	}
	if (g_IsExit == TRUE) {	// EXIT画面の描画
		DrawExit();
	}

	DrawFade();				// フェード画面の描画


#ifdef _DEBUG
	// デバッグ表示
	DrawDebugProc();
#endif

	// バックバッファ、フロントバッファ入れ替え
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
// モードの設定
//=============================================================================
void SetMode(int mode)
{
	// モードを変える前に全部メモリを解放しちゃう
	StopSound();			// まず曲を止める

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
	UninitStop();
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


	g_Mode = mode;	// 次のモードをセット

	switch (g_Mode)
	{
	case MODE_TITLE:
		// タイトル画面の初期化
		InitTitle();
		InitEffect();
		PlaySound(SOUND_LABEL_BGM_TITLE);
		break;

	case MODE_GAME:
		// ゲーム画面の初期化
		InitWaveController();
		InitBG();
		InitUI();
		InitPlayer();
		InitEnemy();
		InitBullet();
		InitItem();
		InitHPBar();
		InitScore();
		InitStop();
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
		// RECORDを読み込む
		LoadData();
		InitResult();
		break;

	case MODE_GAMEOVER:
		InitEffect();
		InitGameover();
		// ゲームオーバーサウンド

		break;
	}
}

//=============================================================================
// リロードを伴わないモードの設定
//=============================================================================
void SetModeWithoutReload(int mode) {
	g_Mode = mode;
}

//=============================================================================
// モードの取得
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}

//=============================================================================
// ニューゲームかロードゲームかをセットする
//=============================================================================
void SetLoadGame(BOOL flg)
{
	g_LoadGame = flg;
}

//=============================================================================
// ゲーム終了フラグをセット
//=============================================================================
void EndGame(void) {
	g_IsGameEnd = TRUE;
}

//=============================================================================
// EXIT画面をセット
//=============================================================================
void SetExit(BOOL mode) {
	g_IsExit = mode;
}

//=============================================================================
// CUTIN画面をセット
//=============================================================================
void SetCutin(BOOL mode) {
	g_IsCutin = mode;
}

//=============================================================================
// PAUSE画面をセット
//=============================================================================
void SetPause(BOOL mode) {
	g_IsPause = mode;
}