//=============================================================================
//
// バレット処理 [bullet.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "bullet.h"
#include "collision.h"
#include "score.h"
#include "bg.h"
#include "effect.h"
#include "sound.h"
#include "slow.h"
#include "item.h"
#include "waveController.h"
#include "math.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define			TEXTURE_MAX			BULLET_TYPE_MAX

#define			DELAY				8						// 生成時描画遅延

#define			BEZIER_OFFSET		500						// ベジェ曲線の制御点のズレ具合

// 描画モード番号
enum {

	DRAW_DEFAULT,				// デフォルト
	DRAW_ADD,					// 加算

	DRAW_MAX,
};


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static BULLET_DATA g_BulletData[TEXTURE_MAX] = {

	{	// プライヤー通常弾
		"data/TEXTURE/bullet00.png",							// テクスチャ名
		XMINT2(50, 50), DRAW_DEFAULT, 1.0f,						// キャラサイズ, 描画, α
		XMFLOAT3(60.0f, 0.0f, 0.0f),							// スピード
		XMINT2(1, 1), 1, 4,										// 分割数, パターン数, ウェイト
		40, 40,													// 当たり判定
	},

	{	// エネミー通常弾
		"data/TEXTURE/bullet01.png",
		XMINT2(50, 50), DRAW_DEFAULT, 1.0f,
		XMFLOAT3(-6.5f, 0.0f, 0.0f),
		XMINT2(1, 1), 1, 4,
		20, 20,
	},

	{	// エネミースロー弾
		"data/TEXTURE/bullet01.png",
		XMINT2(50, 50), DRAW_DEFAULT, 1.0f,
		XMFLOAT3(-5.0f, 0.0f, 0.0f),
		XMINT2(1, 1), 1, 4,
		20, 20,
	},

#ifdef BEZIER
	{	// エネミーベジェ弾
		"data/TEXTURE/bullet01.png",
		XMINT2(50, 50), DRAW_DEFAULT, 1.0f,
		XMFLOAT3(0.0f, 0.0f, 0.0f),			// スピードは参照しない
		XMINT2(1, 1), 1, 4,
		20, 20,
	},
#endif

};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static BULLET	g_Bullet[BULLET_MAX];	// バレット構造体

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_BulletData[i].textureName,
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

	// バレット構造体の初期化
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].isActive   = FALSE;
		g_Bullet[i].isDelayed = FALSE;
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet(void)
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
void UpdateBullet(void)
{
	for (int i = 0; i < BULLET_MAX; i++) {

		// 無効なら実行しない
		if (g_Bullet[i].isActive == FALSE) {
			continue;
		}

		int type = g_Bullet[i].type;



		////////////////////////////////////////////////////////////////////////////
		// アニメーション
		////////////////////////////////////////////////////////////////////////////
		g_Bullet[i].animFrameCnt++;
		if ((g_Bullet[i].animFrameCnt % g_BulletData[type].wait) == 0)
		{
			// パターンの切り替え
			g_Bullet[i].animPattern = (g_Bullet[i].animPattern + 1) % g_BulletData[type].patternNum;
		}




		/////////////////////////////////////////////////////////////////////////////
		// 描画遅延制御
		/////////////////////////////////////////////////////////////////////////////
		if (g_Bullet[i].isDelayed == TRUE) {
			g_Bullet[i].delayCnt++;
			if (g_Bullet[i].delayCnt == DELAY) {
				g_Bullet[i].isDelayed = FALSE;
			}
		}

		////////////////////////////////////////////////////////////////////////////
		// 移動処理
		////////////////////////////////////////////////////////////////////////////
#ifdef BEZIER
		// ベジェ
		if (g_Bullet[i].type == BULLET_TYPE_ENEMY_BEZIER) {

			XMFLOAT3 p0, p1, p2;
			float t;

			p0 = g_Bullet[i].start;
			p1 = p0;
			p2 = g_Bullet[i].end;

			if (p0.y > p2.y) {
				p1.y += BEZIER_OFFSET;
			}
			else {
				p1.y -= BEZIER_OFFSET;
			}

			t = g_Bullet[i].bezierCnt / g_Bullet[i].bezierT;

			g_Bullet[i].pos = Bezier(p0, p1, p2, t);

			// カウンタを進める
			g_Bullet[i].bezierCnt++;
		}

		// 他
		else {
			XMVECTOR pos = XMLoadFloat3(&g_Bullet[i].pos);
			XMVECTOR speed = XMLoadFloat3(&g_Bullet[i].speed);
			pos += speed;
			XMStoreFloat3(&g_Bullet[i].pos, pos);

			// 画面を出た弾は消滅(弾の半分が出るまで)
			if (g_Bullet[i].pos.y < -g_Bullet[i].h * 0.5f) {
				g_Bullet[i].isActive = FALSE;
			}
			else if (g_Bullet[i].pos.y > SCREEN_HEIGHT + g_Bullet[i].h * 0.5f) {
				g_Bullet[i].isActive = FALSE;
			}
			if (g_Bullet[i].pos.x < -g_Bullet[i].w * 0.5f) {
				g_Bullet[i].isActive = FALSE;
			}
			else if (g_Bullet[i].pos.x > SCREEN_WIDTH + g_Bullet[i].w * 0.5f) {
				g_Bullet[i].isActive = FALSE;
			}

		}

#else
		XMVECTOR pos = XMLoadFloat3(&g_Bullet[i].pos);
		XMVECTOR speed = XMLoadFloat3(&g_Bullet[i].speed);
		pos += speed;
		XMStoreFloat3(&g_Bullet[i].pos, pos);

		// 画面を出た弾は消滅(弾の半分が出るまで)
		if (g_Bullet[i].pos.y < -g_Bullet[i].h * 0.5f) {
			g_Bullet[i].isActive = FALSE;
		}
		else if (g_Bullet[i].pos.y > SCREEN_HEIGHT + g_Bullet[i].h * 0.5f) {
			g_Bullet[i].isActive = FALSE;
		}
		if (g_Bullet[i].pos.x < -g_Bullet[i].w * 0.5f) {
			g_Bullet[i].isActive = FALSE;
		}
		else if (g_Bullet[i].pos.x > SCREEN_WIDTH + g_Bullet[i].w * 0.5f) {
			g_Bullet[i].isActive = FALSE;
		}

#endif

		////////////////////////////////////////////////////////////////////////////
		// 当たり判定
		////////////////////////////////////////////////////////////////////////////
		if (GetGamePhase() == PHASE_ENTRO) {		// ゲームセットだったら当たり判定をしない
			continue;
		}

		// 弾→エネミー
		{
			ENEMY* enemy = GetEnemy();

			// エネミーの数分当たり判定を行う
			for (int j = 0; j < ENEMY_MAX; j++)
			{
				// 生きてるエネミーと当たり判定をする
				if (enemy[j].isActive == FALSE) {
					continue;
				}

				BOOL collision = CollisionBB(g_Bullet[i].pos, g_BulletData[type].colliderW, g_BulletData[type].colliderH,
					enemy[j].pos, enemy[j].colliderW, enemy[j].colliderH);

				if (collision == TRUE) {

					// プレイヤーの弾とそれ以外に分けて
					switch (g_Bullet[i].type) {

					case BULLET_TYPE_PLAYER_NORMAL:
						g_Bullet[i].isActive = FALSE;		// 弾を消す
						SetEffect(EFFECT_TYPE_ENEMY_HIT, g_Bullet[i].pos, 1);	// 被弾エフェクト
						AddEnemyHP(j, -1);					// エネミーにダメージ
						break;

					default:
						break;
					}
				}
			}
		}

		// 弾→プレイヤー
		{
			PLAYER* player = GetPlayer();

			for (int j = 0; j < PLAYER_MAX; j++)
			{
				// 生きてるプレイヤーと当たり判定をする
				if (player[j].isActive == TRUE)
				{
					BOOL collision = CollisionBB(g_Bullet[i].pos, g_BulletData[type].colliderW, g_BulletData[type].colliderH,
						player[j].pos, player[j].colliderW, player[j].colliderH);

					if (collision == TRUE) {

						// エネミーの弾とそれ以外に分けて
						switch (g_Bullet[i].type) {

						case BULLET_TYPE_ENEMY_NORMAL:
						case BULLET_TYPE_ENEMY_SLOW:
#ifdef BEZIER
						case BULLET_TYPE_ENEMY_BEZIER:
#endif
							g_Bullet[i].isActive = FALSE;		// 弾を消す

							// すでに被弾中じゃなければ
							if (player[j].isDamaged == FALSE) {
								PlaySound(SOUND_LABEL_SE_PLAYER_HIT);					// 被弾音をプレイ
								SetEffect(EFFECT_TYPE_PLAYER_HIT, g_Bullet[i].pos, 1);	// 被弾エフェクト

								AddPlayerHP(j, -1);				// プレイヤーにダメージ
							}
							break;

						default:
							break;
						}
					}
				}
			}
		}

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
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
	for (int i = 0; i < BULLET_MAX; i++) {

		// 無効なら実行しない
		if (g_Bullet[i].isActive == FALSE) {
			continue;
		}

		// 描画遅延？
		if (g_Bullet[i].isDelayed == TRUE) {
			continue;
		}



		int type = g_Bullet[i].type;

		////////////////////////////////////////////////////////////////////////////
		// DEFAULT
		////////////////////////////////////////////////////////////////////////////
		if (g_BulletData[type].mode == DRAW_DEFAULT) {

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[type]);

			//バレットの位置やテクスチャー座標を反映
			float px = g_Bullet[i].pos.x - bg->pos.x;		// BGを追従
			float py = g_Bullet[i].pos.y - bg->pos.y;		//
			float pw = g_Bullet[i].w;
			float ph = g_Bullet[i].h;

			float tw = 1.0f / g_BulletData[type].patternDivide.x;	// テクスチャの幅
			float th = 1.0f / g_BulletData[type].patternDivide.y;	// テクスチャの高さ
			float tx = (float)(g_Bullet[i].animPattern % g_BulletData[type].patternDivide.x) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Bullet[i].animPattern / g_BulletData[type].patternDivide.x) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, g_BulletData[type].alpha),
				g_Bullet[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

		////////////////////////////////////////////////////////////////////////////
		// ADD
		////////////////////////////////////////////////////////////////////////////
		else if (g_BulletData[type].mode == DRAW_ADD) {

			SetBlendState(BLEND_MODE_ADD);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[type]);

			//バレットの位置やテクスチャー座標を反映
			float px = g_Bullet[i].pos.x - bg->pos.x;
			float py = g_Bullet[i].pos.y - bg->pos.y;
			float pw = g_Bullet[i].w;
			float ph = g_Bullet[i].h;

			float tw = 1.0f / g_BulletData[type].patternDivide.x;	// テクスチャの幅
			float th = 1.0f / g_BulletData[type].patternDivide.y;	// テクスチャの高さ
			float tx = (float)(g_Bullet[i].animPattern % g_BulletData[type].patternDivide.x) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Bullet[i].animPattern / g_BulletData[type].patternDivide.x) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, g_BulletData[type].alpha),
				g_Bullet[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			SetBlendState(BLEND_MODE_ALPHABLEND);
		}
	}
}

