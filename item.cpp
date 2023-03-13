//=============================================================================
//
// アイテム [item.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "item.h"
#include "collision.h"
#include "bg.h"
#include "effect.h"
#include "sound.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define			TEXTURE_MAX			ITEM_TYPE_MAX

#define			HEAL_NUM			2						// 回復量

#define			SPEED_RANGE			5.0f					// アイテム移動の上下のブレ幅

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static ITEM		g_Item[ITEM_MAX];			// バレット構造体

static ITEM_DATA g_ItemData[ITEM_TYPE_MAX] = {

	{
		"data/TEXTURE/heart.png",								// テクスチャ名
		80.0f, 80.0f,											// キャラサイズ
		XMFLOAT3(-5.0f, 0.0f, 0.0f),							// スピード
		1, 1, 1, 4,												// 分割数, パターン数, ウェイト
		80, 80,													// 当たり判定

	},

	{
		"data/TEXTURE/power.png",								// テクスチャ名
		80.0f, 80.0f,											// キャラサイズ
		XMFLOAT3(-5.0f, 0.0f, 0.0f),							// スピード
		1, 1, 1, 4,												// 分割数, パターン数, ウェイト
		80, 80,													// 当たり判定

	},

};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitItem(void)
{
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_ItemData[i].textureName,
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

	for (int i = 0; i < ITEM_MAX; i++)
	{
		g_Item[i].isActive = FALSE;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitItem(void)
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
void UpdateItem(void) {

	for (int i = 0; i < ITEM_MAX; i++) {

		// 無効なら実行しない
		if (g_Item[i].isActive == FALSE) {
			continue;
		}

		int type = g_Item[i].type;

		/////////////////////////////////////////////////////////////////////////////
		// 移動処理
		/////////////////////////////////////////////////////////////////////////////
		{
			XMVECTOR xmvPos = XMLoadFloat3(&g_Item[i].pos);
			XMVECTOR xmvSpeed = XMLoadFloat3(&g_Item[i].speed);

			XMStoreFloat3(&g_Item[i].pos, xmvPos + xmvSpeed);
		}

		// マップ外に出たら消滅(半分突き出して)
		if (g_Item[i].pos.y > SCREEN_HEIGHT + g_Item[i].h * 0.5f) {
			g_Item[i].isActive = FALSE;
		}



		/////////////////////////////////////////////////////////////////////////////
		// 当たり判定
		/////////////////////////////////////////////////////////////////////////////
		PLAYER* player = GetPlayer();

		// プレイヤーの数分処理
		for (int j = 0; j < PLAYER_MAX; j++) {

			// 生きてるプレイヤーのみ実行
			if (player[j].isActive == FALSE) {
				continue;
			}

			BOOL collision = CollisionBB(g_Item[i].pos, g_ItemData[type].colliderW, g_ItemData[type].colliderH,
				player[j].pos, player[j].colliderW, player[j].colliderH);

			// 衝突していなければ実行しない
			if (collision == FALSE) {
				continue;
			}
							
			// 消滅
			g_Item[i].isActive = FALSE;

			// アイテムによって分岐
			switch (g_Item[i].type) {

			case ITEM_TYPE_HEART:
				AddPlayerHP(j, HEAL_NUM);
				SetEffect(EFFECT_TYPE_HEAL, g_Item[i].pos, 1);
				PlaySound(SOUND_LABEL_SE_HEAL);
				break;

			case ITEM_TYPE_POWER:
				AddPlayerPower(j, 1);
				SetEffect(EFFECT_TYPE_POWERUP, g_Item[i].pos, 2);
				PlaySound(SOUND_LABEL_SE_POWER_UP);
				break;
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawItem(void)
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
	for (int i = 0; i < ITEM_MAX; i++) {

		// 無効なら実行しない
		if (g_Item[i].isActive == FALSE) {
			continue;
		}

		int type = g_Item[i].type;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[type]);

		float px = g_Item[i].pos.x - bg->pos.x;			// BGを追従
		float py = g_Item[i].pos.y - bg->pos.y;			//
		float pw = g_Item[i].w;
		float ph = g_Item[i].h;

		float tw = 1.0f / g_ItemData[type].patternDivideX;	// テクスチャの幅
		float th = 1.0f / g_ItemData[type].patternDivideY;	// テクスチャの高さ
		float tx = (float)(g_Item[i].animPattern % g_ItemData[type].patternDivideX) * tw;	// テクスチャの左上X座標
		float ty = (float)(g_Item[i].animPattern / g_ItemData[type].patternDivideX) * th;	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}

//=============================================================================
// アイテムをセット
//=============================================================================
void SetItem(int type, XMFLOAT3 pos) {

	for (int i = 0; i < ITEM_MAX; i++) {

		// 未使用スロットを検索
		if (g_Item[i].isActive == TRUE) {
			continue;
		}

		g_Item[i].isActive = TRUE;
		g_Item[i].w = g_ItemData[type].w;
		g_Item[i].h = g_ItemData[type].h;

		g_Item[i].pos = pos;
		g_Item[i].type = type;

		g_Item[i].speed = g_ItemData[type].speed;
		// 上下ブレを付ける
		g_Item[i].speed.y = rand() / RAND_MAX * SPEED_RANGE; 


		g_Item[i].animFrameCnt = 0;
		g_Item[i].animPattern = 0;

		g_Item[i].colliderW = g_ItemData[type].colliderW;
		g_Item[i].colliderH = g_ItemData[type].colliderH;

		// 1個セットしたら終了
		return;

	}
}