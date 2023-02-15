//=============================================================================
//
// スコア処理 [score.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "score.h"
#include "UI.h"
#include "file.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define		TEXTURE_WIDTH				44				// 文字サイズ
#define		TEXTURE_HEIGHT				96				// 
#define		TEXTURE_MAX					1				// テクスチャの数

#define		RANDOM_PLUS					0.1f			// デフォルト得点が最大で何倍プラスされる

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/number384x64.png",
};

static BOOL				g_Load = FALSE;				// 初期化を行ったかのフラグ

static BOOL				g_IsActive;					// true:使っている  false:未使用
static float			g_w, g_h;					// 幅と高さ
static XMFLOAT3			g_Pos;						// ポリゴンの座標
static int				g_TexNo;					// テクスチャ番号

static int				g_Score;						// スコア

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitScore(void)
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


	// スコアの初期化
	g_IsActive   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 1472.0f, 12.0f, 0.0f };
	g_TexNo = 0;

	g_Score = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitScore(void)
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
void UpdateScore(void)
{

}

//=============================================================================
// 描画処理
// DrawUI()で呼び出す
//=============================================================================
void DrawScore(float alpha)
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
	int number = g_Score;
	for (int i = 0; i < SCORE_DIGIT; i++)
	{
		// 今回表示する桁の数字
		float x = (float)(number % 10);

		// スコアの位置やテクスチャー座標を反映
		float px = g_Pos.x + g_w * (SCORE_DIGIT - i - 1) - UI->pos.x;	// スコアの表示位置X(左上)	// UIを追従 // 右の桁から描画
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


//=============================================================================
// スコアを加算する(ブレあり)
// 引数:add :追加する点数。マイナスも可能
//=============================================================================
void AddScore(int add)
{
	// デフォルト得点のRANBOM_PLUS倍までランダムにプラス
	add += rand() % (int)(add * RANDOM_PLUS);

	g_Score += add;
	if (g_Score > SCORE_MAX)
	{
		g_Score = SCORE_MAX;
	}

}

//=============================================================================
// スコアを取得
//=============================================================================
int GetScore(void)
{
	return g_Score;
}