//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
BULLET *GetBullet(void)
{
	return &g_Bullet[0];
}

//=============================================================================
// バレットデータ構造体の先頭アドレスを取得
//=============================================================================
BULLET_DATA* GetBulletData(void)
{
	return &g_BulletData[0];
}

//=============================================================================
// バレット発射(デフォルトスピードと方向)
//=============================================================================
void SetBullet(int type, XMFLOAT3 pos)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		// 未使用スロットを検索
		if (g_Bullet[i].isActive == TRUE) {
			continue;
		}

		g_Bullet[i].isActive = TRUE;
		g_Bullet[i].w = g_BulletData[type].size.x;
		g_Bullet[i].h = g_BulletData[type].size.y;

		g_Bullet[i].pos = pos;
		g_Bullet[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Bullet[i].type = type;
		g_Bullet[i].speed = g_BulletData[type].speed;

		g_Bullet[i].animFrameCnt = 0;
		g_Bullet[i].animPattern = 0;

		g_Bullet[i].colliderW = g_BulletData[type].colliderW;
		g_Bullet[i].colliderH = g_BulletData[type].colliderH;

		//g_Bullet[i].isDelayed = TRUE;
		//g_Bullet[i].delayCnt = 0;

		return;			// 1発セットできたら終了
	}
}

