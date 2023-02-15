//=============================================================================
//
// ICON [icon.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "icon.h"
#include "UI.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define		TEXTURE_WIDTH				125					// キャラサイズ
#define		TEXTURE_HEIGHT				125					// 
#define		TEXTURE_MAX					PLAYER_LEVEL_MAX

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
	"data/TEXTURE/icon00.png",
	"data/TEXTURE/icon01.png",
	"data/TEXTURE/icon02.png",
	"data/TEXTURE/icon03.png",
	"data/TEXTURE/icon04.png",
};

static	BOOL		g_Load = FALSE;				// 初期化を行ったかのフラグ
static	ICON		g_Icon;

static	int			g_Level;					// プレイヤーレベル

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitIcon(void)
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


	// 初期化
	g_Icon.isActive = TRUE;
	g_Icon.w = TEXTURE_WIDTH;
	g_Icon.h = TEXTURE_HEIGHT;
	g_Icon.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// 終了処理
//=============================================================================
void UninitIcon(void)
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
void UpdateIcon(void) {

	// 無効なら実行しない
	if (g_Icon.isActive == FALSE) {
		return;
	}

	PLAYER* player = GetPlayer();
	g_Level = player[0].level;

}

//=============================================================================
// 描画処理
// DrawUI()で呼び出す
//=============================================================================
void DrawIcon(float alpha) {

	// 無効なら実行しない
	if (g_Icon.isActive == FALSE) {
		return;
	}

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

	UI* UI = GetUI();
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Level]);

	float px = g_Icon.pos.x - UI->pos.x;		// UIを追従
	float py = g_Icon.pos.y - UI->pos.y;		//
	float pw = g_Icon.w;
	float ph = g_Icon.h;

	float tw = 1.0f;
	float th = 1.0f;
	float tx = 0.0f;
	float ty = 0.0f;

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

}