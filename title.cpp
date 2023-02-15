//=============================================================================
//
// タイトル [title.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX					TITLE_MAX		// テクスチャの数

#define	BUTTON_WIDTH				512.0f			// デフォルトボタンサイズ
#define	BUTTON_HEIGHT				64.0f			//
#define	BUTTON_SCALE				1.28f			// アクティブボタンの倍率

#define	PERIOD						128				// 点滅する周期

// テクスチャ番号
enum {
	BG,
	GLOW,
	LOGO,
	BUTTON_START_GAME,
	BUTTON_CREDITS,
	BUTTON_EXIT,
	CREDITS_BG,
	CREDITS,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title00.png",
	"data/TEXTURE/title01.png",
	"data/TEXTURE/title02.png",
	"data/TEXTURE/titleButton00.png",
	"data/TEXTURE/titleButton01.png",
	"data/TEXTURE/titleButton02.png",
	"data/TEXTURE/whiteSprite.png",
	"data/TEXTURE/credits.png",
};

static BOOL		g_Load = FALSE;

static int		g_ButtonNow;			// 今カーソルの居るボタン

static XMFLOAT3 g_DefaultButtonPos;					// ボタン基準座標
static XMFLOAT3 g_ButtonPosDiff;					// ボタン座標の間隔
static XMFLOAT3 g_ButtonPos[TITLE_BUTTON_MAX];		// ボタン座標
static float	g_ButtonW[TITLE_BUTTON_MAX];		// ボタンのサイズ
static float	g_ButtonH[TITLE_BUTTON_MAX];		// 


static BOOL		g_IsCreditsActive;	// CREDITS画面フラグ

static float	g_CreditsW = 512;	// CREDITSテクスチャサイズ
static float	g_CreditsH = 1800;	//

static float	g_CreditsWindowW;	// CREDITS枠サイズ
static float	g_CreditsWindowH;	//
static float	g_CreditsWindowY;	// CREDITS表示枠の座標(0～g_CreditsH-g_CreditsWindowH)

static float	g_CreditsScrl;		// CREDITSスクロール量


