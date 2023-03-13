//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : GP11B132 33 呉優
//
//=============================================================================
#include "player.h"
#include "input.h"
#include "bg.h"
#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "file.h"
#include "effect.h"
#include "sound.h"
#include "UI.h"
#include "debugproc.h"
#include "slow.h"
#include "waveController.h"
#include "fade.h"
#include "damage.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(180)	// キャラサイズ
#define TEXTURE_HEIGHT				(180)	// 
#define TEXTURE_MAX					(10)	// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(2)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// 総パターン数

// プレイヤーの画面内配置座標
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

#define	SHOOT_SE_RATE_000			(50)		// 発射SEの割合
#define	SHOOT_SE_RATE_001			(30)		//
#define	SHOOT_SE_RATE_002			(20)		// 使わない

#define	PLAYER_COLLIDER_WIDTH		50			// プレイヤーの当たり判定サイズ
#define	PLAYER_COLLIDER_HEIGHT		25			//

#define	DAMAGED_TIME				120.0f			// 無敵時間
#define	DAMAGED_WAIT				4				// 被弾アニメーションのウェイト値

#define	POPPED_TIME					40.0f			// 弾かれ時間

#define	HP_START					10				// スタートHP

#define	SHOOT_INTERVAL				10				// 連射間隔

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

// テクスチャファイル
static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/spaceship_player_sprite.png",
};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static PLAYER	g_Player[PLAYER_MAX];	// プレイヤー構造体

static int		g_PlayerAlive;			// 生存中プレイヤー数

static float	g_ShootSENode00;		// 発射SE判定用
static float	g_ShootSENode01;

static XMFLOAT3	g_ShootDiff = XMFLOAT3(20.0f, 0.0f, 0.0f);	// ブレッと発射地点のプレイヤーからのズレ
static XMFLOAT3	g_ShootEffectDiff = XMFLOAT3(90.0f, 0.0f, 0.0f);	// 発射SEのプレイヤーからのズレ

static int g_LevelUpCost[PLAYER_LEVEL_MAX - 1] = {		// レベルアップに必要なパワーアップ数
	1, 1, 2, 3
};

