//=============================================================================
//
// ビル処理 [building.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "building.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX					BUILDING_LABEL_MAX			// テクスチャの数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bg_cyber_city002.png",
};

static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ
static BUILDING	g_Building[BUILDING_MAX];

// スクロールスピード
static float g_ScrlSpeed[BUILDING_MAX] = {
	200.0f,
};

// 出現間隔
static int g_Wait[BUILDING_MAX] = {
	200,
};

// テクスチャサイズ
static float g_TexWidth[BUILDING_MAX] = {
	1024,
};
static float g_TexHeight[BUILDING_MAX] = {
	2048,
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBuilding(void)
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


	// 背景の初期化
	for (int i = 0; i < BUILDING_MAX; i++) {
		g_Building[i].w = g_TexWidth[i];
		g_Building[i].h = g_TexHeight[i];
		g_Building[i].pos = XMFLOAT3(0.0f, 300.0f, 0.0f);		// 初期座標(Y座標が肝)
		g_Building[i].isActive = FALSE;
		g_Building[i].texNo = 0;

		g_Building[i].scrlSpeed = g_ScrlSpeed[i];

		g_Building[i].wait = g_Wait[i];
		g_Building[i].waitCnt = 0;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBuilding(void)
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
void UpdateBuilding(void)
{
	for (int i = 0; i < BUILDING_MAX; i++) {

		g_Building[i].waitCnt++;												// カウントを進める

		if (g_Building[i].waitCnt > g_Building[i].wait) {						// 間隔に達したら

			g_Building[i].isActive = TRUE;									// アクティブ化
			g_Building[i].pos.x = SCREEN_WIDTH + g_Building[i].w * 0.5f;	// 座標をセット(右端より半分出っ張る)
			g_Building[i].pos.y = rand() % 200 + 300;

			// 次の出現間隔を設定
			g_Building[i].waitCnt = 0;
		}

		// 横スクロール
		g_Building[i].pos.x -= g_Building[i].scrlSpeed;

		// 左端へ出たら非アクティブ化
		if (g_Building[i].pos.x < -g_Building[i].w * 0.5f) {					// 左端より半分出っ張る
			g_Building[i].isActive = FALSE;
		}

	}

#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBuilding(void)
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

	for (int i = 0; i < BUILDING_MAX; i++) {

		// 非アクティブだったら飛ばす
		if (g_Building[i].isActive == FALSE) {
			continue;
		}

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer,
			g_Building[i].pos.x, g_Building[i].pos.y, g_Building[i].w, g_Building[i].h,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}