//=============================================================================
//
// PAUSE [pause.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "pause.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	TEXTURE_SIZE_W				512.0f			// キャラサイズ
#define	TEXTURE_SIZE_H				64.0f			//

// テクスチャ番号
enum {
	TEXTURE_BG,
	TEXTURE_RESUME,
	TEXTURE_RESTART,
	TEXTURE_BACK_TO_TITLE,
	TEXTURE_EXIT_GAME,
	TEXTURE_MANUAL,

	TEXTURE_MAX,
};

// ボタン番号
enum {
	RESUME,
	RESTART,
	BACK_TO_TITLE,
	EXIT_GAME,

	BUTTON_MAX,
};
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/whiteSprite.png",
	"data/TEXTURE/pauseButton00.png",
	"data/TEXTURE/pauseButton01.png",
	"data/TEXTURE/pauseButton02.png",
	"data/TEXTURE/pauseButton03.png",
	"data/TEXTURE/manual.png",
};

static BOOL		g_Load = FALSE;

static int		g_ButtonNow;			// 今カーソルの居るボタン

static XMFLOAT3 g_DefaultPos;			// ボタン基準座標
static XMFLOAT3 g_Pos[BUTTON_MAX];		// ボタン座標
static XMFLOAT3 g_PosDiff;				// ボタン座標の間隔
static float	g_Scale;				// アクティブボタンの拡大率

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPause(void)
{
	ID3D11Device* pDevice = GetDevice();

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
	float diff = 0.16f;		// 間隔
	g_DefaultPos = XMFLOAT3(SCREEN_WIDTH * 0.3f, SCREEN_HEIGHT * (diff * (1.5f)), 0.0f);	// 基本座標
	g_PosDiff = XMFLOAT3(0.0f, SCREEN_HEIGHT * diff, 0.0f);		// 間隔

	g_Pos[RESUME] = g_DefaultPos;
	for (int i = 0; i < BUTTON_MAX; i++) {

		if (i > RESUME) {
			XMVECTOR pos = XMLoadFloat3(&g_Pos[i - 1]);
			XMVECTOR diff = XMLoadFloat3(&g_PosDiff);

			XMStoreFloat3(&g_Pos[i], pos + diff);
		}
	}

	g_ButtonNow = 0;
	g_Scale = 1.2f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPause(void)
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
void UpdatePause(void)
{
	// ボタン入力
	if (GetKeyboardTrigger(DIK_DOWN)) {
		g_ButtonNow = (g_ButtonNow + 1) % BUTTON_MAX;
		// ボタン移動音
		PlaySound(SOUND_LABEL_SE_BUTTON_MOVE);
	}
	if (GetKeyboardTrigger(DIK_UP)) {
		g_ButtonNow = (g_ButtonNow + BUTTON_MAX - 1) % BUTTON_MAX;
		// ボタン移動音
		PlaySound(SOUND_LABEL_SE_BUTTON_MOVE);
	}
	if (GetKeyboardTrigger(DIK_ESCAPE)) {
		// PAUSEを終了
		SetPause(FALSE);
		// 決定音
		PlaySound(SOUND_LABEL_SE_CANCEL);
		// リフレッシュ
		return;
	}

	// 画面遷移
	if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_SPACE)) {
		switch (g_ButtonNow) {
		case RESUME:
			// PAUSEを終了
			SetPause(FALSE);
			// 決定音
			PlaySound(SOUND_LABEL_SE_CANCEL);
			// 多重トリガー防止
			return;
			break;

		case RESTART:
			// PAUSEを終了
			SetPause(FALSE);
			// GAME MODEを再開
			SetFade(FADE_OUT, MODE_GAME);
			// 決定音
			PlaySound(SOUND_LABEL_SE_SELECT000);
			// 多重トリガー防止
			return;
			break;

		case BACK_TO_TITLE:
			// PAUSEを終了
			SetPause(FALSE);
			// タイトルへ
			SetFade(FADE_OUT, MODE_TITLE);
			// 決定音
			PlaySound(SOUND_LABEL_SE_SELECT001);
			// 多重トリガー防止
			return;
			break;

		case EXIT_GAME:
			// EXIT画面をセット
			SetExit(TRUE);
			// 決定音
			PlaySound(SOUND_LABEL_SE_SELECT001);
			// 多重トリガー防止
			return;
			break;

		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPause(void)
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
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BG]);

		SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f));		// 半透明スプライトをセット

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// BUTTON
	for (int i = 0; i < BUTTON_MAX; i++) {

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i + 1]);

		float px = g_Pos[i].x;
		float py = g_Pos[i].y;

		float pw, ph;
		// アクティブボタン？
		if (i == g_ButtonNow) {
			pw = TEXTURE_SIZE_W * g_Scale;
			ph = TEXTURE_SIZE_H * g_Scale;
		}
		// それ以外
		else {
			pw = TEXTURE_SIZE_W;
			ph = TEXTURE_SIZE_H;
		}

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer,
			px, py, pw, ph,
			0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// MANUAL
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MANUAL]);

		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);
		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}