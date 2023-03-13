//=============================================================================
//
// エネミー処理 [enemy.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "enemy.h"
#include "bg.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "bullet.h"
#include "math.h"
#include "UI.h"
#include "score.h"
#include "sound.h"
#include "effect.h"
#include "item.h"
#include "slow.h"
#include "bossHp.h"
#include "waveController.h"
#include "fade.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	TEXTURE_MAX					ENEMY_TYPE_MAX

#define	BOSS_IDLING_TIME			480.0f					// 浮遊0.5周のフレーム数



// バレット発射番号
enum {

	ENEMY_SHOOT_TYPE_STRAIGHT,			// 真っ直ぐ撃つ
	ENEMY_SHOOT_TYPE_AIM,				// プレイヤーに向かって撃つ
	ENEMY_SHOOT_TYPE_BARRAGE00,			// 雑魚の5方向弾幕

#ifdef BEZIER
	ENEMY_SHOOT_TYPE_BEZIER,			// 曲線追尾
#endif

	ENEMY_ATTACK_TYPE_MAX,
};


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ

static ENEMY	g_Enemy[ENEMY_MAX];		// エネミースロット

static float	g_BossMovingCnt;					// ボス移動カウンタ
static float	g_BossAttackCnt;					// ボス攻撃カウンタ

static BOOL		g_IsTutorial;						// チュートリアルフラグ

static int		g_BossAction;						// ボス行動ラベル
static int		g_LastAction;						// ひとつ前の行動

static int		g_BossActionFrame[BOSS_MAX] = {		// ボス各行動の実行フレーム数

	750,	// 登場時間(BGMに合わせて)(12.5秒)

	480,
	480,

	240,

	0,

	300,	// 撃沈アニメ時間
};

static XMFLOAT3 g_BossSpawningSpeed;				// 最大登場速度

static XMFLOAT3	g_BossIdlingSpeed;					// 最大浮遊速度
static BOOL		g_IsFirstIdling;					// 登場直後の浮遊？

static XMFLOAT3 g_BossSinkingSpeed;					// 撃沈速度
static XMFLOAT3 g_SinkingExplosionDiff;				// 撃沈エフェクトの最大距離

static float	g_SinkingCnt;		// 撃沈ブレ用カウンタ
static XMFLOAT3 g_SinkingPos;		// 撃沈ブレ描画用座標
static XMFLOAT3 g_SinkingDiff;		// 撃沈ブレの最大距離
static int		g_SinkingSoundCnt;	// 撃沈SE用フラグ

static float	g_BossBarrageAngle;					// ボス弾幕角度記録用

static int		g_BossShotWait[BOSS_MAX] = {		// ボスの攻撃ウェイト値
	0,

	20,
	10,

	0,
	0,

	20,		// 撃沈爆発エフェクトのセット間隔
};