static BOOL	g_IsDeadCnting;	// 爆死カウントする？
static int	g_DeadCnt;		// プレイヤー爆死GAMEOVER後カウンタ
static int	g_DeadNum;		// 爆死後余韻時間の長さ

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
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


	// プレイヤー構造体の初期化
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_Player[i].isActive = FALSE;
		g_Player[i].pos = XMFLOAT3(400.0f, 400.0f, 0.0f);	// 中心点から表示
		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].w = TEXTURE_WIDTH;
		g_Player[i].h = TEXTURE_HEIGHT;
		g_Player[i].texNo = 0;

		g_Player[i].animFrameCnt = 0;
		g_Player[i].animPattern = 0;

		g_Player[i].speed = XMFLOAT3(10.0f, 0.0f, 0.0f);

		g_Player[i].dirV = 0;
		g_Player[i].dirH = CHAR_DIR_FORWARD;									// デフォルトは前向き
		g_Player[i].animPattern = g_Player[i].dirH * TEXTURE_PATTERN_DIVIDE_X;

		// HP初期化
		if (PLAYER_HP_MAX < HP_START) {	// HP上限が低い場合はHP上限を優先
			g_Player[i].hp = PLAYER_HP_MAX;
		}
		else {
			g_Player[i].hp = HP_START;
		}

		g_Player[i].colliderW = PLAYER_COLLIDER_WIDTH;
		g_Player[i].colliderH = PLAYER_COLLIDER_HEIGHT;
		g_Player[i].isDamaged = FALSE;
		g_Player[i].isAlphaChanged = FALSE;
		g_Player[i].damagedCnt = 0;

		g_Player[i].isPopped = FALSE;
		g_Player[i].poppedCnt = 0.0f;
		g_Player[i].poppedSpeed = XMFLOAT3(-30.0f, 3.0f, 0.0f);		// POPPED最大スピード

		g_Player[i].level = PLAYER_LEVEL00;		// スタートレベル
		g_Player[i].powerUp = 0;

		g_Player[i].shootCnt = 0;
	}

	g_ShootSENode00 = SHOOT_SE_RATE_000;
	g_ShootSENode01 = SHOOT_SE_RATE_000 + SHOOT_SE_RATE_001;

	// 1Pのみ有効化
	g_Player[0].isActive = TRUE;
	g_PlayerAlive = 1;

	g_IsDeadCnting = FALSE;
	g_DeadCnt = 0;
	g_DeadNum = 300;	// 爆死後余韻時間(スロー含め)

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void) {

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// 生きてるプレイヤーだけ処理をする
		if (g_Player[i].isActive == FALSE) {
			continue;
		}

		/////////////////////////////////////////////////////////////////////////////
		// 自滅処理
		/////////////////////////////////////////////////////////////////////////////
		if (GetKeyboardTrigger(DIK_3)) {
			PlaySound(SOUND_LABEL_SE_PLAYER_HIT);	// 被弾音をプレイ
			AddPlayerHP(i, -9999);
		}



		////////////////////////////////////////////////////////////////////////////
		// 移動処理
		////////////////////////////////////////////////////////////////////////////
		// 弾かれてなければ
		if (g_Player[i].isPopped == FALSE) {

			float speed = g_Player[i].speed.x;

			g_Player[i].animPattern = CHAR_DIR_FORWARD;		// デフォルトで前向きアニメーション
			if (GetKeyboardPress(DIK_DOWN) || IsButtonPressed(0, BUTTON_DOWN))
			{
				g_Player[i].pos.y += speed;
				g_Player[i].dirV = CHAR_DIR_DOWNWARD;
				g_Player[i].animPattern = CHAR_DIR_DOWNWARD;
			}
			else if (GetKeyboardPress(DIK_UP) || IsButtonPressed(0, BUTTON_UP))
			{
				g_Player[i].pos.y -= speed;
				g_Player[i].dirV = CHAR_DIR_UPWARD;
				g_Player[i].animPattern = CHAR_DIR_UPWARD;
			}
			else {	// 上下キーを押してない場合は縦方向をリセット
				g_Player[i].dirV = CHAR_DIR_FORWARD;
			}

			if (GetKeyboardPress(DIK_RIGHT) || IsButtonPressed(0, BUTTON_RIGHT))
			{
				g_Player[i].pos.x += speed;
				g_Player[i].dirH = CHAR_DIR_FORWARD;
			}
			else if (GetKeyboardPress(DIK_LEFT) || IsButtonPressed(0, BUTTON_LEFT))
			{
				g_Player[i].pos.x -= speed;
				g_Player[i].dirH = CHAR_DIR_BACKWARD;
			}
		}

		// 弾かれていれば
		else {

			g_Player[i].poppedCnt++;

			// 左へ飛ぶスピードにグラフをかける(y = 1 - x ^ 2)
			float ratio = 1.0f -
				(g_Player[i].poppedCnt / POPPED_TIME) * (g_Player[i].poppedCnt / POPPED_TIME);
			
			XMFLOAT3 speed = XMFLOAT3(g_Player[i].poppedSpeed.x * ratio, g_Player[i].poppedSpeed.y, g_Player[i].poppedSpeed.z);
			
			// 左へ飛んでいく
			XMVECTOR xmvPos = XMLoadFloat3(&g_Player[i].pos);
			XMVECTOR xmvSpeed = XMLoadFloat3(&speed);
			XMStoreFloat3(&g_Player[i].pos, xmvPos + xmvSpeed);

			// 終了？
			if (g_Player[i].poppedCnt == POPPED_TIME) {
				g_Player[i].poppedCnt = 0.0f;
				g_Player[i].isPopped = FALSE;

				// POPPEDエフェクトを無効化
				StopEffect(EFFECT_TYPE_PLAYER_POPPED00);
				StopEffect(EFFECT_TYPE_PLAYER_POPPED01);

				// BACKエフェクトを有効化
				SetEffect(EFFECT_TYPE_PLAYER_BACKFIRE, i, EFFECT_LOOP_FOREVER);
				SetEffect(EFFECT_TYPE_PLAYER_BACKGLOW, i, EFFECT_LOOP_FOREVER);
				SetEffect(EFFECT_TYPE_PLAYER_BACKPARTICLES, i, EFFECT_LOOP_FOREVER);
			}
		}

		// 慣性


		/////////////////////////////////////////////////////////////////////////////
		// MAP外チェック
		/////////////////////////////////////////////////////////////////////////////
		BG* bg = GetBG();

		if (g_Player[i].pos.x < 0.0f)
		{
			g_Player[i].pos.x = 0.0f;
		}

		if (g_Player[i].pos.x > SCREEN_WIDTH)
		{
			g_Player[i].pos.x = SCREEN_WIDTH;
		}

		if (g_Player[i].pos.y < 0.0f)
		{
			g_Player[i].pos.y = 0.0f;
		}

		if (g_Player[i].pos.y > SCREEN_HEIGHT)
		{
			g_Player[i].pos.y = SCREEN_HEIGHT;
		}





		/////////////////////////////////////////////////////////////////////////////
		// 被弾アニメーション
		/////////////////////////////////////////////////////////////////////////////
		// 被弾？
		if (g_Player[i].isDamaged == TRUE) {

			g_Player[i].damagedCnt++;
			if (g_Player[i].damagedCnt % DAMAGED_WAIT == 0) {

				g_Player[i].isAlphaChanged = !g_Player[i].isAlphaChanged;	// αフラグを反転
				if (g_Player[i].damagedCnt >= DAMAGED_TIME) {		// アニメーション終了？

					g_Player[i].isDamaged = FALSE;					// 無敵状態終了
					g_Player[i].isAlphaChanged = FALSE;		// アニメパラメーターをクリア
					g_Player[i].damagedCnt = 0;					//
				}
			}

		}

		/////////////////////////////////////////////////////////////////////////////
		// バレット処理
		/////////////////////////////////////////////////////////////////////////////
		// 弾かれ中は撃てない
		if (g_Player[i].isPopped == FALSE) {

			// 連射処理
			if (GetKeyboardPress(DIK_SPACE) || IsButtonPressed(0, BUTTON_A)) {

				// 連射判定
				if (g_Player[i].shootCnt % SHOOT_INTERVAL == 0) {

					g_Player[i].shootCnt++;			// カウンタを進めて、連射する
				}
				else {
					g_Player[i].shootCnt++;			// カウンタを進めて
					break;							// 連射しない
				}

				XMFLOAT3 pos = g_Player[i].pos;
				XMFLOAT3 shootPos = XMFLOAT3(pos.x + g_ShootDiff.x, pos.y + g_ShootDiff.y, 0.0f);

				// レベルにより弾をセット
				if (g_Player[i].level == PLAYER_LEVEL00) {				// 単発
					SetBullet(BULLET_TYPE_PLAYER_NORMAL, shootPos);

				}
				else if (g_Player[i].level == PLAYER_LEVEL01) {			// 2連扇形弾幕
					SetBarrage(BULLET_TYPE_PLAYER_NORMAL, shootPos, 4, -2, 2);
				}
				else if (g_Player[i].level == PLAYER_LEVEL02) {			// 3連扇形弾幕
					SetBarrage(BULLET_TYPE_PLAYER_NORMAL, shootPos, 5, -5, 5);
				}
				else if (g_Player[i].level == PLAYER_LEVEL03) {			// 4
					SetBarrage(BULLET_TYPE_PLAYER_NORMAL, shootPos, 4, -6, 6);
				}
				else if (g_Player[i].level == PLAYER_LEVEL04) {			// 5
					SetBarrage(BULLET_TYPE_PLAYER_NORMAL, shootPos, 5, -10, 10);
				}

				// 発射SE(加重ランダム)
				float rate = rand() % 100;
				if (rate < g_ShootSENode00) {
					PlaySound(SOUND_LABEL_SE_SHOOT000);
				}
				else if (rate < g_ShootSENode01) {
					PlaySound(SOUND_LABEL_SE_SHOOT001);
				}
				else {
					PlaySound(SOUND_LABEL_SE_SHOOT002);
				}

				// 発射エフェクト
				SetEffect(EFFECT_TYPE_PLAYER_SHOT, i, 1);
				SetEffect(EFFECT_TYPE_PLAYER_FLASH, i, 1);
			}

			// 連射カウンタをクリア
			else
				g_Player[i].shootCnt = 0;


			//if (IsButtonTriggered(0, BUTTON_B))
			//{
			//	XMFLOAT3 pos = g_Player[i].pos;
			//	pos.y += g_Player[i].jumpY;
			//	SetBullet(pos);
			//}

		}



		/////////////////////////////////////////////////////////////////////////////
		// エネミーとの当たり判定
		/////////////////////////////////////////////////////////////////////////////

		// ゲームセットじゃなければ
		if(GetGamePhase() != PHASE_ENTRO) {

			ENEMY* enemy = GetEnemy();

			// エネミーの数分当たり判定を行う
			for (int j = 0; j < ENEMY_MAX; j++)
			{
				// 生きてるエネミーと当たり判定をする
				if (enemy[j].isActive == FALSE) {
					continue;
				}

				BOOL collision = CollisionBB(g_Player[i].pos, g_Player[i].colliderW, g_Player[i].colliderH,
					enemy[j].pos, enemy[j].colliderW, enemy[j].colliderH);
				if (collision == FALSE) {
					continue;
				}


				// ボス以外のエネミーは瞬殺、ボスはノーダメ
				if (enemy[j].type != ENEMY_TYPE_BOSS) {
					KillEnemy(j);
				}

				// 弾かれる(無敵でも)
				g_Player[i].isPopped = TRUE;

				// 上下へ弾かれるスピードをランダムに決める
				if (rand() % 2 == 0) {
					g_Player[i].poppedSpeed.y = -g_Player[i].poppedSpeed.y;
				}

				// POPPEDエフェクトを有効化
				SetEffect(EFFECT_TYPE_PLAYER_POPPED00, i, EFFECT_LOOP_FOREVER);
				SetEffect(EFFECT_TYPE_PLAYER_POPPED01, i, EFFECT_LOOP_FOREVER);

				// BACKエフェクトを無効化
				StopEffect(EFFECT_TYPE_PLAYER_BACKFIRE);
				StopEffect(EFFECT_TYPE_PLAYER_BACKGLOW);
				StopEffect(EFFECT_TYPE_PLAYER_BACKPARTICLES);

				// 被弾音
				PlaySound(SOUND_LABEL_SE_PLAYER_HIT);
				// 被弾エフェクト
				SetEffect(EFFECT_TYPE_PLAYER_HIT, g_Player[i].pos, 1);

				// 無敵状態でなければ
				if (g_Player[i].isDamaged == FALSE) {

					// プレイヤーに大ダメージ
					AddPlayerHP(i, -3);

					// damageエフェクトをセット
					SetDamage();
				}
			}
		}
	}

	// 爆死カウントをする？
	if (g_IsDeadCnting == TRUE) {
		g_DeadCnt++;
		// 爆死カウントが終われば、か、ENTER押せば
		if (g_DeadCnt >= g_DeadNum || GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0)) {
			// シーンチェンジ
			SetFade(FADE_OUT, MODE_GAMEOVER);
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
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
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].isActive == TRUE)		// アクティブ中プレイヤーのみ実行
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

			//プレイヤーの位置やテクスチャー座標を反映
			float px = g_Player[i].pos.x - bg->pos.x;		// BGを追従
			float py = g_Player[i].pos.y - bg->pos.y;		//
			float pw = g_Player[i].w;
			float ph = g_Player[i].h;

			// アニメーション用
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Player[i].animPattern % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Player[i].animPattern / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// 被弾アニメーションか？
			if (g_Player[i].isAlphaChanged == TRUE) {
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f),
					g_Player[i].rot.z);
			}
			else {
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Player[i].rot.z);
			}


			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}

