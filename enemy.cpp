//=============================================================================
//
// �G�l�~�[���� [enemy.cpp]
// Author : GP11B132 33 ���D
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
// �}�N����`
//*****************************************************************************
#define	TEXTURE_MAX					ENEMY_TYPE_MAX

#define	BOSS_IDLING_TIME			480.0f					// ���V0.5���̃t���[����



// �o���b�g���˔ԍ�
enum {

	ENEMY_SHOOT_TYPE_STRAIGHT,			// �^����������
	ENEMY_SHOOT_TYPE_AIM,				// �v���C���[�Ɍ������Č���
	ENEMY_SHOOT_TYPE_BARRAGE00,			// �G����5�����e��

#ifdef BEZIER
	ENEMY_SHOOT_TYPE_BEZIER,			// �Ȑ��ǔ�
#endif

	ENEMY_ATTACK_TYPE_MAX,
};


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O

static ENEMY	g_Enemy[ENEMY_MAX];		// �G�l�~�[�X���b�g

static float	g_BossMovingCnt;					// �{�X�ړ��J�E���^
static float	g_BossAttackCnt;					// �{�X�U���J�E���^

static BOOL		g_IsTutorial;						// �`���[�g���A���t���O

static int		g_BossAction;						// �{�X�s�����x��
static int		g_LastAction;						// �ЂƂO�̍s��

static int		g_BossActionFrame[BOSS_MAX] = {		// �{�X�e�s���̎��s�t���[����

	750,	// �o�ꎞ��(BGM�ɍ��킹��)(12.5�b)

	480,
	480,

	240,

	0,

	480,	// �����A�j������
};

static XMFLOAT3 g_BossSpawningSpeed;				// �ő�o�ꑬ�x

static XMFLOAT3	g_BossIdlingSpeed;					// �ő啂�V���x
static BOOL		g_IsFirstIdling;					// �o�꒼��̕��V�H

static XMFLOAT3 g_BossSinkingSpeed;					// �������x
static XMFLOAT3 g_SinkingExplosionDiff;				// �����G�t�F�N�g�̍ő勗��

static float	g_SinkingCnt;		// �����u���p�J�E���^
static XMFLOAT3 g_SinkingPos;		// �����u���`��p���W
static XMFLOAT3 g_SinkingDiff;		// �����u���̍ő勗��
static int		g_SinkingSoundCnt;	// ����SE�p�t���O

static float	g_BossBarrageAngle;					// �{�X�e���p�x�L�^�p

static int		g_BossShotWait[BOSS_MAX] = {		// �{�X�̍U���E�F�C�g�l
	0,

	20,
	10,

	0,
	0,

	20,		// ���������G�t�F�N�g�̃Z�b�g�Ԋu
};