//=============================================================================
// バレット発射(スピードと方向指定)
//=============================================================================
void SetBullet(int type, XMFLOAT3 pos, XMFLOAT3 speed)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		// 未使用スロットを検索
		if (g_Bullet[i].isActive == TRUE) {
			continue;
		}

		g_Bullet[i].isActive = TRUE;
		g_Bullet[i].w = g_BulletData[type].size.x;
		g_Bullet[i].h = g_BulletData[type].size.y;

		g_Bullet[i].pos = pos;
		g_Bullet[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Bullet[i].type = type;
		g_Bullet[i].speed = speed;

		g_Bullet[i].animFrameCnt = 0;
		g_Bullet[i].animPattern = 0;

		g_Bullet[i].colliderW = g_BulletData[type].colliderW;
		g_Bullet[i].colliderH = g_BulletData[type].colliderH;

		//g_Bullet[i].isDelayed = TRUE;
		//g_Bullet[i].delayCnt = 0;

		return;			// 1発セットできたら終了
	}
}

//=============================================================================
// 円形弾幕生成
//=============================================================================
void SetBarrage(int type, XMFLOAT3 pos, float degree, float startDegree) {

	// 角度によって繰り返す回数を計算
	for (int i = 0; i < 360 / degree; i++) {

		// 今回の角度を決める
		float _degree = degree * i + startDegree;

		// 角度をラジアンに変換
		float rad = XMConvertToRadians(_degree);

		// ラジアンにより単位ベクトルを作成
		XMFLOAT3 unit = XMFLOAT3(cosf(rad), sinf(rad), 0.0f);
		XMVECTOR xmvUnit = XMLoadFloat3(&unit);

		// スピードの絶対値を取る
		float length;
		if (g_BulletData[type].speed.x < 0.0f) {
			length = -g_BulletData[type].speed.x;
		}
		else {
			length = g_BulletData[type].speed.x;
		}

		// 単位ベクトルを元のスピードの長さへ伸ばす
		XMVECTOR xmvLength = XMLoadFloat3(&XMFLOAT3(length, length, length));
		XMVECTOR xmvResult = xmvUnit * xmvLength;

		// ベクトルを元に戻す
		XMFLOAT3 result;
		XMStoreFloat3(&result, xmvResult);

		// 弾をセット
		SetBullet(type, pos, result);

	}

}