static ENEMY_DATA g_EnemyData[ENEMY_TYPE_MAX] = {

#ifndef BEZIER


	{
		"data/TEXTURE/enemy00.png",						// テクスチャ名
		XMINT2(200, 200),								// サイズ
		BULLET_TYPE_ENEMY_NORMAL,						// バレットタイプ
		ENEMY_SHOOT_TYPE_STRAIGHT, 60,					// 発射パターン, 弾発射ウェイト
		XMINT2(2, 1), 2, 4,								// 分割数, パターン数, ウェイト
		160, 80,										// 当たり判定
		4, 5000, 500,									// HP, 撃破得点, 命中得点
		10,												// ドロップ率
	},

#else

	{
		"data/TEXTURE/enemy00.png",						// テクスチャ名
		XMINT2(200, 200),								// サイズ
		BULLET_TYPE_ENEMY_BEZIER,						// バレットタイプ
		ENEMY_SHOOT_TYPE_BEZIER,	60,					// 発射パターン, 弾発射ウェイト
		XMINT2(2, 1), 2, 4,								// 分割数, パターン数, ウェイト
		160, 80,										// 当たり判定
		5, 5000, 500,									// HP, 撃破得点, 命中得点
		10,												// ドロップ率
	},

#endif

	{
		"data/TEXTURE/enemy01.png",
		XMINT2(200, 200),
		BULLET_TYPE_ENEMY_NORMAL,
		ENEMY_SHOOT_TYPE_AIM, 60,
		XMINT2(2, 1), 2, 4,
		150, 150,
		7, 10000, 1000,
		40,
	},

	{
		"data/TEXTURE/enemy02.png",
		XMINT2(300, 300),
		BULLET_TYPE_ENEMY_SLOW,
		ENEMY_SHOOT_TYPE_BARRAGE00, 60,
		XMINT2(2, 1), 2, 4,
		280, 280,
		12, 30000, 1500,
		80,
	},

	// BOSS
	{
		"data/TEXTURE/enemy_boss.png",
		XMINT2(1100, 1100),
		BULLET_TYPE_ENEMY_NORMAL,
		ENEMY_SHOOT_TYPE_BARRAGE00, 0,
		XMINT2(2, 1), 2, 4,
		900, 400,
		BOSS_HP_MAX, 100000, 500,
		0,
	},

	{
		"data/TEXTURE/enemy02.png",
		XMINT2(300, 300),
		BULLET_TYPE_ENEMY_SLOW,
		ENEMY_SHOOT_TYPE_BARRAGE00, 60,
		XMINT2(2, 1), 2, 4,
		280, 280,
		25, 30000, 1500,
		100,
	},
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_EnemyData[i].textureName,
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


	// エネミー構造体の初期化
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		g_Enemy[i].isActive = FALSE;
		g_Enemy[i].pos = XMFLOAT3(200.0f + i*200.0f, 100.0f, 0.0f);	// 中心点から表示
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].damagedAnimFrameCnt = 0;
		g_Enemy[i].damagedAnimPattern = 0;

		g_Enemy[i].isDamaged = FALSE;
	}


	// ボスの初期化
	g_BossAction = BOSS_SPAWNING;
	g_BossMovingCnt = 0.0f;
	g_BossAttackCnt = 0.0f;
	g_BossSpawningSpeed = XMFLOAT3(-4.0f, 0.0f, 0.0f);		// 最大登場速度
	g_BossIdlingSpeed = XMFLOAT3(0.0f, 1.0f, 0.0f);			// 最大浮遊速度
	g_IsFirstIdling = FALSE;
	g_BossBarrageAngle = 0;
	
	g_BossSinkingSpeed = XMFLOAT3(-0.8f, 1.6f, 0.0f);			// 撃沈速度
	g_SinkingExplosionDiff = XMFLOAT3(128.0f, 256.0f, 0.0f);	// 撃沈爆発の最大距離
	g_SinkingCnt = 0.0f;
	g_SinkingDiff = XMFLOAT3(6.0f, 6.0f, 0.0f);		// 撃沈ブレの最大距離
	g_SinkingSoundCnt = 0;

	g_IsTutorial = TRUE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
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
void UpdateEnemy(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		// 非アクティブは飛ばす
		if (g_Enemy[i].isActive == FALSE) {
			continue;
		}

		int type = g_Enemy[i].type;					// エネミータイプ



		/////////////////////////////////////////////////////////////////////////////
		// 移動処理
		/////////////////////////////////////////////////////////////////////////////
		// 雑魚
		if(type != ENEMY_TYPE_BOSS) {

			XMVECTOR xmvPos = XMLoadFloat3(&g_Enemy[i].pos);
			XMVECTOR xmvSpeed = XMLoadFloat3(&g_Enemy[i].speed);
			XMStoreFloat3(&g_Enemy[i].pos, xmvPos + xmvSpeed);

			// 画面左端を出たら無効化(半分突き出して)
			if (g_Enemy[i].pos.x < -g_Enemy[i].w * 0.5f) {
				g_Enemy[i].isActive = FALSE;
				continue;
			}
		}

		// ボスのみ
		else {

			// カウンタを進める
			g_BossMovingCnt++;

			switch (g_BossAction) {

			case BOSS_SPAWNING:			// 登場
			{
				// 最大登場スピードを取得
				float speed = g_BossSpawningSpeed.x;

				// グラフをかける( y=-(x^3)+1 )
				float ratio = g_BossMovingCnt / g_BossActionFrame[BOSS_SPAWNING];
				float graph = 1.0f - ratio * ratio * ratio;
				speed *= graph;

				// 移動
				g_Enemy[i].pos.x += speed;

				if (g_BossMovingCnt >= g_BossActionFrame[BOSS_SPAWNING]) {
					g_BossMovingCnt = 0;			// カウンタをクリア
					g_BossAttackCnt = 0;			//
					SetBossAction(BOSS_BARRAGE);	// 次の行動へ
					g_IsFirstIdling = TRUE;			// 登場直後浮遊をセット
				}
				break;
			}

			case BOSS_BARRAGE:			// 上下浮遊
			case BOSS_HOMING:			//
			case BOSS_IDLING:			//
			case BOSS_CALLING:			//
			{
				// 最大浮遊速度を取得
				float speed = g_BossIdlingSpeed.y;

				// 登場直後？
				if (g_IsFirstIdling == TRUE) {
					// グラフをかける( y=-((4x - 1)^2)+1 )
					float ratio = g_BossMovingCnt / BOSS_IDLING_TIME;
					float graph = 1.0f - (4.0f * ratio - 1.0f) * (4.0f * ratio - 1.0f);

					// 移動
					g_Enemy[i].pos.y += speed * graph;

					if (g_BossMovingCnt >= BOSS_IDLING_TIME * 0.5f) {		// 時間の半分
						g_BossMovingCnt = 0;								// カウンタをクリア
						g_BossIdlingSpeed.y = -g_BossIdlingSpeed.y;	// 方向を反転
						g_IsFirstIdling = FALSE;
					}
				}
				else {
					// グラフをかける( y=-((2x - 1)^2)+1 )
					float ratio = g_BossMovingCnt / BOSS_IDLING_TIME;
					float graph = 1.0f - (2.0f * ratio - 1.0f) * (2.0f * ratio - 1.0f);

					// 移動
					g_Enemy[i].pos.y += speed * graph;

					if (g_BossMovingCnt >= BOSS_IDLING_TIME) {
						g_BossMovingCnt = 0;								// カウンタをクリア
						g_BossIdlingSpeed.y = -g_BossIdlingSpeed.y;	// 方向を反転
					}
				}
				break;
			}

			case BOSS_SINKING:			// 撃沈
			{
				// 移動
				g_Enemy[i].pos.x += g_BossSinkingSpeed.x;
				g_Enemy[i].pos.y += g_BossSinkingSpeed.y;

				///////////////////////////////////////////////////////////////////////////
				// 撃沈ブレを計算
				///////////////////////////////////////////////////////////////////////////
				// カウンタを進める
				g_SinkingCnt++;
				// ブレる方向を決める
				XMFLOAT3 vec = XMFLOAT3(
					g_SinkingDiff.x * cos(g_SinkingCnt),
					g_SinkingDiff.x * sin(g_SinkingCnt),
					0.0f);

				// 最終座標を計算
				XMVECTOR xmvVec = XMLoadFloat3(&vec);
				XMVECTOR xmvPos = XMLoadFloat3(&g_Enemy[i].pos);
				XMStoreFloat3(&g_SinkingPos, xmvVec + xmvPos);

				if (g_BossMovingCnt >= g_BossActionFrame[BOSS_SINKING] ||
					GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0)) {	// ENTERキーでスキップ
					// ボスを消す
					g_Enemy[i].isActive = FALSE;
					//リザルトモードへ
					SetFade(FADE_OUT, MODE_RESULT);
				}
				break;
			}
			}
		}


		/////////////////////////////////////////////////////////////////////////////
		// 被弾エフェクト
		/////////////////////////////////////////////////////////////////////////////
		// 被弾した直後？
		if (g_Enemy[i].isDamaged == TRUE) {

			g_Enemy[i].damagedAnimFrameCnt++;

			g_Enemy[i].damagedAnimPattern = 1;					// パターンを変える

			// wait値経てば終了
			if (g_Enemy[i].damagedAnimFrameCnt > g_EnemyData[type].wait) {

				g_Enemy[i].damagedAnimFrameCnt = 0.0f;

				g_Enemy[i].damagedAnimPattern = 0;				// パターンを戻す
				g_Enemy[i].isDamaged = FALSE;			// 被弾エフェクト終了

			}
		}


		/////////////////////////////////////////////////////////////////////////////
		// 攻撃処理
		/////////////////////////////////////////////////////////////////////////////
		g_Enemy[i].shootCnt++;			// カウンタを進める

		// ボス以外
		if (type != ENEMY_TYPE_BOSS) {
			switch (g_EnemyData[type].shootType) {

			case ENEMY_SHOOT_TYPE_STRAIGHT:

				if (g_Enemy[i].shootCnt >= g_EnemyData[type].shootWait) {		// ウェイトに達した？
					g_Enemy[i].shootCnt = 0;
					SetBullet(g_EnemyData[type].bulletType, g_Enemy[i].pos);
				}
				break;

			case ENEMY_SHOOT_TYPE_AIM:

				if (g_Enemy[i].shootCnt >= g_EnemyData[type].shootWait) {		// ウェイトに達した？
					g_Enemy[i].shootCnt = 0;
					PLAYER* player = GetPlayer();
					BULLET_DATA* bulletData = GetBulletData();

					XMVECTOR xmvTargetSpeed = XMLoadFloat3(&player[0].pos) - XMLoadFloat3(&g_Enemy[i].pos);
					XMVECTOR xmvBulletSpeed = XMLoadFloat3(&bulletData[g_EnemyData[type].bulletType].speed);

					// 長さを求める
					XMVECTOR xmvBulletLength = XMVector3Length(xmvBulletSpeed);
					XMVECTOR xmvTargetLength = XMVector3Length(xmvTargetSpeed);

					XMVECTOR xmvResult = xmvTargetSpeed * (xmvBulletLength / xmvTargetLength);
					XMFLOAT3 result;
					XMStoreFloat3(&result, xmvResult);

					SetBullet(g_EnemyData[type].bulletType, g_Enemy[i].pos, result);
				}
				break;

			case ENEMY_SHOOT_TYPE_BARRAGE00:

				if (g_Enemy[i].shootCnt >= g_EnemyData[type].shootWait) {		// ウェイトに達した？
					g_Enemy[i].shootCnt = 0;

					g_Enemy[i].shootPattern = (g_Enemy[i].shootPattern + 1) % 2;	// パターンを変える

					if (g_Enemy[i].shootPattern == 0) {
						SetBarrage(BULLET_TYPE_ENEMY_NORMAL, g_Enemy[i].pos, 30.0f, 0.0f);		// 
					}
					else {
						SetBarrage(BULLET_TYPE_ENEMY_NORMAL, g_Enemy[i].pos, 30.0f, 15.0f);		// ちょっとズレる
					}

				}
				break;


#ifdef BEZIER

			case ENEMY_SHOOT_TYPE_BEZIER:

				if (g_Enemy[i].shootCnt >= g_EnemyData[type].shootWait) {		// ウェイトに達した？
					g_Enemy[i].shootCnt = 0;

					SetBezierBullet(BULLET_TYPE_ENEMY_BEZIER, g_Enemy[i].pos, GetPlayer()->pos, 120.0f);
				}

				break;

#endif

			}
		}

		// ボス
		else {
			g_BossAttackCnt++;		// カウンタを進める

			if (g_BossAction == BOSS_BARRAGE) {
				if (g_Enemy[i].shootCnt >= g_BossShotWait[g_BossAction]) {
					g_Enemy[i].shootCnt = 0;
					SetBarrage(BULLET_TYPE_ENEMY_NORMAL, g_Enemy[i].pos, 20.0f, g_BossBarrageAngle);
					g_BossBarrageAngle += 15.0f;
				}

				if (g_BossAttackCnt >= g_BossActionFrame[g_BossAction]) {
					g_BossAttackCnt = 0;
					// モード切替
					g_LastAction = g_BossAction;
					SetBossAction(BOSS_IDLING);
				}
			}
			else if (g_BossAction == BOSS_HOMING) {
				if (g_Enemy[i].shootCnt >= g_BossShotWait[g_BossAction]) {
					g_Enemy[i].shootCnt = 0;

					PLAYER* player = GetPlayer();
					BULLET_DATA* bulletData = GetBulletData();

					XMVECTOR xmvTargetSpeed = XMLoadFloat3(&player[0].pos) - XMLoadFloat3(&g_Enemy[i].pos);
					XMVECTOR xmvBulletSpeed = XMLoadFloat3(&bulletData[g_EnemyData[type].bulletType].speed);

					// 長さを求める
					XMVECTOR xmvBulletLength = XMVector3Length(xmvBulletSpeed);
					XMVECTOR xmvTargetLength = XMVector3Length(xmvTargetSpeed);

					XMVECTOR xmvResult = xmvTargetSpeed * (xmvBulletLength / xmvTargetLength);
					XMFLOAT3 result;
					XMStoreFloat3(&result, xmvResult);

					// 1回弾をセット
					SetBullet(g_EnemyData[type].bulletType, g_Enemy[i].pos, result);

					// resultの法線ベクトルを取得(長さがxmvBulletLength)
					XMFLOAT3 normal = XMFLOAT3(result.y, -result.x, 0.0f);
					XMVECTOR xmvNormal = XMLoadFloat3(&normal);

					// 単位ベクトルにする
					xmvNormal /= xmvBulletLength;

					// 間隔を求める(弾の幅の何倍)
					float width = bulletData[g_EnemyData[type].bulletType].size.x;
					width *= 1.0f;

					// 発射座標を求める
					XMFLOAT3 newPos = XMFLOAT3(
						g_Enemy[i].pos.x + xmvNormal.m128_f32[0] * width,
						g_Enemy[i].pos.y + xmvNormal.m128_f32[1] * width,
						0.0f);

					// 2回目セット
					SetBullet(g_EnemyData[type].bulletType, newPos, result);

					// 発射座標を求める
					newPos = XMFLOAT3(
						g_Enemy[i].pos.x - xmvNormal.m128_f32[0] * width,
						g_Enemy[i].pos.y - xmvNormal.m128_f32[1] * width,
						0.0f);

					// 3回目セット
					SetBullet(g_EnemyData[type].bulletType, newPos, result);
				}

				if (g_BossAttackCnt >= g_BossActionFrame[g_BossAction]) {
					g_BossAttackCnt = 0;
					// モード切替
					g_LastAction = g_BossAction;
					SetBossAction(BOSS_CALLING);		// 雑魚を呼ぶ
				}
			}
			else if (g_BossAction == BOSS_IDLING) {
				if (g_BossAttackCnt >= g_BossActionFrame[g_BossAction]) {
					g_BossAttackCnt = 0;

					// 攻撃か必殺か、順番に切り替える
					if (g_LastAction == BOSS_BARRAGE) {
						g_LastAction = g_BossAction;
						SetBossAction(BOSS_HOMING);
					}
					else {
						g_LastAction = g_BossAction;
						SetBossAction(BOSS_BARRAGE);
					}
				}
			}
			else if (g_BossAction == BOSS_CALLING) {
				g_Enemy[i].shootCnt = 0;
				g_BossAttackCnt = 0;
				// 上と下を通る
				SetEnemy(ENEMY_TYPE03, XMFLOAT3(1.1f, 0.2f, 0.0f), XMFLOAT3(-1.2f, 0.0f, 0.0f));
				SetEnemy(ENEMY_TYPE03, XMFLOAT3(1.1f, 0.8f, 0.0f), XMFLOAT3(-1.2f, 0.0f, 0.0f));

				// モード切替(記録はしない)
				SetBossAction(BOSS_IDLING);
			}
			else if (g_BossAction == BOSS_SINKING) {	// 撃沈：爆発エフェクトを大量にセット

				if (g_Enemy[i].shootCnt >= g_BossShotWait[g_BossAction]) {
					g_Enemy[i].shootCnt = 0;

					// セットする距離と方向をランダムに決める
					XMFLOAT3 diff = XMFLOAT3(
						g_SinkingExplosionDiff.x * sinf(rand()),
						g_SinkingExplosionDiff.y * sinf(rand()),
						0.0f);
					XMVECTOR xmvDiff = XMLoadFloat3(&diff);

					// 座標を決める
					XMVECTOR xmvPos = XMLoadFloat3(&g_Enemy[i].pos);
					XMFLOAT3 result;
					XMStoreFloat3(&result, xmvPos + xmvDiff);

					// エフェクトをセット
					SetEffect(EFFECT_TYPE_BOSS_EXPLOSION, result, 2);

					// 爆発SEは3回に1回再生
					g_SinkingSoundCnt = (g_SinkingSoundCnt + 1) % 3;
					if(g_SinkingSoundCnt == 0)
						PlaySound(SOUND_LABEL_SE_EXPLOSION002);
				}
			}
		}
	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
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
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].isActive == TRUE)			// このエネミーが使われている？
		{
			int type = g_Enemy[i].type;

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy[i].type]);

			// typeがボスかつボスが撃沈モードであれば
			if (type == ENEMY_TYPE_BOSS && g_BossAction == BOSS_SINKING) {

				float px = g_SinkingPos.x - bg->pos.x;	// 撃沈地点とBGを追従
				float py = g_SinkingPos.y - bg->pos.y;	// 
				float pw = g_Enemy[i].w;
				float ph = g_Enemy[i].h;

				float tw = 1.0f / g_EnemyData[type].patternDivide.x;	// テクスチャの幅
				float th = 1.0f / g_EnemyData[type].patternDivide.y;	// テクスチャの高さ
				float tx = (float)(g_Enemy[i].damagedAnimPattern % g_EnemyData[type].patternDivide.x) * tw;	// テクスチャの左上X座標
				float ty = (float)(g_Enemy[i].damagedAnimPattern / g_EnemyData[type].patternDivide.x) * th;	// テクスチャの左上Y座標

				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Enemy[i].rot.z);
			}
			else {

				float px = g_Enemy[i].pos.x - bg->pos.x;	// BGを追従
				float py = g_Enemy[i].pos.y - bg->pos.y;	// 
				float pw = g_Enemy[i].w;
				float ph = g_Enemy[i].h;

				float tw = 1.0f / g_EnemyData[type].patternDivide.x;	// テクスチャの幅
				float th = 1.0f / g_EnemyData[type].patternDivide.y;	// テクスチャの高さ
				float tx = (float)(g_Enemy[i].damagedAnimPattern % g_EnemyData[type].patternDivide.x) * tw;	// テクスチャの左上X座標
				float ty = (float)(g_Enemy[i].damagedAnimPattern / g_EnemyData[type].patternDivide.x) * th;	// テクスチャの左上Y座標

				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Enemy[i].rot.z);
			}

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}

