//=============================================================================
//
// エフェクト処理 [effect.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "bg.h"
#include "sound.h"
#include "cutin.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define		WAIT			8				// フォントアニメーションウェイト値

#define		FONT_SCALE		20.0f			// フォント拡縮の倍率(大きいほどデフォルトサイズを維持)

// テクスチャ番号
enum {

	BANNER,
	CHARA,
	LINE,
	FONT,

	TEXTURE_MAX,
};

// カットインアニメーション段階
enum {
	STEP_INTRO,		// 出現(一閃するアニメーション)
	STEP_STAY,		// メインアニメーション
	STEP_OUTRO,		// 消失
	STEP_IDLING,	// 停止(余韻＆時間調整)

	SETP_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void StopCutin(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/cutin00.png",
	"data/TEXTURE/cutin01.png",
	"data/TEXTURE/cutin02.png",
	"data/TEXTURE/cutin03.png",
};

static BOOL		g_Load = FALSE;										// 初期化を行ったかのフラグ

static XMFLOAT3	g_BannerPos;	// バナー座標(動かない)

static XMFLOAT3 g_CharaPos;		// キャラ座標(左へスライド)
static XMFLOAT3 g_CharaSpeed;	// キャラのスライドスピード

static XMFLOAT3 g_FontPos;			// フォント座標
static XMFLOAT3 g_FontPos_org;		// 基準フォント座標(動かない)
static XMFLOAT3 g_FontDiff;			// フォント最大ブレ距離
static int		g_AnimCnt;			// フォントカウンタ
static XMFLOAT2	g_FontSize;			// フォントサイズ(修正後)

static XMFLOAT2 g_Size[TEXTURE_MAX] = {		// キャラサイズ
	XMFLOAT2(SCREEN_WIDTH, 512.0f),
	XMFLOAT2(SCREEN_WIDTH, 512.0f),
	XMFLOAT2(SCREEN_WIDTH, 512.0f),
	XMFLOAT2(1024.0f, 512.0f),
};

static int		g_StepCnt;			// アニメーションカウンタ
static int		g_CutinStep;	// アニメーション段階フラグ
static float	g_CutinStepNum[SETP_MAX] = {	// 各段階の実行フレーム数
	4,
	220,
	4,
	30,
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCutin(void)
{
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++) {

		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
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



	// 初期化処理
	g_BannerPos = XMFLOAT3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f);	// 動かない

	g_CharaPos = XMFLOAT3(SCREEN_WIDTH * 1.1f, SCREEN_HEIGHT * 0.5f, 0.0f);		// CHARA初期座標
	g_CharaSpeed = XMFLOAT3(-1.5f, 0.0f, 0.0f);		// スライドスピード

	g_FontPos_org = XMFLOAT3(SCREEN_WIDTH * 0.3f, SCREEN_HEIGHT * 0.5f, 0.0f);		// FONT基準座標
	g_FontPos = g_FontPos_org;
	g_FontDiff = XMFLOAT3(20.0f, 20.0f, 0.0f);		// 最大ブレ距離
	g_AnimCnt = 0;
	g_FontSize.x = g_Size[FONT].x;
	g_FontSize.y = g_Size[FONT].y;

	g_StepCnt = 0;

	g_CutinStep = STEP_INTRO;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCutin(void)
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
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateCutin(void)
{
	// スキップ
	if (GetKeyboardTrigger(DIK_RETURN)) {
		StopCutin();
		return;
	}

	//////////////////////////////////////////////////////////////////////////////
	// 段階制御
	//////////////////////////////////////////////////////////////////////////////
	g_StepCnt++;

	// INTRO
	if (g_CutinStep == STEP_INTRO) {
		if (g_StepCnt >= g_CutinStepNum[g_CutinStep]) {
			// カウンタをリセット
			g_StepCnt = 0;
			// ステップを進める
			g_CutinStep = STEP_STAY;
		}
	}
	// STAY
	else if (g_CutinStep == STEP_STAY) {
		if (g_StepCnt >= g_CutinStepNum[g_CutinStep]) {
			// カウンタをリセット
			g_StepCnt = 0;
			// ステップを進める
			g_CutinStep = STEP_OUTRO;
		}
	}
	// OUTRO
	else if (g_CutinStep == STEP_OUTRO) {
		if (g_StepCnt >= g_CutinStepNum[g_CutinStep]) {
			// カウンタをリセット
			g_StepCnt = 0;
			// ステップを進める
			g_CutinStep = STEP_IDLING;
		}
	}
	// IDLING
	else if (g_CutinStep == STEP_IDLING) {
		if (g_StepCnt >= g_CutinStepNum[g_CutinStep]) {
			StopCutin();
		}
		return;		// これ以上実行しない
	}


	//////////////////////////////////////////////////////////////////////////////
	// キャラスライド
	//////////////////////////////////////////////////////////////////////////////
	g_CharaPos.x += g_CharaSpeed.x;


	//////////////////////////////////////////////////////////////////////////////
	// フォント処理
	//////////////////////////////////////////////////////////////////////////////
	g_AnimCnt++;
	if (g_AnimCnt % WAIT == 0) {

		// セットする距離と方向をランダムに決める(最大距離の0～1倍)
		XMFLOAT3 diff = XMFLOAT3(
			g_FontDiff.x * sinf(rand()),
			g_FontDiff.y * sinf(rand()),
			0.0f);

		// 座標を決める
		g_FontPos.x = g_FontPos_org.x + diff.x;
		g_FontPos.y = g_FontPos_org.y + diff.y;

		// サイズの倍率を決める
		float ratio = 1.0f - sinf(rand()) / FONT_SCALE;

		// サイズを拡縮
		g_FontSize.x = g_Size[FONT].x * ratio;
		g_FontSize.y = g_Size[FONT].y * ratio;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCutin(void)
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

	BG* bg = GetBG();

	// STEP_INTRO
	if (g_CutinStep == STEP_INTRO) {
		// BANNER
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BANNER]);

			float px = g_BannerPos.x - bg->pos.x;			// BGを追従
			float py = g_BannerPos.y - bg->pos.y;			//
			float pw = g_Size[BANNER].x;
			float ph = g_Size[BANNER].y * ((float)g_StepCnt / g_CutinStepNum[STEP_INTRO]);	// 進行度

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
		// CHARA
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[CHARA]);

			float px = g_CharaPos.x - bg->pos.x;			// BGを追従
			float py = g_CharaPos.y - bg->pos.y;			//
			float pw = g_Size[CHARA].x;
			float ph = g_Size[CHARA].y * ((float)g_StepCnt / g_CutinStepNum[STEP_INTRO]);	// 進行度

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
	// STEP_STAY
	else if (g_CutinStep == STEP_STAY) {
		// BANNER
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BANNER]);