//=============================================================================
// Player構造体の先頭アドレスを取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}

//=============================================================================
// プレイヤーがUIに遮られたか
//=============================================================================
BOOL IsPlayerInsideUI(void) {
			
	UI* UI = GetUI();

	for (int i = 0; i < PLAYER_MAX; i++) {

		if (g_Player[i].isActive == TRUE) {				// 有効プレイヤーのみチェック

			// Y値をチェック
			float playerY = g_Player[i].pos.y;
			float UIY = UI->pos.y + UI->h + UI->extraRange;

			if (playerY < UIY) {
				return TRUE;			// 1人でも中にいれば
			}

		}

	}

	return FALSE;						// 全員外にいれば

}

//=============================================================================
// HPを増減
//=============================================================================
void AddPlayerHP(int player, int add) {

	g_Player[player].hp += add;

	// 上限を超えた？
	if (g_Player[player].hp > PLAYER_HP_MAX) {
		g_Player[player].hp = PLAYER_HP_MAX;
	}

	if (add > 0) {
		return;
	}

	g_Player[player].isDamaged = TRUE;								// 被弾アニメーションをプレイ

	// 存命？
	if (g_Player[player].hp > 0) {
		return;
	}

	//////////////////////////////////////////////////////////////////////////////
	// 死んだ場合の処理
	//////////////////////////////////////////////////////////////////////////////
	g_Player[player].isActive = FALSE;

	// PLAYER関連エフェクトを無効
	StopEffect(EFFECT_TYPE_PLAYER_BACKFIRE);
	StopEffect(EFFECT_TYPE_PLAYER_BACKGLOW);
	StopEffect(EFFECT_TYPE_PLAYER_BACKPARTICLES);
	StopEffect(EFFECT_TYPE_PLAYER_FLASH);
	StopEffect(EFFECT_TYPE_PLAYER_POPPED00);
	StopEffect(EFFECT_TYPE_PLAYER_POPPED01);

	// スロー
	SetSlow(SLOW_PLAYER_DIE, SLOW_FRAME_NUM_DEFAULT * 3.0f);

	// 爆死エフェクト
	SetEffect(EFFECT_TYPE_ENEMY_EXPLOSION, g_Player[player].pos, 5);

	// エネミーの爆死サウンドを止める
	StopSound(SOUND_LABEL_SE_EXPLOSION000);
	StopSound(SOUND_LABEL_SE_EXPLOSION001);
	StopSound(SOUND_LABEL_SE_EXPLOSION002);

	// 存命人数を調整
	g_PlayerAlive--;

	// 全員死亡していれば
	if (g_PlayerAlive <= 0) {
		// ゲームセットフラグ
		SetGamePhase(PHASE_ENTRO);
		// 爆死カウンタをセット
		StartDeadCnt();
		// BGMをとめる
		StopSound(SOUND_LABEL_BGM_GAME);
		StopSound(SOUND_LABEL_BGM_BOSS);
	}
}

//=============================================================================
// POWERを増減
//=============================================================================
void AddPlayerPower(int player, int add) {

	g_Player[player].powerUp += add;

	if (g_Player[player].powerUp >= g_LevelUpCost[g_Player[player].level]) {
		g_Player[player].powerUp = 0;
		g_Player[player].level++;

		if (g_Player[player].level >= PLAYER_LEVEL_MAX) {
			g_Player[player].level = PLAYER_LEVEL_MAX - 1;
		}
	}
}

//=============================================================================
// DeadCntをセット
//=============================================================================
void StartDeadCnt(void) {
	g_IsDeadCnting = TRUE;
	g_DeadCnt = 0;
}