//=============================================================================
// Enemy構造体の先頭アドレスを取得
//=============================================================================
ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}

//=============================================================================
// エネミーをセット
//=============================================================================
void SetEnemy(int type, XMFLOAT3 perPos, XMFLOAT3 speed) {

	for (int i = 0; i < ENEMY_MAX; i++) {
		
		// 未使用スロットを検索
		if (g_Enemy[i].isActive == TRUE) {
			continue;
		}

		g_Enemy[i].isActive = TRUE;
		g_Enemy[i].w = g_EnemyData[type].size.x;
		g_Enemy[i].h = g_EnemyData[type].size.y;

		// 座標を変換
		{
			XMFLOAT3 pos = XMFLOAT3(perPos.x * SCREEN_WIDTH, perPos.y * SCREEN_HEIGHT, perPos.z);
			g_Enemy[i].pos = pos;

		}

		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].type = type;

		g_Enemy[i].shootCnt = 0;
		g_Enemy[i].shootPattern = 0;

		g_Enemy[i].speed = speed;

		g_Enemy[i].damagedAnimFrameCnt = 0;
		g_Enemy[i].damagedAnimPattern = 0;

		g_Enemy[i].colliderW = g_EnemyData[type].colliderW;
		g_Enemy[i].colliderH = g_EnemyData[type].colliderH;

		// BOSS以外はプレイヤー火力レベルに応じて敵HPが増減
		if(type != ENEMY_TYPE_BOSS)
		{
			PLAYER* player = GetPlayer();

			// HPが√火力レベルと比例
			float rate = sqrt((float)player->level + 1.0f);
			g_Enemy[i].hp = (float)g_EnemyData[type].hp * rate;
		}
		else {
			g_Enemy[i].hp = g_EnemyData[type].hp;
		}

		g_Enemy[i].killScore = g_EnemyData[type].killScore;
		g_Enemy[i].hitScore = g_EnemyData[type].hitScore;

		g_Enemy[i].dropRate = g_EnemyData[type].dropRate;


		// ボスの初期化
		if (type == ENEMY_TYPE_BOSS) {
			// HPを表示
			SetBossHp();
			// CUTINをセット
			SetCutin(TRUE);
			// 曲を止める
			StopSound(SOUND_LABEL_BGM_GAME);
			// SEをプレイ
			PlaySound(SOUND_LABEL_SE_CUTIN000);
			PlaySound(SOUND_LABEL_SE_CUTIN001);

			// ゲームフェースをセット
			SetGamePhase(PHASE_BOSS);
		}

		return;			// 1体セットしたら終了
	}
}

