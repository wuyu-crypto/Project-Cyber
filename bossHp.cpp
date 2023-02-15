//=============================================================================
//
// ボスHP処理 [bossHp.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "bossHp.h"
#include "bg.h"
#include "enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define		TEXTURE_WIDTH				512.0f			// キャラサイズ
#define		TEXTURE_HEIGHT				32.0f			// 
#define		TEXTURE_MAX					3				// テクスチャの数

enum {
	HP_BG,
	HP_RED,
	HP_FRAME,

	HP_BAR_MAX,
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

// テクスチャファイル
static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bossHp00.png",
	"data/TEXTURE/bossHp01.png",
	"data/TEXTURE/bossHp02.png",

};

static BOOL		g_Load = FALSE;				// 初期化を行ったかのフラグ

static BOOL		g_IsActive;					// 有効化？

static XMFLOAT3 g_Pos;						// HPバーの座標
static XMFLOAT3	g_Diff;						// ボス座標からのズレ

static float	g_W;						// 赤いHPの幅

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBossHp(void)
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
	g_IsActive = FALSE;

	g_Diff = XMFLOAT3(-300.0f, -300.0f, 0.0f);			// ボス座標からのズレ

	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBossHp(void)
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
void UpdateBossHp(void) {

	// 無効化なら実行しない
	if (g_IsActive == FALSE) {
		return;
	}

	float HP;
	XMFLOAT3 pos;

	// ボスを検索
	ENEMY* enemy = GetEnemy();
	for (int i = 0; i < ENEMY_MAX; i++) {

		if (enemy[i].type == ENEMY_TYPE_BOSS) {
			// HPを取得
			HP = (float)enemy[i].hp;
			// 位置を取得
			pos = enemy[i].pos;
		}
	}

	// 比例を計算
	float ratio = HP / BOSS_HP_MAX;
	// 赤いHPの幅を伸縮
	g_W = ratio * TEXTURE_WIDTH;

	// 位置を同期
	XMVECTOR xmvPos = XMLoadFloat3(&pos);
	XMVECTOR xmvDiff = XMLoadFloat3(&g_Diff);
	XMStoreFloat3(&g_Pos, xmvPos + xmvDiff);

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBossHp(void) {

	if (g_IsActive == FALSE) {
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

	BG* bg = GetBG();

	//////////////////////////////////////////////////////////////////////////////
	// 背景を描画
	//////////////////////////////////////////////////////////////////////////////
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[HP_BG]);

		// 位置やテクスチャ座標を反映
		float px = g_Pos.x - bg->pos.x;		// UIを追従
		float py = g_Pos.y - bg->pos.y;		//
		float pw = TEXTURE_WIDTH;
		float ph = TEXTURE_HEIGHT;

		float tw = 1.0f;
		float th = 1.0f;
		float tx = 0.0f;
		float ty = 0.0f;

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	//////////////////////////////////////////////////////////////////////////////
	// 赤いHPを描画
	//////////////////////////////////////////////////////////////////////////////
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[HP_RED]);

		// 位置やテクスチャ座標を反映
		float px = g_Pos.x - bg->pos.x;		// BGを追従
		float py = g_Pos.y - bg->pos.y;		//
		float pw = g_W;
		float ph = TEXTURE_HEIGHT;

		float tw = 1.0f;
		float th = 1.0f;
		float tx = 0.0f;
		float ty = 0.0f;

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	//////////////////////////////////////////////////////////////////////////////
	// 枠を描画
	//////////////////////////////////////////////////////////////////////////////
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[HP_FRAME]);

		// 位置やテクスチャ座標を反映
		float px = g_Pos.x - bg->pos.x;		// BGを追従
		float py = g_Pos.y - bg->pos.y;		//
		float pw = TEXTURE_WIDTH;
		float ph = TEXTURE_HEIGHT;

		float tw = 1.0f;
		float th = 1.0f;
		float tx = 0.0f;
		float ty = 0.0f;

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}

//=============================================================================
// ボスHPを有効化
//=============================================================================
void SetBossHp(void) {
	g_IsActive = TRUE;
}

//=============================================================================
// ボスHPを無効化
//=============================================================================
void HideBossHp(void) {
	g_IsActive = FALSE;
}