static ENEMY_DATA g_EnemyData[ENEMY_TYPE_MAX] = {

#ifndef BEZIER


	{
		"data/TEXTURE/enemy00.png",						// �e�N�X�`����
		XMINT2(200, 200),								// �T�C�Y
		BULLET_TYPE_ENEMY_NORMAL,						// �o���b�g�^�C�v
		ENEMY_SHOOT_TYPE_STRAIGHT, 60,					// ���˃p�^�[��, �e���˃E�F�C�g
		XMINT2(2, 1), 2, 4,								// ������, �p�^�[����, �E�F�C�g
		160, 80,										// �����蔻��
		5, 5000, 500,									// HP, ���j���_, �������_
		10,												// �h���b�v��
	},

#else

	{
		"data/TEXTURE/enemy00.png",						// �e�N�X�`����
		XMINT2(200, 200),								// �T�C�Y
		BULLET_TYPE_ENEMY_BEZIER,						// �o���b�g�^�C�v
		ENEMY_SHOOT_TYPE_BEZIER,	60,					// ���˃p�^�[��, �e���˃E�F�C�g
		XMINT2(2, 1), 2, 4,								// ������, �p�^�[����, �E�F�C�g
		160, 80,										// �����蔻��
		5, 5000, 500,									// HP, ���j���_, �������_
		10,												// �h���b�v��
	},

#endif

	{
		"data/TEXTURE/enemy01.png",
		XMINT2(200, 200),
		BULLET_TYPE_ENEMY_NORMAL,
		ENEMY_SHOOT_TYPE_AIM, 60,
		XMINT2(2, 1), 2, 4,
		150, 150,
		10, 10000, 1000,
		40,
	},

	{
		"data/TEXTURE/enemy02.png",
		XMINT2(300, 300),
		BULLET_TYPE_ENEMY_SLOW,
		ENEMY_SHOOT_TYPE_BARRAGE00, 60,
		XMINT2(2, 1), 2, 4,
		280, 280,
		20, 30000, 1500,
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
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �G�l�~�[�\���̂̏�����
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		g_Enemy[i].isActive = FALSE;
		g_Enemy[i].pos = XMFLOAT3(200.0f + i*200.0f, 100.0f, 0.0f);	// ���S�_����\��
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].damagedAnimFrameCnt = 0;
		g_Enemy[i].damagedAnimPattern = 0;

		g_Enemy[i].isDamaged = FALSE;
	}


	// �{�X�̏�����
	g_BossAction = BOSS_SPAWNING;
	g_BossMovingCnt = 0.0f;
	g_BossAttackCnt = 0.0f;
	g_BossSpawningSpeed = XMFLOAT3(-4.0f, 0.0f, 0.0f);		// �ő�o�ꑬ�x
	g_BossIdlingSpeed = XMFLOAT3(0.0f, 1.0f, 0.0f);			// �ő啂�V���x
	g_IsFirstIdling = FALSE;
	g_BossBarrageAngle = 0;
	
	g_BossSinkingSpeed = XMFLOAT3(-0.8f, 1.6f, 0.0f);			// �������x
	g_SinkingExplosionDiff = XMFLOAT3(128.0f, 256.0f, 0.0f);	// ���������̍ő勗��
	g_SinkingCnt = 0.0f;
	g_SinkingDiff = XMFLOAT3(6.0f, 6.0f, 0.0f);		// �����u���̍ő勗��
	g_SinkingSoundCnt = 0;

	g_IsTutorial = TRUE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		// ��A�N�e�B�u�͔�΂�
		if (g_Enemy[i].isActive == FALSE) {
			continue;
		}

		int type = g_Enemy[i].type;					// �G�l�~�[�^�C�v



		/////////////////////////////////////////////////////////////////////////////
		// �ړ�����
		/////////////////////////////////////////////////////////////////////////////
		// �G��
		if(type != ENEMY_TYPE_BOSS) {

			XMVECTOR xmvPos = XMLoadFloat3(&g_Enemy[i].pos);
			XMVECTOR xmvSpeed = XMLoadFloat3(&g_Enemy[i].speed);
			XMStoreFloat3(&g_Enemy[i].pos, xmvPos + xmvSpeed);

			// ��ʍ��[���o���疳����(�����˂��o����)
			if (g_Enemy[i].pos.x < -g_Enemy[i].w * 0.5f) {
				g_Enemy[i].isActive = FALSE;
				continue;
			}
		}

		// �{�X�̂�
		else {

			// �J�E���^��i�߂�
			g_BossMovingCnt++;

			switch (g_BossAction) {

			case BOSS_SPAWNING:			// �o��
			{
				// �ő�o��X�s�[�h���擾
				float speed = g_BossSpawningSpeed.x;

				// �O���t��������( y=-(x^3)+1 )
				float ratio = g_BossMovingCnt / g_BossActionFrame[BOSS_SPAWNING];
				float graph = 1.0f - ratio * ratio * ratio;
				speed *= graph;

				// �ړ�
				g_Enemy[i].pos.x += speed;

				if (g_BossMovingCnt >= g_BossActionFrame[BOSS_SPAWNING]) {
					g_BossMovingCnt = 0;			// �J�E���^���N���A
					g_BossAttackCnt = 0;			//
					SetBossAction(BOSS_BARRAGE);	// ���̍s����
					g_IsFirstIdling = TRUE;			// �o�꒼�㕂�V���Z�b�g
				}
				break;
			}

			case BOSS_BARRAGE:			// �㉺���V
			case BOSS_HOMING:			//
			case BOSS_IDLING:			//
			case BOSS_CALLING:			//
			{
				// �ő啂�V���x���擾
				float speed = g_BossIdlingSpeed.y;

				// �o�꒼��H
				if (g_IsFirstIdling == TRUE) {
					// �O���t��������( y=-((4x - 1)^2)+1 )
					float ratio = g_BossMovingCnt / BOSS_IDLING_TIME;
					float graph = 1.0f - (4.0f * ratio - 1.0f) * (4.0f * ratio - 1.0f);

					// �ړ�
					g_Enemy[i].pos.y += speed * graph;

					if (g_BossMovingCnt >= BOSS_IDLING_TIME * 0.5f) {		// ���Ԃ̔���
						g_BossMovingCnt = 0;								// �J�E���^���N���A
						g_BossIdlingSpeed.y = -g_BossIdlingSpeed.y;	// �����𔽓]
						g_IsFirstIdling = FALSE;
					}
				}
				else {
					// �O���t��������( y=-((2x - 1)^2)+1 )
					float ratio = g_BossMovingCnt / BOSS_IDLING_TIME;
					float graph = 1.0f - (2.0f * ratio - 1.0f) * (2.0f * ratio - 1.0f);

					// �ړ�
					g_Enemy[i].pos.y += speed * graph;

					if (g_BossMovingCnt >= BOSS_IDLING_TIME) {
						g_BossMovingCnt = 0;								// �J�E���^���N���A
						g_BossIdlingSpeed.y = -g_BossIdlingSpeed.y;	// �����𔽓]
					}
				}
				break;
			}

			case BOSS_SINKING:			// ����
			{
				// �ړ�
				g_Enemy[i].pos.x += g_BossSinkingSpeed.x;
				g_Enemy[i].pos.y += g_BossSinkingSpeed.y;

				///////////////////////////////////////////////////////////////////////////
				// �����u�����v�Z
				///////////////////////////////////////////////////////////////////////////
				// �J�E���^��i�߂�
				g_SinkingCnt++;
				// �u������������߂�
				XMFLOAT3 vec = XMFLOAT3(
					g_SinkingDiff.x * cos(g_SinkingCnt),
					g_SinkingDiff.x * sin(g_SinkingCnt),
					0.0f);

				// �ŏI���W���v�Z
				XMVECTOR xmvVec = XMLoadFloat3(&vec);
				XMVECTOR xmvPos = XMLoadFloat3(&g_Enemy[i].pos);
				XMStoreFloat3(&g_SinkingPos, xmvVec + xmvPos);

				if (g_BossMovingCnt >= g_BossActionFrame[BOSS_SINKING] ||
					GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0)) {	// ENTER�L�[�ŃX�L�b�v
					// �{�X������
					g_Enemy[i].isActive = FALSE;
					//���U���g���[�h��
					SetFade(FADE_OUT, MODE_RESULT);
				}
				break;
			}
			}
		}


		/////////////////////////////////////////////////////////////////////////////
		// ��e�G�t�F�N�g
		/////////////////////////////////////////////////////////////////////////////
		// ��e��������H
		if (g_Enemy[i].isDamaged == TRUE) {

			g_Enemy[i].damagedAnimFrameCnt++;

			g_Enemy[i].damagedAnimPattern = 1;					// �p�^�[����ς���

			// wait�l�o�ĂΏI��
			if (g_Enemy[i].damagedAnimFrameCnt > g_EnemyData[type].wait) {

				g_Enemy[i].damagedAnimFrameCnt = 0.0f;

				g_Enemy[i].damagedAnimPattern = 0;				// �p�^�[����߂�
				g_Enemy[i].isDamaged = FALSE;			// ��e�G�t�F�N�g�I��

			}
		}


		/////////////////////////////////////////////////////////////////////////////
		// �U������
		/////////////////////////////////////////////////////////////////////////////
		g_Enemy[i].shootCnt++;			// �J�E���^��i�߂�

		// �{�X�ȊO
		if (type != ENEMY_TYPE_BOSS) {
			switch (g_EnemyData[type].shootType) {

			case ENEMY_SHOOT_TYPE_STRAIGHT:

				if (g_Enemy[i].shootCnt >= g_EnemyData[type].shootWait) {		// �E�F�C�g�ɒB�����H
					g_Enemy[i].shootCnt = 0;
					SetBullet(g_EnemyData[type].bulletType, g_Enemy[i].pos);
				}
				break;

			case ENEMY_SHOOT_TYPE_AIM:

				if (g_Enemy[i].shootCnt >= g_EnemyData[type].shootWait) {		// �E�F�C�g�ɒB�����H
					g_Enemy[i].shootCnt = 0;
					PLAYER* player = GetPlayer();
					BULLET_DATA* bulletData = GetBulletData();

					XMVECTOR xmvTargetSpeed = XMLoadFloat3(&player[0].pos) - XMLoadFloat3(&g_Enemy[i].pos);
					XMVECTOR xmvBulletSpeed = XMLoadFloat3(&bulletData[g_EnemyData[type].bulletType].speed);

					// ���������߂�
					XMVECTOR xmvBulletLength = XMVector3Length(xmvBulletSpeed);
					XMVECTOR xmvTargetLength = XMVector3Length(xmvTargetSpeed);

					XMVECTOR xmvResult = xmvTargetSpeed * (xmvBulletLength / xmvTargetLength);
					XMFLOAT3 result;
					XMStoreFloat3(&result, xmvResult);

					SetBullet(g_EnemyData[type].bulletType, g_Enemy[i].pos, result);
				}
				break;

			case ENEMY_SHOOT_TYPE_BARRAGE00:

				if (g_Enemy[i].shootCnt >= g_EnemyData[type].shootWait) {		// �E�F�C�g�ɒB�����H
					g_Enemy[i].shootCnt = 0;

					g_Enemy[i].shootPattern = (g_Enemy[i].shootPattern + 1) % 2;	// �p�^�[����ς���

					if (g_Enemy[i].shootPattern == 0) {
						SetBarrage(BULLET_TYPE_ENEMY_NORMAL, g_Enemy[i].pos, 30.0f, 0.0f);		// 
					}
					else {
						SetBarrage(BULLET_TYPE_ENEMY_NORMAL, g_Enemy[i].pos, 30.0f, 15.0f);		// ������ƃY����
					}

				}
				break;


#ifdef BEZIER

			case ENEMY_SHOOT_TYPE_BEZIER:

				if (g_Enemy[i].shootCnt >= g_EnemyData[type].shootWait) {		// �E�F�C�g�ɒB�����H
					g_Enemy[i].shootCnt = 0;

					SetBezierBullet(BULLET_TYPE_ENEMY_BEZIER, g_Enemy[i].pos, GetPlayer()->pos, 120.0f);
				}

				break;

#endif

			}
		}

		// �{�X
		else {
			g_BossAttackCnt++;		// �J�E���^��i�߂�

			if (g_BossAction == BOSS_BARRAGE) {
				if (g_Enemy[i].shootCnt >= g_BossShotWait[g_BossAction]) {
					g_Enemy[i].shootCnt = 0;
					SetBarrage(BULLET_TYPE_ENEMY_NORMAL, g_Enemy[i].pos, 20.0f, g_BossBarrageAngle);
					g_BossBarrageAngle += 15.0f;
				}

				if (g_BossAttackCnt >= g_BossActionFrame[g_BossAction]) {
					g_BossAttackCnt = 0;
					// ���[�h�ؑ�
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

					// ���������߂�
					XMVECTOR xmvBulletLength = XMVector3Length(xmvBulletSpeed);
					XMVECTOR xmvTargetLength = XMVector3Length(xmvTargetSpeed);

					XMVECTOR xmvResult = xmvTargetSpeed * (xmvBulletLength / xmvTargetLength);
					XMFLOAT3 result;
					XMStoreFloat3(&result, xmvResult);

					// 1��e���Z�b�g
					SetBullet(g_EnemyData[type].bulletType, g_Enemy[i].pos, result);

					// result�̖@���x�N�g�����擾(������xmvBulletLength)
					XMFLOAT3 normal = XMFLOAT3(result.y, -result.x, 0.0f);
					XMVECTOR xmvNormal = XMLoadFloat3(&normal);

					// �P�ʃx�N�g���ɂ���
					xmvNormal /= xmvBulletLength;

					// �Ԋu�����߂�(�e�̕��̉��{)
					float width = bulletData[g_EnemyData[type].bulletType].size.x;
					width *= 1.0f;

					// ���ˍ��W�����߂�
					XMFLOAT3 newPos = XMFLOAT3(
						g_Enemy[i].pos.x + xmvNormal.m128_f32[0] * width,
						g_Enemy[i].pos.y + xmvNormal.m128_f32[1] * width,
						0.0f);

					// 2��ڃZ�b�g
					SetBullet(g_EnemyData[type].bulletType, newPos, result);

					// ���ˍ��W�����߂�
					newPos = XMFLOAT3(
						g_Enemy[i].pos.x - xmvNormal.m128_f32[0] * width,
						g_Enemy[i].pos.y - xmvNormal.m128_f32[1] * width,
						0.0f);

					// 3��ڃZ�b�g
					SetBullet(g_EnemyData[type].bulletType, newPos, result);
				}

				if (g_BossAttackCnt >= g_BossActionFrame[g_BossAction]) {
					g_BossAttackCnt = 0;
					// ���[�h�ؑ�
					g_LastAction = g_BossAction;
					SetBossAction(BOSS_CALLING);		// �G�����Ă�
				}
			}
			else if (g_BossAction == BOSS_IDLING) {
				if (g_BossAttackCnt >= g_BossActionFrame[g_BossAction]) {
					g_BossAttackCnt = 0;

					// �U�����K�E���A���Ԃɐ؂�ւ���
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
				// ��Ɖ���ʂ�
				SetEnemy(ENEMY_TYPE03, XMFLOAT3(1.1f, 0.2f, 0.0f), XMFLOAT3(-1.2f, 0.0f, 0.0f));
				SetEnemy(ENEMY_TYPE03, XMFLOAT3(1.1f, 0.8f, 0.0f), XMFLOAT3(-1.2f, 0.0f, 0.0f));

				// ���[�h�ؑ�(�L�^�͂��Ȃ�)
				SetBossAction(BOSS_IDLING);
			}
			else if (g_BossAction == BOSS_SINKING) {	// �����F�����G�t�F�N�g���ʂɃZ�b�g

				if (g_Enemy[i].shootCnt >= g_BossShotWait[g_BossAction]) {
					g_Enemy[i].shootCnt = 0;

					// �Z�b�g���鋗���ƕ����������_���Ɍ��߂�
					XMFLOAT3 diff = XMFLOAT3(
						g_SinkingExplosionDiff.x * sinf(rand()),
						g_SinkingExplosionDiff.y * sinf(rand()),
						0.0f);
					XMVECTOR xmvDiff = XMLoadFloat3(&diff);

					// ���W�����߂�
					XMVECTOR xmvPos = XMLoadFloat3(&g_Enemy[i].pos);
					XMFLOAT3 result;
					XMStoreFloat3(&result, xmvPos + xmvDiff);

					// �G�t�F�N�g���Z�b�g
					SetEffect(EFFECT_TYPE_BOSS_EXPLOSION, result, 2);

					// ����SE��3���1��Đ�
					g_SinkingSoundCnt = (g_SinkingSoundCnt + 1) % 3;
					if(g_SinkingSoundCnt == 0)
						PlaySound(SOUND_LABEL_SE_EXPLOSION002);
				}
			}
		}
	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
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
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].isActive == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{
			int type = g_Enemy[i].type;

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy[i].type]);

			// type���{�X���{�X���������[�h�ł����
			if (type == ENEMY_TYPE_BOSS && g_BossAction == BOSS_SINKING) {

				float px = g_SinkingPos.x - bg->pos.x;	// �����n�_��BG��Ǐ]
				float py = g_SinkingPos.y - bg->pos.y;	// 
				float pw = g_Enemy[i].w;
				float ph = g_Enemy[i].h;

				float tw = 1.0f / g_EnemyData[type].patternDivide.x;	// �e�N�X�`���̕�
				float th = 1.0f / g_EnemyData[type].patternDivide.y;	// �e�N�X�`���̍���
				float tx = (float)(g_Enemy[i].damagedAnimPattern % g_EnemyData[type].patternDivide.x) * tw;	// �e�N�X�`���̍���X���W
				float ty = (float)(g_Enemy[i].damagedAnimPattern / g_EnemyData[type].patternDivide.x) * th;	// �e�N�X�`���̍���Y���W

				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Enemy[i].rot.z);
			}
			else {

				float px = g_Enemy[i].pos.x - bg->pos.x;	// BG��Ǐ]
				float py = g_Enemy[i].pos.y - bg->pos.y;	// 
				float pw = g_Enemy[i].w;
				float ph = g_Enemy[i].h;

				float tw = 1.0f / g_EnemyData[type].patternDivide.x;	// �e�N�X�`���̕�
				float th = 1.0f / g_EnemyData[type].patternDivide.y;	// �e�N�X�`���̍���
				float tx = (float)(g_Enemy[i].damagedAnimPattern % g_EnemyData[type].patternDivide.x) * tw;	// �e�N�X�`���̍���X���W
				float ty = (float)(g_Enemy[i].damagedAnimPattern / g_EnemyData[type].patternDivide.x) * th;	// �e�N�X�`���̍���Y���W

				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Enemy[i].rot.z);
			}

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}