static int						g_Cnt;
static float					g_Alpha;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// ボタン関連の初期化
	g_DefaultButtonPos = XMFLOAT3(1445.0f, 530.0f, 0.0f);
	g_ButtonPosDiff = XMFLOAT3(0.0f, 150.0f, 0.0f);
	g_ButtonPos[TITLE_START_GAME] = g_DefaultButtonPos;
	for (int i = 0; i < TITLE_BUTTON_MAX; i++) {

		if (i > TITLE_START_GAME) {
			XMVECTOR pos = XMLoadFloat3(&g_ButtonPos[i - 1]);
			XMVECTOR diff = XMLoadFloat3(&g_ButtonPosDiff);

			XMStoreFloat3(&g_ButtonPos[i], pos + diff);
		}

		g_ButtonW[i] = BUTTON_WIDTH;
		g_ButtonH[i] = BUTTON_HEIGHT;
	}


	g_ButtonNow = TITLE_START_GAME;

	g_IsCreditsActive = FALSE;
	g_CreditsWindowW = g_CreditsW;	// 枠のサイズ
	g_CreditsWindowH = 800.0f;		//
	g_CreditsWindowY = 0.0f;		// テクスチャ枠のY座標
	g_CreditsScrl = 0.01f;			// スクロール量(0～1)



	g_Cnt = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitle(void)
{
	//////////////////////////////////////////////////////////////////////////////
	// カウンタでα制御
	//////////////////////////////////////////////////////////////////////////////
	{
		g_Cnt = (g_Cnt + 1) % PERIOD;

		float ratio = (float)g_Cnt / (float)PERIOD;

		// グラフをかける( y=1-(2x-1)^2 )
		ratio = 1 -
			(2 * ratio - 1) * (2 * ratio - 1);

		g_Alpha = ratio;
	}

	//////////////////////////////////////////////////////////////////////////////
	// CREDITS処理
	//////////////////////////////////////////////////////////////////////////////
	if (g_IsCreditsActive == TRUE) {

		// スクロール処理
		if (GetKeyboardPress(DIK_DOWN))
		{
			g_CreditsWindowY += g_CreditsScrl;
		}
		else if (GetKeyboardPress(DIK_UP))
		{
			g_CreditsWindowY -= g_CreditsScrl;
		}

		// 画面外処理
		if (g_CreditsWindowY < 0.0f) {	// 最上部までスクロールした？
			g_CreditsWindowY = 0.0f;
		}

		if (g_CreditsWindowY > 1.0f - g_CreditsWindowH / g_CreditsH) {	// 最下部までスクロールした？
			g_CreditsWindowY = 1.0f - g_CreditsWindowH / g_CreditsH;
		}

		// 消す
		if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_SPACE) || GetKeyboardTrigger(DIK_ESCAPE)) {
			// 決定音
			PlaySound(SOUND_LABEL_SE_CANCEL);
			// CREDITSを消す
			g_IsCreditsActive = FALSE;
		}
		return;		// 終了してキー入力をリフレッシュ
	}

	//////////////////////////////////////////////////////////////////////////////
	// ボタン入力処理
	//////////////////////////////////////////////////////////////////////////////
	if (GetKeyboardTrigger(DIK_DOWN)) {
		g_ButtonNow = (g_ButtonNow + 1) % TITLE_BUTTON_MAX;
		// ボタン移動音
		PlaySound(SOUND_LABEL_SE_BUTTON_MOVE);
	}
	if (GetKeyboardTrigger(DIK_UP)) {
		g_ButtonNow = (g_ButtonNow + TITLE_BUTTON_MAX - 1) % TITLE_BUTTON_MAX;
		// ボタン移動音
		PlaySound(SOUND_LABEL_SE_BUTTON_MOVE);
	}
	if (GetKeyboardTrigger(DIK_ESCAPE)) {
		// EXIT画面を表示
		SetExit(TRUE);
		// 決定音
		PlaySound(SOUND_LABEL_SE_SELECT001);
		// リフレッシュ
		return;
	}

	//////////////////////////////////////////////////////////////////////////////
	// 画面遷移
	//////////////////////////////////////////////////////////////////////////////
	if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_SPACE)) {
		switch (g_ButtonNow) {

		case TITLE_START_GAME:
			SetFade(FADE_OUT, MODE_GAME);
			// 決定音
			PlaySound(SOUND_LABEL_SE_SELECT000);
			// 多重トリガー防止
			return;
			break;

		case TITLE_CREDITS:
			// CREDITSを表示
			g_IsCreditsActive = TRUE;
			// 決定音
			PlaySound(SOUND_LABEL_SE_SELECT001);
			// 多重トリガー防止
			return;
			break;

		case TITLE_EXIT:
			// EXIT画面を表示
			SetExit(TRUE);
			// 決定音
			PlaySound(SOUND_LABEL_SE_SELECT001);
			// 多重トリガー防止
			return;
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	// ボタン処理
	//////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < TITLE_BUTTON_MAX; i++) {
		g_ButtonW[i] = BUTTON_WIDTH;
		g_ButtonH[i] = BUTTON_HEIGHT;
	}

	g_ButtonW[g_ButtonNow] = BUTTON_WIDTH * BUTTON_SCALE;
	g_ButtonH[g_ButtonNow] = BUTTON_HEIGHT * BUTTON_SCALE;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// BG
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BG]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// GLOW
	{
		SetBlendState(BLEND_MODE_ADD);
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GLOW]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_Alpha));

		// 2回描画
		for (int i = 0; i < 1; i++) {
			GetDeviceContext()->Draw(4, 0);
		}

		SetBlendState(BLEND_MODE_ALPHABLEND);
	}

	// LOGO
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[LOGO]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// BUTTON
	for (int i = 0; i < TITLE_BUTTON_MAX; i++) {

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i + BUTTON_START_GAME]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer,
			g_ButtonPos[i].x, g_ButtonPos[i].y, g_ButtonW[i], g_ButtonH[i],
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// CREDITS
	if (g_IsCreditsActive == TRUE) {

		// 背景を設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[CREDITS_BG]);
		SetSpriteLTColor(g_VertexBuffer,
			0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f));		// 真っ黒半透明
		GetDeviceContext()->Draw(4, 0);



		// 枠とCREDITSを表示
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[CREDITS]);

		float px = SCREEN_WIDTH * 0.5f;
		float py = SCREEN_HEIGHT * 0.5f;
		float pw = g_CreditsWindowW;
		float ph = g_CreditsWindowH;

		float tx = 0.0f;
		float ty = g_CreditsWindowY;
		float tw = 1.0f;
		float th = g_CreditsWindowH / g_CreditsH;

		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		GetDeviceContext()->Draw(4, 0);

	}

}





