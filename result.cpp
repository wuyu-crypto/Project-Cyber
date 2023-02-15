//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "result.h"
#include "input.h"
#include "score.h"
#include "fade.h"
#include "sound.h"
#include "file.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				64				// 文字サイズ
#define TEXTURE_HEIGHT				128				//

#define TEXTURE_MAX					4				// テクスチャの数

#define	PERIOD						128				// 点滅する周期

enum {
	BG,
	GLOW,
	LOGO,
	SCORE,
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/result00.png",
	"data/TEXTURE/result01.png",
	"data/TEXTURE/result02.png",
	"data/TEXTURE/number384x64.png",
};

// SCORE変数
static BOOL				g_IsActive;
static float			g_w, g_h;					// 幅と高さ
static XMFLOAT3			g_Pos;						// ポリゴンの座標
static int				g_TexNo;					// テクスチャ番号

// RECORD変数
static int				g_ScoreRecord[SCORE_RECORD_MAX];	// RECORD
static int				g_ScoreRank;						// NEW RECORDの順位
static XMFLOAT3			g_DefaultRecordPos;					// RECORDの基準座標
static XMFLOAT3			g_RecordDiff;						// RECORDの間隔
static XMFLOAT3			g_RecordPos[SCORE_RECORD_MAX];		// RECORDの座標
static float			g_RecordW;							// RECORDの文字サイズ
static float			g_RecordH;							// 
static float			g_RecordStuffing;					// 文字詰め

static BOOL				g_Load = FALSE;