//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}

//=============================================================================
// �G�l�~�[���Z�b�g
//=============================================================================
void SetEnemy(int type, XMFLOAT3 perPos, XMFLOAT3 speed) {

	for (int i = 0; i < ENEMY_MAX; i++) {
		
		// ���g�p�X���b�g������
		if (g_Enemy[i].isActive == TRUE) {
			continue;
		}

		g_Enemy[i].isActive = TRUE;
		g_Enemy[i].w = g_EnemyData[type].size.x;
		g_Enemy[i].h = g_EnemyData[type].size.y;

		// ���W��ϊ�
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

		// BOSS�ȊO�̓v���C���[�Ἠ��x���ɉ����ēGHP������
		if(type != ENEMY_TYPE_BOSS)
		{
			PLAYER* player = GetPlayer();

			// HP����Ἠ��x���Ɣ��
			float rate = sqrt((float)player->level + 1.0f);
			g_Enemy[i].hp = (float)g_EnemyData[type].hp * rate;
		}
		else {
			g_Enemy[i].hp = g_EnemyData[type].hp;
		}

		g_Enemy[i].killScore = g_EnemyData[type].killScore;
		g_Enemy[i].hitScore = g_EnemyData[type].hitScore;

		g_Enemy[i].dropRate = g_EnemyData[type].dropRate;


		// �{�X�̏�����
		if (type == ENEMY_TYPE_BOSS) {
			// HP��\��
			SetBossHp();
			// CUTIN���Z�b�g
			SetCutin(TRUE);
			// �Ȃ��~�߂�
			StopSound(SOUND_LABEL_BGM_GAME);
			// SE���v���C
			PlaySound(SOUND_LABEL_SE_CUTIN000);
			PlaySound(SOUND_LABEL_SE_CUTIN001);

			// �Q�[���t�F�[�X���Z�b�g
			SetGamePhase(PHASE_BOSS);
		}

		return;			// 1�̃Z�b�g������I��
	}
}

