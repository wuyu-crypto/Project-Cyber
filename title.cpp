//=============================================================================
//
// �^�C�g�� [title.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX					TITLE_MAX		// �e�N�X�`���̐�

#define	BUTTON_WIDTH				512.0f			// �f�t�H���g�{�^���T�C�Y
#define	BUTTON_HEIGHT				64.0f			//
#define	BUTTON_SCALE				1.28f			// �A�N�e�B�u�{�^���̔{��

#define	PERIOD						128				// �_�ł������

// �e�N�X�`���ԍ�
enum {
	BG,
	GLOW,
	LOGO,
	BUTTON_START_GAME,
	BUTTON_CREDITS,
	BUTTON_EXIT,
	CREDITS_BG,
	CREDITS,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title00.png",
	"data/TEXTURE/title01.png",
	"data/TEXTURE/title02.png",
	"data/TEXTURE/titleButton00.png",
	"data/TEXTURE/titleButton01.png",
	"data/TEXTURE/titleButton02.png",
	"data/TEXTURE/whiteSprite.png",
	"data/TEXTURE/credits.png",
};

static BOOL		g_Load = FALSE;

static int		g_ButtonNow;			// ���J�[�\���̋���{�^��

static XMFLOAT3 g_DefaultButtonPos;					// �{�^������W
static XMFLOAT3 g_ButtonPosDiff;					// �{�^�����W�̊Ԋu
static XMFLOAT3 g_ButtonPos[TITLE_BUTTON_MAX];		// �{�^�����W
static float	g_ButtonW[TITLE_BUTTON_MAX];		// �{�^���̃T�C�Y
static float	g_ButtonH[TITLE_BUTTON_MAX];		// 


static BOOL		g_IsCreditsActive;	// CREDITS��ʃt���O

static float	g_CreditsW = 512;	// CREDITS�e�N�X�`���T�C�Y
static float	g_CreditsH = 1800;	//

static float	g_CreditsWindowW;	// CREDITS�g�T�C�Y
static float	g_CreditsWindowH;	//
static float	g_CreditsWindowY;	// CREDITS�\���g�̍��W(0�`g_CreditsH-g_CreditsWindowH)

static float	g_CreditsScrl;		// CREDITS�X�N���[����


