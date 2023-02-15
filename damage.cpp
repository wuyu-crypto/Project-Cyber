//=============================================================================
//
// damage処理 [damage.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "damage.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define		TEXTURE_WIDTH				SCREEN_WIDTH	// キャラサイズ
#define		TEXTURE_HEIGHT				SCREEN_HEIGHT	// 
#define		TEXTURE_MAX					1				// テクスチャの数

#define		ANIM_LENGTH					20				// アニメーションの長さ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
enum {
	ANIM_START,
	ANIM_ENDING,

	ANIM_NUM,
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

// テクスチャファイル
static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/damage.png",
};

static BOOL		g_Load = FALSE;				// 初期化を行ったかのフラグ

static BOOL		g_IsActive;					// 実行フラグ
static XMFLOAT3	g_Pos;						// 座標
static float	g_W;						// サイズ
static float	g_H;
static float	g_Alpha;					// アルファ値

static int		g_AnimStep;					// アニメ段階
static float	g_FrameCnt;					// 補間用カウンタ

static float	g_AnimFrame[ANIM_NUM] = {	// アニメフレーム数
	120.0f,
	300.0f,
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitDamage(void)
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
	g_Pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_W = TEXTURE_WIDTH;
	g_H = TEXTURE_HEIGHT;
	g_Alpha = 0.0f;

	g_AnimStep = ANIM_START;
	g_FrameCnt = 0.0f;


	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// 終了処理
//=============================================================================
void UninitDamage(void)
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
void UpdateDamage(void) {

	if (g_IsActive != TRUE)
		return;

	switch (g_AnimStep) {

	case ANIM_START:
	{
		// アルファ値を補間で増加
		g_Alpha = g_FrameCnt / g_AnimFrame[g_AnimStep];
	}
		break;

	case ANIM_ENDING:
	{
		// アルファ値を補間で減少
		g_Alpha = 1 - g_FrameCnt / g_AnimFrame[g_AnimStep];
	}
		break;
	}

	// カウンタを進める
	g_FrameCnt += 0.01f;

	// 次のステップへ？
	if (g_FrameCnt > g_AnimFrame[g_AnimStep]) {
		g_AnimStep++;

		// アニメ終了？
		if (g_AnimStep == ANIM_NUM)
			g_IsActive = FALSE;
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawDamage(void) {

	if (g_IsActive != TRUE)
		return;

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

	// 位置やテクスチャ座標を反映
	float px = g_Pos.x;
	float py = g_Pos.y;
	float pw = g_W;
	float ph = g_H;

	float tw = 1.0f;
	float th = 1.0f;
	float tx = 0.0f;
	float ty = 0.0f;

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, g_Alpha));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
}

//=============================================================================
// damageエフェクトをセット
//=============================================================================
void SetDamage(void) {

	g_IsActive = TRUE;
	g_Alpha = 0.0f;

	g_AnimStep = ANIM_START;
	g_FrameCnt = 0.0f;
}