static int				g_Cnt;
static float			g_Alpha;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
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
	g_Pos   = XMFLOAT3(1178.0f, SCREEN_HEIGHT * 0.45f, 0.0f);	// SCOREの座標
	g_TexNo = 0;

	// RECORDの初期化
	{
		int score[SCORE_RECORD_MAX * 2 + 1];	// スコアスロット
		SAVEDATA* data = GetData();

		// まず全スロットに0を入れる
		for (int i = 0; i < SCORE_RECORD_MAX * 2 + 1; i++) {
			score[i] = 0;
		}
		// 先頭スロットにRECORDを入れる
		for (int i = 0; i < SCORE_RECORD_MAX; i++) {

			score[i] = data->score[i];
		}
		// 現在SCOREを入れる
		g_ScoreRank = SCORE_RECORD_MAX;
		score[g_ScoreRank] = GetScore();

		// スコアをソート
		{
			BOOL isFinish = FALSE;	// ソート終了フラグ
			int find = 1;			// 添え字調整値

			while (isFinish == FALSE) {

				BOOL isSwaped = FALSE;		// ソートしたかフラグ
				for (int i = 0; i < (SCORE_RECORD_MAX * 2 + 1) - find; i++) {	// 1周にスロット数-find回ソートする

					if (score[i] <= score[i + 1]) {	// 現在スロットが次のスロット以下なら
						// スロットを交換する
						int _score = score[i];
						score[i] = score[i + 1];
						score[i + 1] = _score;

						isSwaped = TRUE;		// ソートした

						// ソートしたのがNEW RECORDなら
						if (i + 1 == g_ScoreRank) {
							g_ScoreRank = i;		// ランキング更新
						}
					}

				}

				if (isSwaped == FALSE) {	// 1回もソートしなかった？
					isFinish = TRUE;	// ソート終了
				}

				find++;
			}
		}

		// 有効スコアをRECORDに記録
		for (int i = 0; i < SCORE_RECORD_MAX; i++) {
			g_ScoreRecord[i] = score[i];
		}

		// RECORDを保存
		SaveData();
	}

	// RECORD座標の初期化
	{
		g_DefaultRecordPos = XMFLOAT3(SCREEN_WIDTH * 0.67f, SCREEN_HEIGHT * 0.625f, 0.0f);
		g_RecordDiff = XMFLOAT3(SCREEN_WIDTH * 0.02f, SCREEN_HEIGHT * 0.0685f, 0.0f);	// 行間

		// 座標を計算
		g_RecordPos[0] = g_DefaultRecordPos;
		for (int i = 0; i < SCORE_RECORD_MAX; i++) {

			if (i > 0) {
				XMVECTOR pos = XMLoadFloat3(&g_RecordPos[i - 1]);
				XMVECTOR diff = XMLoadFloat3(&g_RecordDiff);

				XMStoreFloat3(&g_RecordPos[i], pos + diff);
			}
		}

		g_RecordW = 32;				// 文字サイズ
		g_RecordH = 64;				//

		g_RecordStuffing = 1.0f;	// 文字詰め
	}


	g_Cnt = 0;

	// BGMをプレイ
	PlaySound(SOUND_LABEL_BGM_RESULT);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResult(void)
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
void UpdateResult(void)
{
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		// SEをプレイ
		PlaySound(SOUND_LABEL_SE_SELECT000);
		// モードチェンジ
		SetFade(FADE_OUT, MODE_TITLE);
	}



	//////////////////////////////////////////////////////////////////////////////
	// カウンタでα制御
	//////////////////////////////////////////////////////////////////////////////
	{
		g_Cnt = (g_Cnt + 1) % PERIOD;

		float ratio = (float)g_Cnt / (float)PERIOD;

		// グラフをかける( y=1-(2x-1)^2 )
		ratio = 1 -
			(2 * ratio - 1) * (2 * ratio - 1);

		g_Alpha = ratio;
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResult(void)
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
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BG]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// GLOW
	{
		SetBlendState(BLEND_MODE_ADD);
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GLOW]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_Alpha));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		SetBlendState(BLEND_MODE_ALPHABLEND);
	}

	// LOGO
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[LOGO]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// SCORE
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE]);

		// 桁数分処理する
		int number = GetScore();
		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = (float)(number % 10);

			// スコアの位置やテクスチャー座標を反映
			float px = g_Pos.x + g_w * (SCORE_DIGIT - i - 1);	// スコアの表示位置X(左上)	// 右の桁から描画
			float py = g_Pos.y;									// スコアの表示位置Y(左上)	//
			float pw = g_w;										// スコアの表示幅
			float ph = g_h;										// スコアの表示高さ

			float tw = 1.0f / 10;		// テクスチャの幅
			float th = 1.0f / 1;		// テクスチャの高さ
			float tx = x * tw;			// テクスチャの左上X座標
			float ty = 0.0f;			// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			// 次の桁へ
			number /= 10;
		}

	}

	// RECORD
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE]);

		// RECORD数処理する
		for (int j = 0; j < SCORE_RECORD_MAX; j++) {
			int score = g_ScoreRecord[j];
			
			// 桁数分処理する
			for (int i = 0; i < SCORE_DIGIT; i++) {

				// 今回表示する桁の数字
				float x = (float)(score % 10);

				// スコアの位置やテクスチャー座標を反映
				float px = g_RecordPos[j].x + g_RecordW * (SCORE_DIGIT - i - 1) * g_RecordStuffing;	// スコアの表示位置X(左上)	// 右の桁から描画
				float py = g_RecordPos[j].y;								// スコアの表示位置Y(左上)	//
				float pw = g_RecordW;										// スコアの表示幅
				float ph = g_RecordH;										// スコアの表示高さ

				float tw = 1.0f / 10;		// テクスチャの幅
				float th = 1.0f / 1;		// テクスチャの高さ
				float tx = x * tw;			// テクスチャの左上X座標
				float ty = 0.0f;			// テクスチャの左上Y座標

				// NEW RECORDなら色変えて表示
				if (j == g_ScoreRank) {
					SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
				}
				else {
					SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f));	// 灰色
				}

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				// 次の桁へ
				score /= 10;
			}
		}
	}
}

//=============================================================================
// RECORDを取得
//=============================================================================
int* GetRecord(void) {
	return &g_ScoreRecord[0];
}