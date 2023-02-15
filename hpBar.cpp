//=============================================================================
//
// HPバー処理 [hpBar.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "hpBar.h"
#include "UI.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define		TEXTURE_WIDTH				16				// キャラサイズ
#define		TEXTURE_HEIGHT				80				// 
#define		TEXTURE_MAX					2				// テクスチャの数

#define		HP_BAR_X					128				// HPバー基準座標
#define		HP_BAR_Y					32				//
#define		SLOT_INTERVAL				24				// HPスロットの間隔

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
	"data/TEXTURE/HP00.png",
	"data/TEXTURE/HP01.png",
};

static BOOL		g_Load = FALSE;				// 初期化を行ったかのフラグ
static HP_BAR	g_HPBar[PLAYER_HP_MAX];

static BOOL		g_HPBarIsActive;			// HPバー有効化フラグ

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitHPBar(void)
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


	// HPバーの初期化
	g_HPBarIsActive = TRUE;

	XMFLOAT3 pos = XMFLOAT3(HP_BAR_X, HP_BAR_Y, 0.0f);
	for (int i = 0; i < PLAYER_HP_MAX; i++) {

		g_HPBar[i].pos = pos;
		g_HPBar[i].w = TEXTURE_WIDTH;
		g_HPBar[i].h = TEXTURE_HEIGHT;
		g_HPBar[i].isRemain = FALSE;

		pos = XMFLOAT3(pos.x + SLOT_INTERVAL, pos.y, pos.z);

	}

	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// 終了処理
//=============================================================================
void UninitHPBar(void)
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
void UpdateHPBar(void) {

	// 無効なら実行しない
	if (g_HPBarIsActive == FALSE) {
		return;
	}

	PLAYER* player = GetPlayer();
	int hp = player[0].hp;

	// オーバーフロー防止
	if (hp < 0) {
		hp = 0;			
	}

	// 残りHPの数だけ赤いバーをセット
	for (int i = 0; i < hp; i++) {
		g_HPBar[i].isRemain = TRUE;
	}

	// 失ったHPの数だけ灰色のバーをセット
	for (int i = 0; i < PLAYER_HP_MAX - hp; i++) {
		g_HPBar[i + hp].isRemain = FALSE;
	}

}

//=============================================================================
// 描画処理
// DrawUI()で呼び出す
//=============================================================================
void DrawHPBar(float alpha) {

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
	for (int i = 0; i < PLAYER_HP_MAX; i++) {

		// 赤色か灰色テクスチャを設定
		if (g_HPBar[i].isRemain == TRUE) {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
		}
		else {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);
		}


		// 位置やテクスチャ座標を反映
		float px = g_HPBar[i].pos.x - UI->pos.x;		// UIを追従
		float py = g_HPBar[i].pos.y - UI->pos.y;		//
		float pw = g_HPBar[i].w;
		float ph = g_HPBar[i].h;

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

}