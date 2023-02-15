//=============================================================================
//
// UI処理 [UI.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "UI.h"
#include "player.h"
#include "enemy.h"
#include "hpBar.h"
#include "score.h"
#include "icon.h"
#include "progress.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define		TEXTURE_WIDTH				SCREEN_WIDTH	// キャラサイズ
#define		TEXTURE_HEIGHT				256				// 
#define		TEXTURE_MAX					1				// テクスチャの数

#define		ALPHA_NORMAL				1.0f			// 通常時α
#define		ALPHA_TRANSPARENT			0.3f			// 透過時α

#define		ANIM_LENGTH					20				// 透過アニメーションの長さ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

// テクスチャファイル
static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/UI_bar.png",
};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static UI		g_UI;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitUI(void)
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


	// UIの初期化
	g_UI.isActive = TRUE;
	g_UI.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);		// 左上から表示
	g_UI.w = TEXTURE_WIDTH;
	g_UI.h = TEXTURE_HEIGHT;

	g_UI.isInside = FALSE;
	g_UI.isEntering = FALSE;
	g_UI.isExiting = FALSE;

	g_UI.alpha = ALPHA_NORMAL;
	g_UI.extraRange = -64.0f;						// 追加透過判定範囲Y

	g_UI.animFrameCnt = 0;
	
	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// 終了処理
//=============================================================================
void UninitUI(void)
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
void UpdateUI(void) {

	g_UI.isInside = IsPlayerInsideUI() || IsEnemyInsideUI();

	if (g_UI.isInside == TRUE) {
		g_UI.alpha = ALPHA_TRANSPARENT;
	}
	else {
		g_UI.alpha = ALPHA_NORMAL;
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawUI(void) {

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

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	// UIは背景を追従しない

	// 位置やテクスチャ座標を反映
	float px = g_UI.pos.x;
	float py = g_UI.pos.y;
	float pw = g_UI.w;
	float ph = g_UI.h;
	
	float tw = 1.0f;
	float th = 1.0f;
	float tx = 0.0f;
	float ty = 0.0f;

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, g_UI.alpha));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);


	DrawHPBar(g_UI.alpha);			// HPバーを描画
	DrawScore(g_UI.alpha);			// SCOREを描画
	DrawIcon(g_UI.alpha);			// ICONを描画
	DrawProgress(g_UI.alpha);		// PROGRESSを描画
}

//=============================================================================
// UIを取得
//=============================================================================
UI* GetUI(void) {
	return &g_UI;
}