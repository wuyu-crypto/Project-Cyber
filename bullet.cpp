//=============================================================================
//
// �o���b�g���� [bullet.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "bullet.h"
#include "collision.h"
#include "score.h"
#include "bg.h"
#include "effect.h"
#include "sound.h"
#include "stop.h"
#include "slow.h"
#include "item.h"
#include "waveController.h"
#include "math.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define			TEXTURE_MAX			BULLET_TYPE_MAX

#define			DELAY				8						// �������`��x��

#define			BEZIER_OFFSET		500						// �x�W�F�Ȑ��̐���_�̃Y���

// �`�惂�[�h�ԍ�
enum {

	DRAW_DEFAULT,				// �f�t�H���g
	DRAW_ADD,					// ���Z

	DRAW_MAX,
};


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static BULLET_DATA g_BulletData[TEXTURE_MAX] = {

	{	// �v���C���[�ʏ�e
		"data/TEXTURE/bullet00.png",							// �e�N�X�`����
		XMINT2(50, 50), DRAW_DEFAULT, 1.0f,						// �L�����T�C�Y, �`��, ��
		XMFLOAT3(60.0f, 0.0f, 0.0f),							// �X�s�[�h
		XMINT2(1, 1), 1, 4,										// ������, �p�^�[����, �E�F�C�g
		40, 40,													// �����蔻��
	},

	{	// �G�l�~�[�ʏ�e
		"data/TEXTURE/bullet01.png",
		XMINT2(50, 50), DRAW_DEFAULT, 1.0f,
		XMFLOAT3(-6.5f, 0.0f, 0.0f),
		XMINT2(1, 1), 1, 4,
		20, 20,
	},

	{	// �G�l�~�[�X���[�e
		"data/TEXTURE/bullet01.png",
		XMINT2(50, 50), DRAW_DEFAULT, 1.0f,
		XMFLOAT3(-5.0f, 0.0f, 0.0f),
		XMINT2(1, 1), 1, 4,
		20, 20,
	},

#ifdef BEZIER
	{	// �G�l�~�[�x�W�F�e
		"data/TEXTURE/bullet01.png",
		XMINT2(50, 50), DRAW_DEFAULT, 1.0f,
		XMFLOAT3(0.0f, 0.0f, 0.0f),			// �X�s�[�h�͎Q�Ƃ��Ȃ�
		XMINT2(1, 1), 1, 4,
		20, 20,
	},
#endif

};

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static BULLET	g_Bullet[BULLET_MAX];	// �o���b�g�\����

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBullet(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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

	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// �o���b�g�\���̂̏�����
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].isActive   = FALSE;
		g_Bullet[i].isDelayed = FALSE;
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateBullet(void)
{
	for (int i = 0; i < BULLET_MAX; i++) {

		// �����Ȃ���s���Ȃ�
		if (g_Bullet[i].isActive == FALSE) {
			continue;
		}

		int type = g_Bullet[i].type;



		////////////////////////////////////////////////////////////////////////////
		// �A�j���[�V����
		////////////////////////////////////////////////////////////////////////////
		g_Bullet[i].animFrameCnt++;
		if ((g_Bullet[i].animFrameCnt % g_BulletData[type].wait) == 0)
		{
			// �p�^�[���̐؂�ւ�
			g_Bullet[i].animPattern = (g_Bullet[i].animPattern + 1) % g_BulletData[type].patternNum;
		}




		/////////////////////////////////////////////////////////////////////////////
		// �`��x������
		/////////////////////////////////////////////////////////////////////////////
		if (g_Bullet[i].isDelayed == TRUE) {
			g_Bullet[i].delayCnt++;
			if (g_Bullet[i].delayCnt == DELAY) {
				g_Bullet[i].isDelayed = FALSE;
			}
		}

		////////////////////////////////////////////////////////////////////////////
		// �ړ�����
		////////////////////////////////////////////////////////////////////////////
#ifdef BEZIER
		// �x�W�F
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

			// �J�E���^��i�߂�
			g_Bullet[i].bezierCnt++;
		}

		// ��
		else {
			XMVECTOR pos = XMLoadFloat3(&g_Bullet[i].pos);
			XMVECTOR speed = XMLoadFloat3(&g_Bullet[i].speed);
			pos += speed;
			XMStoreFloat3(&g_Bullet[i].pos, pos);

			// ��ʂ��o���e�͏���(�e�̔������o��܂�)
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

		// ��ʂ��o���e�͏���(�e�̔������o��܂�)
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
		// �����蔻��
		////////////////////////////////////////////////////////////////////////////
		if (GetGamePhase() == PHASE_ENTRO) {		// �Q�[���Z�b�g�������瓖���蔻������Ȃ�
			continue;
		}

		// �e���G�l�~�[
		{
			ENEMY* enemy = GetEnemy();

			// �G�l�~�[�̐��������蔻����s��
			for (int j = 0; j < ENEMY_MAX; j++)
			{
				// �����Ă�G�l�~�[�Ɠ����蔻�������
				if (enemy[j].isActive == FALSE) {
					continue;
				}

				BOOL collision = CollisionBB(g_Bullet[i].pos, g_BulletData[type].colliderW, g_BulletData[type].colliderH,
					enemy[j].pos, enemy[j].colliderW, enemy[j].colliderH);

				if (collision == TRUE) {

					// �v���C���[�̒e�Ƃ���ȊO�ɕ�����
					switch (g_Bullet[i].type) {

					case BULLET_TYPE_PLAYER_NORMAL:
						g_Bullet[i].isActive = FALSE;		// �e������
						SetEffect(EFFECT_TYPE_ENEMY_HIT, g_Bullet[i].pos, 1);	// ��e�G�t�F�N�g
						AddEnemyHP(j, -1);					// �G�l�~�[�Ƀ_���[�W
						break;

					default:
						break;
					}
				}
			}
		}

		// �e���v���C���[
		{
			PLAYER* player = GetPlayer();

			for (int j = 0; j < PLAYER_MAX; j++)
			{
				// �����Ă�v���C���[�Ɠ����蔻�������
				if (player[j].isActive == TRUE)
				{
					BOOL collision = CollisionBB(g_Bullet[i].pos, g_BulletData[type].colliderW, g_BulletData[type].colliderH,
						player[j].pos, player[j].colliderW, player[j].colliderH);

					if (collision == TRUE) {

						// �G�l�~�[�̒e�Ƃ���ȊO�ɕ�����
						switch (g_Bullet[i].type) {

						case BULLET_TYPE_ENEMY_NORMAL:
						case BULLET_TYPE_ENEMY_SLOW:
#ifdef BEZIER
						case BULLET_TYPE_ENEMY_BEZIER:
#endif
							g_Bullet[i].isActive = FALSE;		// �e������

							// ���łɔ�e������Ȃ����
							if (player[j].isDamaged == FALSE) {
								PlaySound(SOUND_LABEL_SE_PLAYER_HIT);					// ��e�����v���C
								SetEffect(EFFECT_TYPE_PLAYER_HIT, g_Bullet[i].pos, 1);	// ��e�G�t�F�N�g

								AddPlayerHP(j, -1);				// �v���C���[�Ƀ_���[�W
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
// �`�揈��
//=============================================================================
void DrawBullet(void)
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
	for (int i = 0; i < BULLET_MAX; i++) {

		// �����Ȃ���s���Ȃ�
		if (g_Bullet[i].isActive == FALSE) {
			continue;
		}

		// �`��x���H
		if (g_Bullet[i].isDelayed == TRUE) {
			continue;
		}



		int type = g_Bullet[i].type;

		////////////////////////////////////////////////////////////////////////////
		// DEFAULT
		////////////////////////////////////////////////////////////////////////////
		if (g_BulletData[type].mode == DRAW_DEFAULT) {

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[type]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Bullet[i].pos.x - bg->pos.x;		// BG��Ǐ]
			float py = g_Bullet[i].pos.y - bg->pos.y;		//
			float pw = g_Bullet[i].w;
			float ph = g_Bullet[i].h;

			float tw = 1.0f / g_BulletData[type].patternDivide.x;	// �e�N�X�`���̕�
			float th = 1.0f / g_BulletData[type].patternDivide.y;	// �e�N�X�`���̍���
			float tx = (float)(g_Bullet[i].animPattern % g_BulletData[type].patternDivide.x) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Bullet[i].animPattern / g_BulletData[type].patternDivide.x) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, g_BulletData[type].alpha),
				g_Bullet[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

		////////////////////////////////////////////////////////////////////////////
		// ADD
		////////////////////////////////////////////////////////////////////////////
		else if (g_BulletData[type].mode == DRAW_ADD) {

			SetBlendState(BLEND_MODE_ADD);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[type]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Bullet[i].pos.x - bg->pos.x;
			float py = g_Bullet[i].pos.y - bg->pos.y;
			float pw = g_Bullet[i].w;
			float ph = g_Bullet[i].h;

			float tw = 1.0f / g_BulletData[type].patternDivide.x;	// �e�N�X�`���̕�
			float th = 1.0f / g_BulletData[type].patternDivide.y;	// �e�N�X�`���̍���
			float tx = (float)(g_Bullet[i].animPattern % g_BulletData[type].patternDivide.x) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Bullet[i].animPattern / g_BulletData[type].patternDivide.x) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, g_BulletData[type].alpha),
				g_Bullet[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			SetBlendState(BLEND_MODE_ALPHABLEND);
		}
	}
}

//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
BULLET *GetBullet(void)
{
	return &g_Bullet[0];
}

//=============================================================================
// �o���b�g�f�[�^�\���̂̐擪�A�h���X���擾
//=============================================================================
BULLET_DATA* GetBulletData(void)
{
	return &g_BulletData[0];
}

//=============================================================================
// �o���b�g����(�f�t�H���g�X�s�[�h�ƕ���)
//=============================================================================
void SetBullet(int type, XMFLOAT3 pos)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		// ���g�p�X���b�g������
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

		return;			// 1���Z�b�g�ł�����I��
	}
}

//=============================================================================
// �o���b�g����(�X�s�[�h�ƕ����w��)
//=============================================================================
void SetBullet(int type, XMFLOAT3 pos, XMFLOAT3 speed)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		// ���g�p�X���b�g������
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

		return;			// 1���Z�b�g�ł�����I��
	}
}

//=============================================================================
// �~�`�e������
//=============================================================================
void SetBarrage(int type, XMFLOAT3 pos, float degree, float startDegree) {

	// �p�x�ɂ���ČJ��Ԃ��񐔂��v�Z
	for (int i = 0; i < 360 / degree; i++) {

		// ����̊p�x�����߂�
		float _degree = degree * i + startDegree;

		// �p�x�����W�A���ɕϊ�
		float rad = XMConvertToRadians(_degree);

		// ���W�A���ɂ��P�ʃx�N�g�����쐬
		XMFLOAT3 unit = XMFLOAT3(cosf(rad), sinf(rad), 0.0f);
		XMVECTOR xmvUnit = XMLoadFloat3(&unit);

		// �X�s�[�h�̐�Βl�����
		float length;
		if (g_BulletData[type].speed.x < 0.0f) {
			length = -g_BulletData[type].speed.x;
		}
		else {
			length = g_BulletData[type].speed.x;
		}

		// �P�ʃx�N�g�������̃X�s�[�h�̒����֐L�΂�
		XMVECTOR xmvLength = XMLoadFloat3(&XMFLOAT3(length, length, length));
		XMVECTOR xmvResult = xmvUnit * xmvLength;

		// �x�N�g�������ɖ߂�
		XMFLOAT3 result;
		XMStoreFloat3(&result, xmvResult);

		// �e���Z�b�g
		SetBullet(type, pos, result);

	}

}

//=============================================================================
// ��`�e������
//=============================================================================
void SetBarrage(int type, XMFLOAT3 pos, float degree, float startDegree, float endDegree) {

	// �p�x�ɂ���ČJ��Ԃ��񐔂��v�Z
	for (int i = 0; i < 360 / degree; i++) {

		// ����̊p�x�����߂�
		float _degree = degree * i + startDegree;

		// endDegree���z����ΏI��
		if (_degree > endDegree) {
			return;
		}

		// �p�x�����W�A���ɕϊ�
		float rad = XMConvertToRadians(_degree);

		// ���W�A���ɂ��P�ʃx�N�g�����쐬
		XMFLOAT3 unit = XMFLOAT3(cosf(rad), sinf(rad), 0.0f);
		XMVECTOR xmvUnit = XMLoadFloat3(&unit);

		// �X�s�[�h�̐�Βl�����
		float length;
		if (g_BulletData[type].speed.x < 0.0f) {
			length = -g_BulletData[type].speed.x;
		}
		else {
			length = g_BulletData[type].speed.x;
		}

		// �P�ʃx�N�g�������̃X�s�[�h�̒����֐L�΂�
		XMVECTOR xmvLength = XMLoadFloat3(&XMFLOAT3(length, length, length));
		XMVECTOR xmvResult = xmvUnit * xmvLength;

		// �x�N�g�������ɖ߂�
		XMFLOAT3 result;
		XMStoreFloat3(&result, xmvResult);

		// �e���Z�b�g
		SetBullet(type, pos, result);

	}

}










void SetBezierBullet(int type, XMFLOAT3 start, XMFLOAT3 end, float t) {

	for (int i = 0; i < BULLET_MAX; i++)
	{
		// ���g�p�X���b�g������
		if (g_Bullet[i].isActive == TRUE) {
			continue;
		}

		g_Bullet[i].isActive = TRUE;
		g_Bullet[i].w = g_BulletData[type].size.x;
		g_Bullet[i].h = g_BulletData[type].size.y;

		g_Bullet[i].pos = start;	// �����ʒu
		g_Bullet[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);	// �g��Ȃ�

		g_Bullet[i].type = type;

		g_Bullet[i].animFrameCnt = 0;
		g_Bullet[i].animPattern = 0;

		g_Bullet[i].colliderW = g_BulletData[type].colliderW;
		g_Bullet[i].colliderH = g_BulletData[type].colliderH;

		// �x�W�F�p
		g_Bullet[i].start = start;
		g_Bullet[i].end = end;
		g_Bullet[i].bezierT = t;
		g_Bullet[i].bezierCnt = 0.0f;

		return;			// 1���Z�b�g�ł�����I��
	}

}