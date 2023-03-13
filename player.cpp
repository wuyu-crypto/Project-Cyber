//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : GP11B132 33 ���D
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(180)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(180)	// 
#define TEXTURE_MAX					(10)	// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(2)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// ���p�^�[����

// �v���C���[�̉�ʓ��z�u���W
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

#define	SHOOT_SE_RATE_000			(50)		// ����SE�̊���
#define	SHOOT_SE_RATE_001			(30)		//
#define	SHOOT_SE_RATE_002			(20)		// �g��Ȃ�

#define	PLAYER_COLLIDER_WIDTH		50			// �v���C���[�̓����蔻��T�C�Y
#define	PLAYER_COLLIDER_HEIGHT		25			//

#define	DAMAGED_TIME				120.0f			// ���G����
#define	DAMAGED_WAIT				4				// ��e�A�j���[�V�����̃E�F�C�g�l

#define	POPPED_TIME					40.0f			// �e���ꎞ��

#define	HP_START					10				// �X�^�[�gHP

#define	SHOOT_INTERVAL				10				// �A�ˊԊu

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

// �e�N�X�`���t�@�C��
static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/spaceship_player_sprite.png",
};

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static PLAYER	g_Player[PLAYER_MAX];	// �v���C���[�\����

static int		g_PlayerAlive;			// �������v���C���[��

static float	g_ShootSENode00;		// ����SE����p
static float	g_ShootSENode01;

static XMFLOAT3	g_ShootDiff = XMFLOAT3(20.0f, 0.0f, 0.0f);	// �u���b�Ɣ��˒n�_�̃v���C���[����̃Y��
static XMFLOAT3	g_ShootEffectDiff = XMFLOAT3(90.0f, 0.0f, 0.0f);	// ����SE�̃v���C���[����̃Y��

static int g_LevelUpCost[PLAYER_LEVEL_MAX - 1] = {		// ���x���A�b�v�ɕK�v�ȃp���[�A�b�v��
	1, 1, 2, 3
};

