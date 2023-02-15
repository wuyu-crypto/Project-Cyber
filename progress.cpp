//=============================================================================
//
// ステージ進行率 [progress.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "progress.h"
#include "UI.h"
#include "waveController.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define		TEXTURE_WIDTH				32				// 文字サイズ
#define		TEXTURE_HEIGHT				64				// 
#define		TEXTURE_MAX					1				// テクスチャの数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[] = {
	"data/TEXTURE/number384x64.png",
};

static BOOL						g_Load = FALSE;			// 初期化を行ったかのフラグ


static BOOL						g_IsActive;
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static int						g_Progress;					// 進行率

//static BOOL						g_IsMax;					// 進行率が最大値に達した？

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitProgress(void)
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

	// 進行率の初期化
	g_IsActive = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = { 1040.0f, 4.0f, 0.0f };		// 表示位置(左上)
	g_TexNo = 0;

	g_Progress = 50;

	//g_IsMax = FALSE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitProgress(void)
{
	if (g_Load == FALSE) {
		return;
	}

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

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateProgress(void)
{
	// 無効なら実行しない
	if (g_IsActive == FALSE) {
		return;
	}

	//if (g_IsMax == TRUE) {		// 既に最大値に達していれば
	//	return;
	//}

	// 時間を取得
	float time = GetTime();
	float bossTime = GetBossTime();

	// 割合を求める
	float progress = time / bossTime;

	// 100をかけて整数を取る
	g_Progress = (int)(progress * 100.0f);

	// 最大値を超えたら最大値に固定
	if (g_Progress >= PROGRESS_MAX) {
		g_Progress = PROGRESS_MAX;
		//g_IsMax = TRUE;
	}





#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);

#endif

}

//=============================================================================
// 描画処理
// DrawUI()で呼び出す
//=============================================================================
void DrawProgress(float alpha)
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

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	UI* UI = GetUI();

	// 桁数分処理する
	int number = g_Progress;
	for (int i = 0; i < PROGRESS_DIGIT; i++)
	{
		// 今回表示する桁の数字
		float x = (float)(number % 10);

		// スコアの位置やテクスチャー座標を反映
		float px = g_Pos.x + g_w * (PROGRESS_DIGIT - i - 1) - UI->pos.x;	// スコアの表示位置X(左上)	// UIを追従
		float py = g_Pos.y - UI->pos.y;									// スコアの表示位置Y(左上)	//
		float pw = g_w;										// スコアの表示幅
		float ph = g_h;										// スコアの表示高さ

		float tw = 1.0f / 10;		// テクスチャの幅
		float th = 1.0f / 1;		// テクスチャの高さ
		float tx = x * tw;			// テクスチャの左上X座標
		float ty = 0.0f;			// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// 次の桁へ
		number /= 10;
	}
}