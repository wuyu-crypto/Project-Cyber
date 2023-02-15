//=============================================================================
//
// �G�t�F�N�g���� [effect.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "bg.h"
#include "sound.h"
#include "cutin.h"
#include "input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define		WAIT			8				// �t�H���g�A�j���[�V�����E�F�C�g�l

#define		FONT_SCALE		20.0f			// �t�H���g�g�k�̔{��(�傫���قǃf�t�H���g�T�C�Y���ێ�)

// �e�N�X�`���ԍ�
enum {

	BANNER,
	CHARA,
	LINE,
	FONT,

	TEXTURE_MAX,
};

// �J�b�g�C���A�j���[�V�����i�K
enum {
	STEP_INTRO,		// �o��(��M����A�j���[�V����)
	STEP_STAY,		// ���C���A�j���[�V����
	STEP_OUTRO,		// ����
	STEP_IDLING,	// ��~(�]�C�����Ԓ���)

	SETP_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void StopCutin(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/cutin00.png",
	"data/TEXTURE/cutin01.png",
	"data/TEXTURE/cutin02.png",
	"data/TEXTURE/cutin03.png",
};

static BOOL		g_Load = FALSE;										// ���������s�������̃t���O

static XMFLOAT3	g_BannerPos;	// �o�i�[���W(�����Ȃ�)

static XMFLOAT3 g_CharaPos;		// �L�������W(���փX���C�h)
static XMFLOAT3 g_CharaSpeed;	// �L�����̃X���C�h�X�s�[�h

static XMFLOAT3 g_FontPos;			// �t�H���g���W
static XMFLOAT3 g_FontPos_org;		// ��t�H���g���W(�����Ȃ�)
static XMFLOAT3 g_FontDiff;			// �t�H���g�ő�u������
static int		g_AnimCnt;			// �t�H���g�J�E���^
static XMFLOAT2	g_FontSize;			// �t�H���g�T�C�Y(�C����)

static XMFLOAT2 g_Size[TEXTURE_MAX] = {		// �L�����T�C�Y
	XMFLOAT2(SCREEN_WIDTH, 512.0f),
	XMFLOAT2(SCREEN_WIDTH, 512.0f),
	XMFLOAT2(SCREEN_WIDTH, 512.0f),
	XMFLOAT2(1024.0f, 512.0f),
};

static int		g_StepCnt;			// �A�j���[�V�����J�E���^
static int		g_CutinStep;	// �A�j���[�V�����i�K�t���O
static float	g_CutinStepNum[SETP_MAX] = {	// �e�i�K�̎��s�t���[����
	4,
	220,
	4,
	30,
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitCutin(void)
{
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++) {

		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
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



	// ����������
	g_BannerPos = XMFLOAT3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f);	// �����Ȃ�

	g_CharaPos = XMFLOAT3(SCREEN_WIDTH * 1.1f, SCREEN_HEIGHT * 0.5f, 0.0f);		// CHARA�������W
	g_CharaSpeed = XMFLOAT3(-1.5f, 0.0f, 0.0f);		// �X���C�h�X�s�[�h

	g_FontPos_org = XMFLOAT3(SCREEN_WIDTH * 0.3f, SCREEN_HEIGHT * 0.5f, 0.0f);		// FONT����W
	g_FontPos = g_FontPos_org;
	g_FontDiff = XMFLOAT3(20.0f, 20.0f, 0.0f);		// �ő�u������
	g_AnimCnt = 0;
	g_FontSize.x = g_Size[FONT].x;
	g_FontSize.y = g_Size[FONT].y;

	g_StepCnt = 0;

	g_CutinStep = STEP_INTRO;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateCutin(void)
{
	// �X�L�b�v
	if (GetKeyboardTrigger(DIK_RETURN)) {
		StopCutin();
		return;
	}

	//////////////////////////////////////////////////////////////////////////////
	// �i�K����
	//////////////////////////////////////////////////////////////////////////////
	g_StepCnt++;

	// INTRO
	if (g_CutinStep == STEP_INTRO) {
		if (g_StepCnt >= g_CutinStepNum[g_CutinStep]) {
			// �J�E���^�����Z�b�g
			g_StepCnt = 0;
			// �X�e�b�v��i�߂�
			g_CutinStep = STEP_STAY;
		}
	}
	// STAY
	else if (g_CutinStep == STEP_STAY) {
		if (g_StepCnt >= g_CutinStepNum[g_CutinStep]) {
			// �J�E���^�����Z�b�g
			g_StepCnt = 0;
			// �X�e�b�v��i�߂�
			g_CutinStep = STEP_OUTRO;
		}
	}
	// OUTRO
	else if (g_CutinStep == STEP_OUTRO) {
		if (g_StepCnt >= g_CutinStepNum[g_CutinStep]) {
			// �J�E���^�����Z�b�g
			g_StepCnt = 0;
			// �X�e�b�v��i�߂�
			g_CutinStep = STEP_IDLING;
		}
	}
	// IDLING
	else if (g_CutinStep == STEP_IDLING) {
		if (g_StepCnt >= g_CutinStepNum[g_CutinStep]) {
			StopCutin();
		}
		return;		// ����ȏ���s���Ȃ�
	}


	//////////////////////////////////////////////////////////////////////////////
	// �L�����X���C�h
	//////////////////////////////////////////////////////////////////////////////
	g_CharaPos.x += g_CharaSpeed.x;


	//////////////////////////////////////////////////////////////////////////////
	// �t�H���g����
	//////////////////////////////////////////////////////////////////////////////
	g_AnimCnt++;
	if (g_AnimCnt % WAIT == 0) {

		// �Z�b�g���鋗���ƕ����������_���Ɍ��߂�(�ő勗����0�`1�{)
		XMFLOAT3 diff = XMFLOAT3(
			g_FontDiff.x * sinf(rand()),
			g_FontDiff.y * sinf(rand()),
			0.0f);

		// ���W�����߂�
		g_FontPos.x = g_FontPos_org.x + diff.x;
		g_FontPos.y = g_FontPos_org.y + diff.y;

		// �T�C�Y�̔{�������߂�
		float ratio = 1.0f - sinf(rand()) / FONT_SCALE;

		// �T�C�Y���g�k
		g_FontSize.x = g_Size[FONT].x * ratio;
		g_FontSize.y = g_Size[FONT].y * ratio;
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawCutin(void)
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

	// STEP_INTRO
	if (g_CutinStep == STEP_INTRO) {
		// BANNER
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BANNER]);

			float px = g_BannerPos.x - bg->pos.x;			// BG��Ǐ]
			float py = g_BannerPos.y - bg->pos.y;			//
			float pw = g_Size[BANNER].x;
			float ph = g_Size[BANNER].y * ((float)g_StepCnt / g_CutinStepNum[STEP_INTRO]);	// �i�s�x

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
		// CHARA
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[CHARA]);

			float px = g_CharaPos.x - bg->pos.x;			// BG��Ǐ]
			float py = g_CharaPos.y - bg->pos.y;			//
			float pw = g_Size[CHARA].x;
			float ph = g_Size[CHARA].y * ((float)g_StepCnt / g_CutinStepNum[STEP_INTRO]);	// �i�s�x

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
	// STEP_STAY
	else if (g_CutinStep == STEP_STAY) {
		// BANNER
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BANNER]);