static BOOL	g_IsDeadCnting;	// �����J�E���g����H
static int	g_DeadCnt;		// �v���C���[����GAMEOVER��J�E���^
static int	g_DeadNum;		// ������]�C���Ԃ̒���

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �v���C���[�\���̂̏�����
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_Player[i].isActive = FALSE;
		g_Player[i].pos = XMFLOAT3(400.0f, 400.0f, 0.0f);	// ���S�_����\��
		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].w = TEXTURE_WIDTH;
		g_Player[i].h = TEXTURE_HEIGHT;
		g_Player[i].texNo = 0;

		g_Player[i].animFrameCnt = 0;
		g_Player[i].animPattern = 0;

		g_Player[i].speed = XMFLOAT3(10.0f, 0.0f, 0.0f);

		g_Player[i].dirV = 0;
		g_Player[i].dirH = CHAR_DIR_FORWARD;									// �f�t�H���g�͑O����
		g_Player[i].animPattern = g_Player[i].dirH * TEXTURE_PATTERN_DIVIDE_X;

		// HP������
		if (PLAYER_HP_MAX < HP_START) {	// HP������Ⴂ�ꍇ��HP�����D��
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
		g_Player[i].poppedSpeed = XMFLOAT3(-30.0f, 3.0f, 0.0f);		// POPPED�ő�X�s�[�h

		g_Player[i].level = PLAYER_LEVEL00;		// �X�^�[�g���x��
		g_Player[i].powerUp = 0;

		g_Player[i].shootCnt = 0;
	}

	g_ShootSENode00 = SHOOT_SE_RATE_000;
	g_ShootSENode01 = SHOOT_SE_RATE_000 + SHOOT_SE_RATE_001;

	// 1P�̂ݗL����
	g_Player[0].isActive = TRUE;
	g_PlayerAlive = 1;

	g_IsDeadCnting = FALSE;
	g_DeadCnt = 0;
	g_DeadNum = 300;	// ������]�C����(�X���[�܂�)

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdatePlayer(void) {

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// �����Ă�v���C���[��������������
		if (g_Player[i].isActive == FALSE) {
			continue;
		}

		/////////////////////////////////////////////////////////////////////////////
		// ���ŏ���
		/////////////////////////////////////////////////////////////////////////////
		if (GetKeyboardTrigger(DIK_3)) {
			PlaySound(SOUND_LABEL_SE_PLAYER_HIT);	// ��e�����v���C
			AddPlayerHP(i, -9999);
		}



		////////////////////////////////////////////////////////////////////////////
		// �ړ�����
		////////////////////////////////////////////////////////////////////////////
		// �e����ĂȂ����
		if (g_Player[i].isPopped == FALSE) {

			float speed = g_Player[i].speed.x;

			g_Player[i].animPattern = CHAR_DIR_FORWARD;		// �f�t�H���g�őO�����A�j���[�V����
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
			else {	// �㉺�L�[�������ĂȂ��ꍇ�͏c���������Z�b�g
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

		// �e����Ă����
		else {

			g_Player[i].poppedCnt++;

			// ���֔�ԃX�s�[�h�ɃO���t��������(y = 1 - x ^ 2)
			float ratio = 1.0f -
				(g_Player[i].poppedCnt / POPPED_TIME) * (g_Player[i].poppedCnt / POPPED_TIME);
			
			XMFLOAT3 speed = XMFLOAT3(g_Player[i].poppedSpeed.x * ratio, g_Player[i].poppedSpeed.y, g_Player[i].poppedSpeed.z);
			
			// ���֔��ł���
			XMVECTOR xmvPos = XMLoadFloat3(&g_Player[i].pos);
			XMVECTOR xmvSpeed = XMLoadFloat3(&speed);
			XMStoreFloat3(&g_Player[i].pos, xmvPos + xmvSpeed);

			// �I���H
			if (g_Player[i].poppedCnt == POPPED_TIME) {
				g_Player[i].poppedCnt = 0.0f;
				g_Player[i].isPopped = FALSE;

				// POPPED�G�t�F�N�g�𖳌���
				StopEffect(EFFECT_TYPE_PLAYER_POPPED00);
				StopEffect(EFFECT_TYPE_PLAYER_POPPED01);

				// BACK�G�t�F�N�g��L����
				SetEffect(EFFECT_TYPE_PLAYER_BACKFIRE, i, EFFECT_LOOP_FOREVER);
				SetEffect(EFFECT_TYPE_PLAYER_BACKGLOW, i, EFFECT_LOOP_FOREVER);
				SetEffect(EFFECT_TYPE_PLAYER_BACKPARTICLES, i, EFFECT_LOOP_FOREVER);
			}
		}

		// ����


		/////////////////////////////////////////////////////////////////////////////
		// MAP�O�`�F�b�N
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
		// ��e�A�j���[�V����
		/////////////////////////////////////////////////////////////////////////////
		// ��e�H
		if (g_Player[i].isDamaged == TRUE) {

			g_Player[i].damagedCnt++;
			if (g_Player[i].damagedCnt % DAMAGED_WAIT == 0) {

				g_Player[i].isAlphaChanged = !g_Player[i].isAlphaChanged;	// ���t���O�𔽓]
				if (g_Player[i].damagedCnt >= DAMAGED_TIME) {		// �A�j���[�V�����I���H

					g_Player[i].isDamaged = FALSE;					// ���G��ԏI��
					g_Player[i].isAlphaChanged = FALSE;		// �A�j���p�����[�^�[���N���A
					g_Player[i].damagedCnt = 0;					//
				}
			}

		}

		/////////////////////////////////////////////////////////////////////////////
		// �o���b�g����
		/////////////////////////////////////////////////////////////////////////////
		// �e���ꒆ�͌��ĂȂ�
		if (g_Player[i].isPopped == FALSE) {

			// �A�ˏ���
			if (GetKeyboardPress(DIK_SPACE) || IsButtonPressed(0, BUTTON_A)) {

				// �A�˔���
				if (g_Player[i].shootCnt % SHOOT_INTERVAL == 0) {

					g_Player[i].shootCnt++;			// �J�E���^��i�߂āA�A�˂���
				}
				else {
					g_Player[i].shootCnt++;			// �J�E���^��i�߂�
					break;							// �A�˂��Ȃ�
				}

				XMFLOAT3 pos = g_Player[i].pos;
				XMFLOAT3 shootPos = XMFLOAT3(pos.x + g_ShootDiff.x, pos.y + g_ShootDiff.y, 0.0f);

				// ���x���ɂ��e���Z�b�g
				if (g_Player[i].level == PLAYER_LEVEL00) {				// �P��
					SetBullet(BULLET_TYPE_PLAYER_NORMAL, shootPos);

				}
				else if (g_Player[i].level == PLAYER_LEVEL01) {			// 2�A��`�e��
					SetBarrage(BULLET_TYPE_PLAYER_NORMAL, shootPos, 4, -2, 2);
				}
				else if (g_Player[i].level == PLAYER_LEVEL02) {			// 3�A��`�e��
					SetBarrage(BULLET_TYPE_PLAYER_NORMAL, shootPos, 5, -5, 5);
				}
				else if (g_Player[i].level == PLAYER_LEVEL03) {			// 4
					SetBarrage(BULLET_TYPE_PLAYER_NORMAL, shootPos, 4, -6, 6);
				}
				else if (g_Player[i].level == PLAYER_LEVEL04) {			// 5
					SetBarrage(BULLET_TYPE_PLAYER_NORMAL, shootPos, 5, -10, 10);
				}

				// ����SE(���d�����_��)
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

				// ���˃G�t�F�N�g
				SetEffect(EFFECT_TYPE_PLAYER_SHOT, i, 1);
				SetEffect(EFFECT_TYPE_PLAYER_FLASH, i, 1);
			}

			// �A�˃J�E���^���N���A
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
		// �G�l�~�[�Ƃ̓����蔻��
		/////////////////////////////////////////////////////////////////////////////

		// �Q�[���Z�b�g����Ȃ����
		if(GetGamePhase() != PHASE_ENTRO) {

			ENEMY* enemy = GetEnemy();

			// �G�l�~�[�̐��������蔻����s��
			for (int j = 0; j < ENEMY_MAX; j++)
			{
				// �����Ă�G�l�~�[�Ɠ����蔻�������
				if (enemy[j].isActive == FALSE) {
					continue;
				}

				BOOL collision = CollisionBB(g_Player[i].pos, g_Player[i].colliderW, g_Player[i].colliderH,
					enemy[j].pos, enemy[j].colliderW, enemy[j].colliderH);
				if (collision == FALSE) {
					continue;
				}


				// �{�X�ȊO�̃G�l�~�[�͏u�E�A�{�X�̓m�[�_��
				if (enemy[j].type != ENEMY_TYPE_BOSS) {
					KillEnemy(j);
				}

				// �e�����(���G�ł�)
				g_Player[i].isPopped = TRUE;

				// �㉺�֒e�����X�s�[�h�������_���Ɍ��߂�
				if (rand() % 2 == 0) {
					g_Player[i].poppedSpeed.y = -g_Player[i].poppedSpeed.y;
				}

				// POPPED�G�t�F�N�g��L����
				SetEffect(EFFECT_TYPE_PLAYER_POPPED00, i, EFFECT_LOOP_FOREVER);
				SetEffect(EFFECT_TYPE_PLAYER_POPPED01, i, EFFECT_LOOP_FOREVER);

				// BACK�G�t�F�N�g�𖳌���
				StopEffect(EFFECT_TYPE_PLAYER_BACKFIRE);
				StopEffect(EFFECT_TYPE_PLAYER_BACKGLOW);
				StopEffect(EFFECT_TYPE_PLAYER_BACKPARTICLES);

				// ��e��
				PlaySound(SOUND_LABEL_SE_PLAYER_HIT);
				// ��e�G�t�F�N�g
				SetEffect(EFFECT_TYPE_PLAYER_HIT, g_Player[i].pos, 1);

				// ���G��ԂłȂ����
				if (g_Player[i].isDamaged == FALSE) {

					// �v���C���[�ɑ�_���[�W
					AddPlayerHP(i, -3);

					// damage�G�t�F�N�g���Z�b�g
					SetDamage();
				}
			}
		}
	}

	// �����J�E���g������H
	if (g_IsDeadCnting == TRUE) {
		g_DeadCnt++;
		// �����J�E���g���I���΁A���AENTER������
		if (g_DeadCnt >= g_DeadNum || GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0)) {
			// �V�[���`�F���W
			SetFade(FADE_OUT, MODE_GAMEOVER);
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetBG();
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].isActive == TRUE)		// �A�N�e�B�u���v���C���[�̂ݎ��s
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Player[i].pos.x - bg->pos.x;		// BG��Ǐ]
			float py = g_Player[i].pos.y - bg->pos.y;		//
			float pw = g_Player[i].w;
			float ph = g_Player[i].h;

			// �A�j���[�V�����p
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Player[i].animPattern % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Player[i].animPattern / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// ��e�A�j���[�V�������H
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


			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}

//=============================================================================
// Player�\���̂̐擪�A�h���X���擾
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}