			float px = g_BannerPos.x - bg->pos.x;			// BGを追従
			float py = g_BannerPos.y - bg->pos.y;			//
			float pw = g_Size[BANNER].x;
			float ph = g_Size[BANNER].y;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
		// CHARA
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[CHARA]);

			float px = g_CharaPos.x - bg->pos.x;			// BGを追従
			float py = g_CharaPos.y - bg->pos.y;			//
			float pw = g_Size[CHARA].x;
			float ph = g_Size[CHARA].y;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
		// LINE
		{
			SetBlendState(BLEND_MODE_ADD);
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[LINE]);

			float px = g_BannerPos.x - bg->pos.x;			// BGを追従
			float py = g_BannerPos.y - bg->pos.y;			//
			float pw = g_Size[LINE].x;
			float ph = g_Size[LINE].y;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = sinf(rand());	// ランダム配置でシュシュシュッの効果
			float ty = 0.0f;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// 2回描画
			GetDeviceContext()->Draw(4, 0);
			GetDeviceContext()->Draw(4, 0);

			SetBlendState(BLEND_MODE_ALPHABLEND);
		}
		// FONT
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[FONT]);

			float px = g_FontPos.x - bg->pos.x;			// BGを追従
			float py = g_FontPos.y - bg->pos.y;			//
			float pw = g_FontSize.x;
			float ph = g_FontSize.y;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
	// STEP_OUTRO
	else if (g_CutinStep == STEP_OUTRO) {
		// BANNER
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BANNER]);

			float px = g_BannerPos.x - bg->pos.x;			// BGを追従
			float py = g_BannerPos.y - bg->pos.y;			//
			float pw = g_Size[BANNER].x;
			float ph = g_Size[BANNER].y * (1.0f - (float)g_StepCnt / g_CutinStepNum[STEP_INTRO]);	// 1 - 進行度

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
		// CHARA
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[CHARA]);

			float px = g_CharaPos.x - bg->pos.x;			// BGを追従
			float py = g_CharaPos.y - bg->pos.y;			//
			float pw = g_Size[CHARA].x;
			float ph = g_Size[CHARA].y * (1.0f - (float)g_StepCnt / g_CutinStepNum[STEP_INTRO]);	// 1 - 進行度

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}

//=============================================================================
// CUTINを終了
//=============================================================================
void StopCutin(void) {
	// CUTINを終了
	SetCutin(FALSE);
	// SEを切る
	StopSound(SOUND_LABEL_SE_CUTIN000);
	StopSound(SOUND_LABEL_SE_CUTIN001);
	// BOSS曲を流す
	PlaySound(SOUND_LABEL_BGM_BOSS);
}