//=============================================================================
// �G�l�~�[��UI�ɎՂ�ꂽ��
//=============================================================================
BOOL IsEnemyInsideUI(void) {

	UI* UI = GetUI();

	for (int i = 0; i < ENEMY_MAX; i++) {

		if (g_Enemy[i].isActive == TRUE) {				// �L���v���C���[�̂݃`�F�b�N

			// Y�l���`�F�b�N
			float enemyY = g_Enemy[i].pos.y;
			float UIY = UI->pos.y + UI->h + UI->extraRange;

			if (enemyY < UIY) {
				return TRUE;		// 1�l�ł����ɂ����
			}

		}

	}

	return FALSE;					// �S���O�ɂ����

}

//=============================================================================
// HP�����Ǝ��S����
//=============================================================================
void AddEnemyHP(int enemy, int add) {

	g_Enemy[enemy].hp += add;

	// �G�l�~�[�����Ă�H
	if (g_Enemy[enemy].hp > 0) {

		AddScore(g_Enemy[enemy].hitScore);							// ���_
		g_Enemy[enemy].isDamaged = TRUE;							// ��e�A�j���[�V�������v���C
		PlaySound(SOUND_LABEL_SE_ENEMY_HIT);						// ��e�����v���C
		return;
	}

	//////////////////////////////////////////////////////////////////////////////
	// ���񂾏ꍇ�̏���
	//////////////////////////////////////////////////////////////////////////////
	AddScore(g_Enemy[enemy].killScore);								// ���_
	PlaySound(SOUND_LABEL_SE_EXPLOSION000 + rand() % 2);			// ���U���������_���Ƀv���C

	// �G�l�~�[�ɂ���Ď��s
	switch (g_Enemy[enemy].type) {

	case ENEMY_TYPE00:
		g_Enemy[enemy].isActive = FALSE;
		SetEffect(EFFECT_TYPE_ENEMY_EXPLOSION, g_Enemy[enemy].pos, 1);	// ���U�G�t�F�N�g
		if (rand() % 100 < g_Enemy[enemy].dropRate) {
			SetItem(ITEM_TYPE_HEART, g_Enemy[enemy].pos);				// �n�[�g�h���b�v
		}
		break;

	case ENEMY_TYPE01:
		g_Enemy[enemy].isActive = FALSE;
		SetEffect(EFFECT_TYPE_ENEMY_EXPLOSION, g_Enemy[enemy].pos, 1);	// ���U�G�t�F�N�g
		// �h���b�v���ɂ��
		if (rand() % 100 < g_Enemy[enemy].dropRate) {

			if (rand() % 100 < 70)
				SetItem(ITEM_TYPE_HEART, g_Enemy[enemy].pos);				// �n�[�g�h���b�v

			else
				SetItem(ITEM_TYPE_POWER, g_Enemy[enemy].pos);				// �p���[�h���b�v
		}
		break;

	case ENEMY_TYPE02:
		g_Enemy[enemy].isActive = FALSE;
		SetEffect(EFFECT_TYPE_ENEMY_EXPLOSION, g_Enemy[enemy].pos, 3);	// �L�т����U�G�t�F�N�g

		// �h���b�v���ɂ��
		if (rand() % 100 < g_Enemy[enemy].dropRate) {

			if (rand() % 100 < 20)
				SetItem(ITEM_TYPE_HEART, g_Enemy[enemy].pos);				// �n�[�g�h���b�v

			else
				SetItem(ITEM_TYPE_POWER, g_Enemy[enemy].pos);				// �p���[�h���b�v
		}
		break;

	case ENEMY_TYPE03:
		g_Enemy[enemy].isActive = FALSE;
		SetEffect(EFFECT_TYPE_ENEMY_EXPLOSION, g_Enemy[enemy].pos, 3);	// �L�т����U�G�t�F�N�g

		// �h���b�v���ɂ��
 		if (rand() % 100 < g_Enemy[enemy].dropRate) {

			if (rand() % 100 < 50)
				SetItem(ITEM_TYPE_HEART, g_Enemy[enemy].pos);				// �n�[�g�h���b�v

			else
				SetItem(ITEM_TYPE_POWER, g_Enemy[enemy].pos);				// �p���[�h���b�v
		}
		break;

	case ENEMY_TYPE_BOSS:
		// �Q�[���Z�b�g
		SetGamePhase(PHASE_ENTRO);
		// �X���[��������
		SetSlow(SLOW_BOSS_DIE, 120);
		// BGM���Ƃ߂�
		StopSound(SOUND_LABEL_BGM_BOSS);
		break;
	}

}

//=============================================================================
// �G�l�~�[���u�E����
//=============================================================================
void KillEnemy(int enemy) {
	AddEnemyHP(enemy, -99999);
}

//=============================================================================
// �{�X�s�����Z�b�g
//=============================================================================
void SetBossAction(int mode) {
	g_BossAction = mode;

	// �������[�h�ɓ��鏀��
	if (mode == BOSS_SINKING) {
		g_BossMovingCnt = 0;	// �p�����[�^�[�N���A
	}
}