//=============================================================================
// 扇形弾幕生成
//=============================================================================
void SetBarrage(int type, XMFLOAT3 pos, float degree, float startDegree, float endDegree) {

	// 角度によって繰り返す回数を計算
	for (int i = 0; i < 360 / degree; i++) {

		// 今回の角度を決める
		float _degree = degree * i + startDegree;

		// endDegreeを越えれば終了
		if (_degree > endDegree) {
			return;
		}

		// 角度をラジアンに変換
		float rad = XMConvertToRadians(_degree);

		// ラジアンにより単位ベクトルを作成
		XMFLOAT3 unit = XMFLOAT3(cosf(rad), sinf(rad), 0.0f);
		XMVECTOR xmvUnit = XMLoadFloat3(&unit);

		// スピードの絶対値を取る
		float length;
		if (g_BulletData[type].speed.x < 0.0f) {
			length = -g_BulletData[type].speed.x;
		}
		else {
			length = g_BulletData[type].speed.x;
		}

		// 単位ベクトルを元のスピードの長さへ伸ばす
		XMVECTOR xmvLength = XMLoadFloat3(&XMFLOAT3(length, length, length));
		XMVECTOR xmvResult = xmvUnit * xmvLength;

		// ベクトルを元に戻す
		XMFLOAT3 result;
		XMStoreFloat3(&result, xmvResult);

		// 弾をセット
		SetBullet(type, pos, result);

	}

}










void SetBezierBullet(int type, XMFLOAT3 start, XMFLOAT3 end, float t) {

	for (int i = 0; i < BULLET_MAX; i++)
	{
		// 未使用スロットを検索
		if (g_Bullet[i].isActive == TRUE) {
			continue;
		}

		g_Bullet[i].isActive = TRUE;
		g_Bullet[i].w = g_BulletData[type].size.x;
		g_Bullet[i].h = g_BulletData[type].size.y;

		g_Bullet[i].pos = start;	// 初期位置
		g_Bullet[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);	// 使わない

		g_Bullet[i].type = type;

		g_Bullet[i].animFrameCnt = 0;
		g_Bullet[i].animPattern = 0;

		g_Bullet[i].colliderW = g_BulletData[type].colliderW;
		g_Bullet[i].colliderH = g_BulletData[type].colliderH;

		// ベジェ用
		g_Bullet[i].start = start;
		g_Bullet[i].end = end;
		g_Bullet[i].bezierT = t;
		g_Bullet[i].bezierCnt = 0.0f;

		return;			// 1発セットできたら終了
	}

}