			float px = g_BannerPos.x - bg->pos.x;			// BG��Ǐ]
			float py = g_BannerPos.y - bg->pos.y;			//
			float pw = g_Size[BANNER].x;
			float ph = g_Size[BANNER].y;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
		// CHARA
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[CHARA]);

			float px = g_CharaPos.x - bg->pos.x;			// BG��Ǐ]
			float py = g_CharaPos.y - bg->pos.y;			//
			float pw = g_Size[CHARA].x;
			float ph = g_Size[CHARA].y;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
		// LINE
		{
			SetBlendState(BLEND_MODE_ADD);
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[LINE]);

			float px = g_BannerPos.x - bg->pos.x;			// BG��Ǐ]
			float py = g_BannerPos.y - bg->pos.y;			//
			float pw = g_Size[LINE].x;
			float ph = g_Size[LINE].y;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = sinf(rand());	// �����_���z�u�ŃV���V���V���b�̌���
			float ty = 0.0f;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// 2��`��
			GetDeviceContext()->Draw(4, 0);
			GetDeviceContext()->Draw(4, 0);

			SetBlendState(BLEND_MODE_ALPHABLEND);
		}
		// FONT
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[FONT]);

			float px = g_FontPos.x - bg->pos.x;			// BG��Ǐ]
			float py = g_FontPos.y - bg->pos.y;			//
			float pw = g_FontSize.x;
			float ph = g_FontSize.y;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
	// STEP_OUTRO
	else if (g_CutinStep == STEP_OUTRO) {
		// BANNER
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BANNER]);

			float px = g_BannerPos.x - bg->pos.x;			// BG��Ǐ]
			float py = g_BannerPos.y - bg->pos.y;			//
			float pw = g_Size[BANNER].x;
			float ph = g_Size[BANNER].y * (1.0f - (float)g_StepCnt / g_CutinStepNum[STEP_INTRO]);	// 1 - �i�s�x

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
		// CHARA
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[CHARA]);

			float px = g_CharaPos.x - bg->pos.x;			// BG��Ǐ]
			float py = g_CharaPos.y - bg->pos.y;			//
			float pw = g_Size[CHARA].x;
			float ph = g_Size[CHARA].y * (1.0f - (float)g_StepCnt / g_CutinStepNum[STEP_INTRO]);	// 1 - �i�s�x

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}

//=============================================================================
// CUTIN���I��
//=============================================================================
void StopCutin(void) {
	// CUTIN���I��
	SetCutin(FALSE);
	// SE��؂�
	StopSound(SOUND_LABEL_SE_CUTIN000);
	StopSound(SOUND_LABEL_SE_CUTIN001);
	// BOSS�Ȃ𗬂�
	PlaySound(SOUND_LABEL_BGM_BOSS);
}