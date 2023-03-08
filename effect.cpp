//=============================================================================
//
// エフェクト処理 [effect.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "effect.h"
#include "bg.h"
#include "player.h"
#include "sound.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define		TEXTURE_MAX					EFFECT_TYPE_MAX
#define		WAIT_DEFAULT				1						// デフォルトウェイト値

#define		EFFECT_MODE_GLOW_NUM		1						// GLOWモードの加算回数

#define		MANUAL_EFFECT_HEIGHT		0.95f					// 操作説明エフェクトの座標(パーセンテージ表記)

// エフェクト描画モード番号
enum {

	DRAW_DEFAULT,			// デフォルト
	DRAW_GLOW,				// 重ね加算

	DRAW_MAX,
};

// エフェクト追従ラベル
enum {

	EFFECT_TRACK_NONE,				// 誰も追従しない
	EFFECT_TRACK_PLAYER,			// プレイヤーを追従

	EFFECT_TRACK_MAX,
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

// エフェクトデータ
static EFFECT_DATA g_EffectData[EFFECT_TYPE_MAX] = {

	{ 
		"data/TEXTURE/player_normal_shot.png",				// テクスチャ名, 
		XMINT2(200, 200), DRAW_GLOW, 0.7f,					// サイズ, 描画モード, α
		EFFECT_TRACK_PLAYER, XMFLOAT3(90.0f, 0.0f, 0.0f),	// 追従ラベル, ズレ
		TRUE,												// 排他フラグ
		XMINT2(2, 2), 3, WAIT_DEFAULT,						// 分割数, パターン数, ウェイト,
		EFFECT_LAYER_FRONT_OF_PLAYER,						// レイヤー
	},

	{
		"data/TEXTURE/hit00.png",
		XMINT2(200, 200), DRAW_GLOW, 1.0f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(4, 4), 10, 4,
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},

	{
		"data/TEXTURE/hit01.png",
		XMINT2(200, 200), DRAW_GLOW, 1.0f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(4, 4), 10, 2,
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},

	//{	// 旧爆発エフェクト
	//	"data/TEXTURE/bomb.png",
	//	XMINT2(200, 200), DRAW_DEFAULT, 1.0f,
	//	EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	FALSE,
	//	XMINT2(4, 4), 16, WAIT_DEFAULT,
	//	EFFECT_LAYER_FRONT_OF_PLAYER,
	//},

	{	// 新爆発エフェクト
		"data/TEXTURE/explode.png",
		XMINT2(250, 250), DRAW_DEFAULT, 1.0f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(3, 3), 9, 4,
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},

	{
		"data/TEXTURE/backfire.png",
		XMINT2(100, 100), DRAW_DEFAULT, 1.0f,
		EFFECT_TRACK_PLAYER, XMFLOAT3(-125.0f, -5.0f, 0.0f),
		FALSE,
		XMINT2(2, 1), 2, 4,
		EFFECT_LAYER_BEHIND_PLAYER,
	},
		
	{
		"data/TEXTURE/backglow00.png",
		XMINT2(300, 300), DRAW_GLOW, 1.0f,
		EFFECT_TRACK_PLAYER, XMFLOAT3(-25.0f, 5.0f, 0.0f),
		FALSE,
		XMINT2(2, 1), 2, 8,
		EFFECT_LAYER_BEHIND_PLAYER,
	},
	
	{
		"data/TEXTURE/backparticles.png",
		XMINT2(400, 400), DRAW_GLOW, 1.0f,
		EFFECT_TRACK_PLAYER, XMFLOAT3(-100.0f, -5.0f, 0.0f),
		FALSE,
		XMINT2(4, 4), 16, 4,
		EFFECT_LAYER_BEHIND_PLAYER,
	},

	{
		"data/TEXTURE/flash00.png",
		XMINT2(200, 200), DRAW_GLOW, 0.5f,
		EFFECT_TRACK_PLAYER, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(1, 1), 1, 4,
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},

	{
		"data/TEXTURE/heal.png",
		XMINT2(300, 300), DRAW_GLOW, 1.0f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(4, 4), 10, 4,
		EFFECT_LAYER_BEHIND_PLAYER,
	},
	
	{
		"data/TEXTURE/powerup.png",
		XMINT2(400, 400), DRAW_GLOW, 1.0f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(4, 4), 10, 4,
		EFFECT_LAYER_BEHIND_PLAYER,
	},

	{
		"data/TEXTURE/popped00.png",
		XMINT2(200, 200), DRAW_GLOW, 1.0f,
		EFFECT_TRACK_PLAYER, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(2, 1), 2, 4,
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},
		
	{
		"data/TEXTURE/popped01.png",
		XMINT2(200, 200), DRAW_GLOW, 0.8f,
		EFFECT_TRACK_PLAYER, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(2, 1), 2, 4,
		EFFECT_LAYER_BEHIND_PLAYER,
	},


	{	// ボス撃沈爆発(普通の爆発よりデカい)
		"data/TEXTURE/explode.png",
		XMINT2(500, 500), DRAW_DEFAULT, 1.0f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(3, 3), 9, 4,
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},

	// 操作説明エフェクト
	{
		"data/TEXTURE/manualEffect00.png",
		XMINT2(1024, 128), DRAW_DEFAULT, 0.5f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(1, 1), 1, 1,		// ループ
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},
	{
		"data/TEXTURE/manualEffect01.png",
		XMINT2(1024, 128), DRAW_DEFAULT, 0.5f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(1, 1), 1, 60,	// 表示秒数
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},
	{
		"data/TEXTURE/manualEffect02.png",
		XMINT2(256, 128), DRAW_DEFAULT, 0.5f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(1, 1), 1, 1,		// ループ
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},
	{
		"data/TEXTURE/manualEffect03.png",
		XMINT2(512, 128), DRAW_DEFAULT, 0.5f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(1, 1), 1, 1,		// ループ
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},


};

static BOOL		g_Load = FALSE;										// 初期化を行ったかのフラグ
static EFFECT	g_Effect[EFFECT_MAX];								// エフェクト構造体

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEffect(void)
{
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++) {

		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_EffectData[i].textureName,
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
	for (int i = 0; i < EFFECT_MAX; i++) {
		g_Effect[i].isActive = FALSE;	// 　全エフェクトを消す
	}

	// モードによりエフェクトをセット
	if (GetMode() == MODE_TITLE) {

		// パッド有効化ならしない
		if (!IsPadActive()) {
			// 操作説明をセット
			SetEffect(EFFECT_TYPE_MANUAL00, XMFLOAT3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * MANUAL_EFFECT_HEIGHT, 0.0f), EFFECT_LOOP_FOREVER);
		}

	}
	else if (GetMode() == MODE_GAME) {
		// BACKFIREエフェクトを有効化(0番目プレイヤーに対して)
		SetEffect(EFFECT_TYPE_PLAYER_BACKFIRE, 0, EFFECT_LOOP_FOREVER);
		// BACKGLOWエフェクトを有効化(0番目プレイヤーに対して)
		SetEffect(EFFECT_TYPE_PLAYER_BACKGLOW, 0, EFFECT_LOOP_FOREVER);
		// BACKPARTICLESを有効化
		SetEffect(EFFECT_TYPE_PLAYER_BACKPARTICLES, 0, EFFECT_LOOP_FOREVER);

		// パッド有効化ならしない
		if (!IsPadActive()) {
			// 操作説明をセット
			SetEffect(EFFECT_TYPE_MANUAL01, XMFLOAT3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * MANUAL_EFFECT_HEIGHT, 0.0f), 30.0f);	// 30秒間プレイ
		}
	}
	else if (GetMode() == MODE_GAMEOVER || GetMode() == MODE_RESULT) {

		// パッド有効化ならしない
		if (!IsPadActive()) {

			// 操作説明をセット
			SetEffect(EFFECT_TYPE_MANUAL02, XMFLOAT3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * MANUAL_EFFECT_HEIGHT, 0.0f), EFFECT_LOOP_FOREVER);
		}

	}



	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEffect(void)
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

	// 全エフェクトを無効化
	for (int i = 0; i < EFFECT_MAX; i++) {
		g_Effect[i].isActive = FALSE;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEffect(void)
{
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (g_Effect[i].isActive == TRUE)
		{
			int type = g_Effect[i].type;

			////////////////////////////////////////////////////////////////////////////
			// アニメーション処理
			////////////////////////////////////////////////////////////////////////////
			g_Effect[i].animFrameCnt++;
			if ((g_Effect[i].animFrameCnt % g_EffectData[type].wait) == 0)
			{
				// パターンの切り替え
				g_Effect[i].animPattern = (g_Effect[i].animPattern + 1) % g_EffectData[type].patternNum;
			}

			// 無限ループなら終了判定しない
			if (g_Effect[i].roop != EFFECT_LOOP_FOREVER) {

				// アニメーション終了？
				if (g_Effect[i].animFrameCnt >= g_Effect[i].animTotalFrame) {
					g_Effect[i].isActive = FALSE;
					continue;
				}

			}



			////////////////////////////////////////////////////////////////////////////
			// エフェクト追従処理
			////////////////////////////////////////////////////////////////////////////
			
			// プレイヤーを追従
			if (g_EffectData[type].trackLabel == EFFECT_TRACK_PLAYER) {

				PLAYER* player = GetPlayer() + g_Effect[i].trackTarget;

				// プレイヤーが存命であれば
				if (player->isActive == TRUE) {

					XMVECTOR xmvPos = XMLoadFloat3(&player->pos);
					XMVECTOR xmvDiff = XMLoadFloat3(&g_EffectData[type].diff);

					// 追従
					XMStoreFloat3(&g_Effect[i].pos, xmvPos + xmvDiff);
				}
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEffect(int layer)
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
	for (int i = 0; i < TEXTURE_MAX; i++) {

		if (g_Effect[i].isActive == TRUE) {

			int type = g_Effect[i].type;

			// レイヤー合っていなければ描画しない
			if (g_EffectData[type].layer != layer) {
				continue;
			}

			////////////////////////////////////////////////////////////////////////////
			// DEFAULT
			////////////////////////////////////////////////////////////////////////////
			if (g_EffectData[type].mode == DRAW_DEFAULT) {

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[type]);

				float px = g_Effect[i].pos.x - bg->pos.x;			// BGを追従
				float py = g_Effect[i].pos.y - bg->pos.y;			//
				float pw = g_Effect[i].w;
				float ph = g_Effect[i].h;

				float tw = 1.0f / g_EffectData[type].patternDivide.x;
				float th = 1.0f / g_EffectData[type].patternDivide.y;
				float tx = (float)(g_Effect[i].animPattern % g_EffectData[type].patternDivide.x) * tw;
				float ty = (float)(g_Effect[i].animPattern / g_EffectData[type].patternDivide.x) * th;

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, g_EffectData[type].alpha));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			////////////////////////////////////////////////////////////////////////////
			// GLOW
			////////////////////////////////////////////////////////////////////////////
			else if (g_EffectData[type].mode == DRAW_GLOW) {

				SetBlendState(BLEND_MODE_ADD);

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[type]);

				float px = g_Effect[i].pos.x - bg->pos.x;			// BGを追従
				float py = g_Effect[i].pos.y - bg->pos.y;			//
				float pw = g_Effect[i].w;
				float ph = g_Effect[i].h;

				float tw = 1.0f / g_EffectData[type].patternDivide.x;
				float th = 1.0f / g_EffectData[type].patternDivide.y;
				float tx = (float)(g_Effect[i].animPattern % g_EffectData[type].patternDivide.x) * tw;
				float ty = (float)(g_Effect[i].animPattern / g_EffectData[type].patternDivide.x) * th;

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, g_EffectData[type].alpha));

				for (int i = 0; i < EFFECT_MODE_GLOW_NUM; i++) {

					// ポリゴン描画
					GetDeviceContext()->Draw(4, 0);
				}

				SetBlendState(BLEND_MODE_ALPHABLEND);
			}
		}
	}
}