//=============================================================================
// �v���C���[��UI�ɎՂ�ꂽ��
//=============================================================================
BOOL IsPlayerInsideUI(void) {
			
	UI* UI = GetUI();

	for (int i = 0; i < PLAYER_MAX; i++) {

		if (g_Player[i].isActive == TRUE) {				// �L���v���C���[�̂݃`�F�b�N

			// Y�l���`�F�b�N
			float playerY = g_Player[i].pos.y;
			float UIY = UI->pos.y + UI->h + UI->extraRange;

			if (playerY < UIY) {
				return TRUE;			// 1�l�ł����ɂ����
			}

		}

	}

	return FALSE;						// �S���O�ɂ����

}

//=============================================================================
// HP�𑝌�
//=============================================================================
void AddPlayerHP(int player, int add) {

	g_Player[player].hp += add;

	// ����𒴂����H
	if (g_Player[player].hp > PLAYER_HP_MAX) {
		g_Player[player].hp = PLAYER_HP_MAX;
	}

	if (add > 0) {
		return;
	}

	g_Player[player].isDamaged = TRUE;								// ��e�A�j���[�V�������v���C

	// �����H
	if (g_Player[player].hp > 0) {
		return;
	}

	//////////////////////////////////////////////////////////////////////////////
	// ���񂾏ꍇ�̏���
	//////////////////////////////////////////////////////////////////////////////
	g_Player[player].isActive = FALSE;

	// PLAYER�֘A�G�t�F�N�g�𖳌�
	StopEffect(EFFECT_TYPE_PLAYER_BACKFIRE);
	StopEffect(EFFECT_TYPE_PLAYER_BACKGLOW);
	StopEffect(EFFECT_TYPE_PLAYER_BACKPARTICLES);
	StopEffect(EFFECT_TYPE_PLAYER_FLASH);
	StopEffect(EFFECT_TYPE_PLAYER_POPPED00);
	StopEffect(EFFECT_TYPE_PLAYER_POPPED01);

	// �X���[
	SetSlow(SLOW_PLAYER_DIE, SLOW_FRAME_NUM_DEFAULT * 3.0f);

	// �����G�t�F�N�g
	SetEffect(EFFECT_TYPE_ENEMY_EXPLOSION, g_Player[player].pos, 5);

	// �G�l�~�[�̔����T�E���h���~�߂�
	StopSound(SOUND_LABEL_SE_EXPLOSION000);
	StopSound(SOUND_LABEL_SE_EXPLOSION001);
	StopSound(SOUND_LABEL_SE_EXPLOSION002);

	// �����l���𒲐�
	g_PlayerAlive--;

	// �S�����S���Ă����
	if (g_PlayerAlive <= 0) {
		// �Q�[���Z�b�g�t���O
		SetGamePhase(PHASE_ENTRO);
		// �����J�E���^���Z�b�g
		StartDeadCnt();
		// BGM���Ƃ߂�
		StopSound(SOUND_LABEL_BGM_GAME);
		StopSound(SOUND_LABEL_BGM_BOSS);
	}
}

//=============================================================================
// POWER�𑝌�
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
// DeadCnt���Z�b�g
//=============================================================================
void StartDeadCnt(void) {
	g_IsDeadCnting = TRUE;
	g_DeadCnt = 0;
}