static int						g_Cnt;
static float					g_Alpha;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
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


	// �{�^���֘A�̏�����
	g_DefaultButtonPos = XMFLOAT3(1445.0f, 530.0f, 0.0f);
	g_ButtonPosDiff = XMFLOAT3(0.0f, 150.0f, 0.0f);
	g_ButtonPos[TITLE_START_GAME] = g_DefaultButtonPos;
	for (int i = 0; i < TITLE_BUTTON_MAX; i++) {

		if (i > TITLE_START_GAME) {
			XMVECTOR pos = XMLoadFloat3(&g_ButtonPos[i - 1]);
			XMVECTOR diff = XMLoadFloat3(&g_ButtonPosDiff);

			XMStoreFloat3(&g_ButtonPos[i], pos + diff);
		}

		g_ButtonW[i] = BUTTON_WIDTH;
		g_ButtonH[i] = BUTTON_HEIGHT;
	}


	g_ButtonNow = TITLE_START_GAME;

	g_IsCreditsActive = FALSE;
	g_CreditsWindowW = g_CreditsW;	// �g�̃T�C�Y
	g_CreditsWindowH = 800.0f;		//
	g_CreditsWindowY = 0.0f;		// �e�N�X�`���g��Y���W
	g_CreditsScrl = 0.01f;			// �X�N���[����(0�`1)



	g_Cnt = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{
	//////////////////////////////////////////////////////////////////////////////
	// �J�E���^�Ń�����
	//////////////////////////////////////////////////////////////////////////////
	{
		g_Cnt = (g_Cnt + 1) % PERIOD;

		float ratio = (float)g_Cnt / (float)PERIOD;

		// �O���t��������( y=1-(2x-1)^2 )
		ratio = 1 -
			(2 * ratio - 1) * (2 * ratio - 1);

		g_Alpha = ratio;
	}

	//////////////////////////////////////////////////////////////////////////////
	// CREDITS����
	//////////////////////////////////////////////////////////////////////////////
	if (g_IsCreditsActive == TRUE) {

		// �X�N���[������
		if (GetKeyboardPress(DIK_DOWN) || IsButtonPressed(0, BUTTON_DOWN))
		{
			g_CreditsWindowY += g_CreditsScrl;
		}
		else if (GetKeyboardPress(DIK_UP) || IsButtonPressed(0, BUTTON_UP))
		{
			g_CreditsWindowY -= g_CreditsScrl;
		}

		// ��ʊO����
		if (g_CreditsWindowY < 0.0f) {	// �ŏ㕔�܂ŃX�N���[�������H
			g_CreditsWindowY = 0.0f;
		}

		if (g_CreditsWindowY > 1.0f - g_CreditsWindowH / g_CreditsH) {	// �ŉ����܂ŃX�N���[�������H
			g_CreditsWindowY = 1.0f - g_CreditsWindowH / g_CreditsH;
		}

		// ����
		if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_SPACE) || GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_A) || IsButtonTriggered(0, BUTTON_B)) {
			// ���艹
			PlaySound(SOUND_LABEL_SE_CANCEL);
			// CREDITS������
			g_IsCreditsActive = FALSE;
		}
		return;		// �I�����ăL�[���͂����t���b�V��
	}

	//////////////////////////////////////////////////////////////////////////////
	// �{�^�����͏���
	//////////////////////////////////////////////////////////////////////////////
	if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_DOWN)) {
		g_ButtonNow = (g_ButtonNow + 1) % TITLE_BUTTON_MAX;
		// �{�^���ړ���
		PlaySound(SOUND_LABEL_SE_BUTTON_MOVE);
	}
	if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP)) {
		g_ButtonNow = (g_ButtonNow + TITLE_BUTTON_MAX - 1) % TITLE_BUTTON_MAX;
		// �{�^���ړ���
		PlaySound(SOUND_LABEL_SE_BUTTON_MOVE);
	}
	if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_X)) {
		// EXIT��ʂ�\��
		SetExit(TRUE);
		// ���艹
		PlaySound(SOUND_LABEL_SE_SELECT001);
		// ���t���b�V��
		return;
	}

	//////////////////////////////////////////////////////////////////////////////
	// ��ʑJ��
	//////////////////////////////////////////////////////////////////////////////
	if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_A)) {
		switch (g_ButtonNow) {

		case TITLE_START_GAME:
			SetFade(FADE_OUT, MODE_GAME);
			// ���艹
			PlaySound(SOUND_LABEL_SE_SELECT000);
			// ���d�g���K�[�h�~
			return;
			break;

		case TITLE_CREDITS:
			// CREDITS��\��
			g_IsCreditsActive = TRUE;
			// ���艹
			PlaySound(SOUND_LABEL_SE_SELECT001);
			// ���d�g���K�[�h�~
			return;
			break;

		case TITLE_EXIT:
			// EXIT��ʂ�\��
			SetExit(TRUE);
			// ���艹
			PlaySound(SOUND_LABEL_SE_SELECT001);
			// ���d�g���K�[�h�~
			return;
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	// �{�^������
	//////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < TITLE_BUTTON_MAX; i++) {
		g_ButtonW[i] = BUTTON_WIDTH;
		g_ButtonH[i] = BUTTON_HEIGHT;
	}

	g_ButtonW[g_ButtonNow] = BUTTON_WIDTH * BUTTON_SCALE;
	g_ButtonH[g_ButtonNow] = BUTTON_HEIGHT * BUTTON_SCALE;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
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

	// BG
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BG]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// GLOW
	{
		SetBlendState(BLEND_MODE_ADD);
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GLOW]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_Alpha));

		// 2��`��
		for (int i = 0; i < 1; i++) {
			GetDeviceContext()->Draw(4, 0);
		}

		SetBlendState(BLEND_MODE_ALPHABLEND);
	}

	// LOGO
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[LOGO]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// BUTTON
	for (int i = 0; i < TITLE_BUTTON_MAX; i++) {

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i + BUTTON_START_GAME]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer,
			g_ButtonPos[i].x, g_ButtonPos[i].y, g_ButtonW[i], g_ButtonH[i],
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// CREDITS
	if (g_IsCreditsActive == TRUE) {

		// �w�i��ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[CREDITS_BG]);
		SetSpriteLTColor(g_VertexBuffer,
			0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f));		// �^����������
		GetDeviceContext()->Draw(4, 0);



		// �g��CREDITS��\��
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[CREDITS]);

		float px = SCREEN_WIDTH * 0.5f;
		float py = SCREEN_HEIGHT * 0.5f;
		float pw = g_CreditsWindowW;
		float ph = g_CreditsWindowH;

		float tx = 0.0f;
		float ty = g_CreditsWindowY;
		float tw = 1.0f;
		float th = g_CreditsWindowH / g_CreditsH;

		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		GetDeviceContext()->Draw(4, 0);

	}

}