//=============================================================================
// エフェクトを取得
//=============================================================================
EFFECT* GetEffect(void) {
	return &g_Effect[0];
}

//=============================================================================
// エフェクトデータを取得
//=============================================================================
EFFECT_DATA* GetEffectData(void) {
	return &g_EffectData[0];
}

//=============================================================================
// 追従エフェクトをセット
//=============================================================================
void SetEffect(int type, int target, int loop) {

	for (int i = 0; i < EFFECT_MAX; i++) {

		if (g_Effect[i].isActive == TRUE) {
			continue;
		}

		// 排他フラグ？
		if (g_EffectData[type].isOnly == TRUE) {
			StopEffect(type);		// 既存をすべて無効
		}

		g_Effect[i].isActive = TRUE;
		g_Effect[i].w = g_EffectData[type].size.x;
		g_Effect[i].h = g_EffectData[type].size.y;
		g_Effect[i].type = type;
		g_Effect[i].roop = loop;

		g_Effect[i].animFrameCnt = 0;
		g_Effect[i].animPattern = 0;
		g_Effect[i].animTotalFrame = loop * g_EffectData[type].patternNum * g_EffectData[type].wait;

		g_Effect[i].trackTarget = target;

		return;			// 1個セットできたら終了
	}
}

//=============================================================================
// エフェクトをセット
//=============================================================================
void SetEffect(int type, XMFLOAT3 pos, int loop) {

	for (int i = 0; i < EFFECT_MAX; i++) {

		// 未使用スロットを検索
		if (g_Effect[i].isActive == TRUE) {
			continue;
		}

		// 排他フラグ？
		if (g_EffectData[type].isOnly == TRUE) {
			StopEffect(type);		// 既存をすべて無効
		}

		g_Effect[i].isActive = TRUE;
		g_Effect[i].w = g_EffectData[type].size.x;
		g_Effect[i].h = g_EffectData[type].size.y;
		g_Effect[i].pos = pos;
		g_Effect[i].type = type;
		g_Effect[i].roop = loop;

		g_Effect[i].animFrameCnt = 0;
		g_Effect[i].animPattern = 0;
		g_Effect[i].animTotalFrame = loop * g_EffectData[type].patternNum * g_EffectData[type].wait;

		return;			// 1個セットできたら終了
	}
}

//=============================================================================
// エフェクトを中止
//=============================================================================
void StopEffect(int type) {

	for (int i = 0; i < EFFECT_MAX; i++) {

		// 同一エフェクトをすべて無効化
		if (g_Effect[i].type == type) {
			g_Effect[i].isActive = FALSE;
		}

	}
}