//=============================================================================
//
// ゲーム中背景 [bg.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "bg.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH * 2)		// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)			//
#define TEXTURE_MAX					(1)				// テクスチャの数

#define	VIBRATION
#undef VIBRATION

#ifdef VIBRATION
#define	VIBRATION_FRAME_NUM			120.0f			// 振動1周のフレーム数
#define	VIBRATION_ROOP				5.0f			// 振動のループ数
#define	VIBRATION_SCALE				-100.0f			// 振動の強さ(ズレの長さ)
#endif

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
#ifdef VIBRATION
void SetVibration(float frame, float roop, float scale);
#endif
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bg.jpg",
};

static BOOL	g_Load = FALSE;			// 初期化を行ったかのフラグ
static BG	g_BG;

#ifdef VIBRATION
static BOOL				g_isVibrationAllowed;		// 振動を許可か？
static BOOL				g_isVibrating;				// 振動フラグ


static float			g_VibrationFrameCnt;			// 振動用フレームカウンタ
static float			g_VibrationRoopCnt;				// ループカウンタ

static float			g_VibrationFrameNum;			// 振動1周のフレーム数
static float			g_VibrationRoopNum;				// 振動のループ数
static float			g_VibrationScale;				// 振動の強さ
#endif


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBG(void)
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
	g_BG.w = TEXTURE_WIDTH;
	g_BG.h = TEXTURE_HEIGHT;
	g_BG.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_BG.isActive = TRUE;
	g_BG.texNo = 0;

	g_BG.scrl = 0;

#ifdef VIBRATION
	// 振動の初期化
	g_isVibrationAllowed = TRUE;
	g_isVibrating = FALSE;
#endif



	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBG(void)
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
void UpdateBG(void)
{

#ifdef VIBRATION
	// 振動が許可されているか？
	if (g_isVibrationAllowed == TRUE) {

		if (g_isVibrating == TRUE) {

			// カウンタを進める
			g_VibrationFrameCnt++;
			if (g_VibrationFrameCnt == g_VibrationFrameNum) {
				g_VibrationFrameCnt = 0.0f;
				g_VibrationRoopCnt++;
			}

			// 振動の中心点座標を求める(原点から左へSCALEの長さ)
			XMFLOAT3 center = XMFLOAT3(-g_VibrationScale, 0.0f, 0.0f);

			// 今回の角度を求める
			float degree = (g_VibrationFrameCnt / g_VibrationFrameNum) * 360.0f;

			// ラジアンに変換
			float rad = XMConvertToRadians(degree);

			// 振動座標を求める
			XMFLOAT3 pos = XMFLOAT3(
				center.x + cosf(rad) * g_VibrationScale,
				center.y + sinf(rad) * g_VibrationScale,
				0.0f
			);

			// 注視点を移動
			g_BG.pos = pos;


			// 振動終了？
			if (g_VibrationRoopCnt == g_VibrationRoopNum) {

				// 注視点をリセット
				g_BG.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);

				// 振動終了
				g_isVibrating = FALSE;

			}

			// 振動したらスクロールを実行しない
			return;

		}

	}

#endif

	// スクロール
	g_BG.scrl += 0.0005f;


#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBG(void)
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
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		g_BG.pos.x, g_BG.pos.y, g_BG.w, g_BG.h,
		g_BG.scrl, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
}


//=============================================================================
// BG構造体の先頭アドレスを取得
//=============================================================================
BG* GetBG(void)
{
	return &g_BG;
}

#ifdef VIBRATION

// 振動エフェクト
void SetVibration(float frame, float roop, float scale) {

	// 振動を有効化
	g_isVibrating = TRUE;

	// NULLならデフォルトをセット
	if (frame == NULL) {
		g_VibrationFrameNum = VIBRATION_FRAME_NUM;
	}
	else {
		g_VibrationFrameNum = frame;
	}

	if (roop == NULL) {
		g_VibrationRoopNum = VIBRATION_ROOP;
	}
	else {
		g_VibrationRoopNum = roop;
	}

	if (scale == NULL) {
		g_VibrationScale = VIBRATION_SCALE;
	}
	else {
		g_VibrationScale = scale;
	}

	// パラメーターをクリア
	g_VibrationFrameCnt = 0.0f;
	g_VibrationRoopCnt = 0.0f;
}

#endif