//=============================================================================
// エネミーがUIに遮られたか
//=============================================================================
BOOL IsEnemyInsideUI(void) {

	UI* UI = GetUI();

	for (int i = 0; i < ENEMY_MAX; i++) {

		if (g_Enemy[i].isActive == TRUE) {				// 有効プレイヤーのみチェック

			// Y値をチェック
			float enemyY = g_Enemy[i].pos.y;
			float UIY = UI->pos.y + UI->h + UI->extraRange;

			if (enemyY < UIY) {
				return TRUE;		// 1人でも中にいれば
			}

		}

	}

	return FALSE;					// 全員外にいれば

}

//=============================================================================
// HP増減と死亡処理
//=============================================================================
void AddEnemyHP(int enemy, int add) {

	g_Enemy[enemy].hp += add;

	// エネミー生きてる？
	if (g_Enemy[enemy].hp > 0) {

		AddScore(g_Enemy[enemy].hitScore);							// 加点
		g_Enemy[enemy].isDamaged = TRUE;							// 被弾アニメーションをプレイ
		PlaySound(SOUND_LABEL_SE_ENEMY_HIT);						// 被弾音をプレイ
		return;
	}

	//////////////////////////////////////////////////////////////////////////////
	// 死んだ場合の処理
	//////////////////////////////////////////////////////////////////////////////
	AddScore(g_Enemy[enemy].killScore);								// 加点
	PlaySound(SOUND_LABEL_SE_EXPLOSION000 + rand() % 2);			// 爆散音をランダムにプレイ

	// エネミーによって実行
	switch (g_Enemy[enemy].type) {

	case ENEMY_TYPE00:
		g_Enemy[enemy].isActive = FALSE;
		SetEffect(EFFECT_TYPE_ENEMY_EXPLOSION, g_Enemy[enemy].pos, 1);	// 爆散エフェクト
		if (rand() % 100 < g_Enemy[enemy].dropRate) {
			SetItem(ITEM_TYPE_HEART, g_Enemy[enemy].pos);				// ハートドロップ
		}
		break;

	case ENEMY_TYPE01:
		g_Enemy[enemy].isActive = FALSE;
		SetEffect(EFFECT_TYPE_ENEMY_EXPLOSION, g_Enemy[enemy].pos, 1);	// 爆散エフェクト
		// ドロップ率により
		if (rand() % 100 < g_Enemy[enemy].dropRate) {

			if (rand() % 100 < 70)
				SetItem(ITEM_TYPE_HEART, g_Enemy[enemy].pos);				// ハートドロップ

			else
				SetItem(ITEM_TYPE_POWER, g_Enemy[enemy].pos);				// パワードロップ
		}
		break;

	case ENEMY_TYPE02:
		g_Enemy[enemy].isActive = FALSE;
		SetEffect(EFFECT_TYPE_ENEMY_EXPLOSION, g_Enemy[enemy].pos, 3);	// 伸びた爆散エフェクト

		// ドロップ率により
		if (rand() % 100 < g_Enemy[enemy].dropRate) {

			if (rand() % 100 < 20)
				SetItem(ITEM_TYPE_HEART, g_Enemy[enemy].pos);				// ハートドロップ

			else
				SetItem(ITEM_TYPE_POWER, g_Enemy[enemy].pos);				// パワードロップ
		}
		break;

	case ENEMY_TYPE03:
		g_Enemy[enemy].isActive = FALSE;
		SetEffect(EFFECT_TYPE_ENEMY_EXPLOSION, g_Enemy[enemy].pos, 3);	// 伸びた爆散エフェクト

		// ドロップ率により
 		if (rand() % 100 < g_Enemy[enemy].dropRate) {

			if (rand() % 100 < 50)
				SetItem(ITEM_TYPE_HEART, g_Enemy[enemy].pos);				// ハートドロップ

			else
				SetItem(ITEM_TYPE_POWER, g_Enemy[enemy].pos);				// パワードロップ
		}
		break;

	case ENEMY_TYPE_BOSS:
		// ゲームセット
		SetGamePhase(PHASE_ENTRO);
		// スローをかける
		SetSlow(SLOW_BOSS_DIE, 120);
		// BGMをとめる
		StopSound(SOUND_LABEL_BGM_BOSS);
		break;
	}

}

//=============================================================================
// エネミーを瞬殺する
//=============================================================================
void KillEnemy(int enemy) {
	AddEnemyHP(enemy, -99999);
}

//=============================================================================
// ボス行動をセット
//=============================================================================
void SetBossAction(int mode) {
	g_BossAction = mode;

	// 撃沈モードに入る準備
	if (mode == BOSS_SINKING) {
		g_BossMovingCnt = 0;	// パラメータークリア
	}
}