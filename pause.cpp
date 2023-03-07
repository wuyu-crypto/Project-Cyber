//=============================================================================
//
// PAUSE [pause.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "pause.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	TEXTURE_SIZE_W				512.0f			// �L�����T�C�Y
#define	TEXTURE_SIZE_H				64.0f			//

// �e�N�X�`���ԍ�
enum {
	TEXTURE_BG,
	TEXTURE_RESUME,
	TEXTURE_RESTART,
	TEXTURE_BACK_TO_TITLE,
	TEXTURE_EXIT_GAME,
	TEXTURE_MANUAL,
	TEXTURE_MANUAL_PAD,

	TEXTURE_MAX,
};

// �{�^���ԍ�
enum {
	RESUME,
	RESTART,
	BACK_TO_TITLE,
	EXIT_GAME,

	BUTTON_MAX,
};
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/whiteSprite.png",
	"data/TEXTURE/pauseButton00.png",
	"data/TEXTURE/pauseButton01.png",
	"data/TEXTURE/pauseButton02.png",
	"data/TEXTURE/pauseButton03.png",
	"data/TEXTURE/manual.png",
	"data/TEXTURE/manual_pad.png",
};

static BOOL		g_Load = FALSE;

static int		g_ButtonNow;			// ���J�[�\���̋���{�^��

static XMFLOAT3 g_DefaultPos;			// �{�^������W
static XMFLOAT3 g_Pos[BUTTON_MAX];		// �{�^�����W
static XMFLOAT3 g_PosDiff;				// �{�^�����W�̊Ԋu
static float	g_Scale;				// �A�N�e�B�u�{�^���̊g�嗦

static bool isPadActive;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPause(void)
{
	ID3D11Device* pDevice = GetDevice();

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
	float diff = 0.16f;		// �Ԋu
	g_DefaultPos = XMFLOAT3(SCREEN_WIDTH * 0.3f, SCREEN_HEIGHT * (diff * (1.5f)), 0.0f);	// ��{���W
	g_PosDiff = XMFLOAT3(0.0f, SCREEN_HEIGHT * diff, 0.0f);		// �Ԋu

	g_Pos[RESUME] = g_DefaultPos;
	for (int i = 0; i < BUTTON_MAX; i++) {

		if (i > RESUME) {
			XMVECTOR pos = XMLoadFloat3(&g_Pos[i - 1]);
			XMVECTOR diff = XMLoadFloat3(&g_PosDiff);

			XMStoreFloat3(&g_Pos[i], pos + diff);
		}
	}

	g_ButtonNow = 0;
	g_Scale = 1.2f;

	isPadActive = IsPadActive();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPause(void)
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
void UpdatePause(void)
{
	// �{�^������
	if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_DOWN)) {
		g_ButtonNow = (g_ButtonNow + 1) % BUTTON_MAX;
		// �{�^���ړ���
		PlaySound(SOUND_LABEL_SE_BUTTON_MOVE);
	}
	if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP)) {
		g_ButtonNow = (g_ButtonNow + BUTTON_MAX - 1) % BUTTON_MAX;
		// �{�^���ړ���
		PlaySound(SOUND_LABEL_SE_BUTTON_MOVE);
	}
	if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_B)) {
		// PAUSE���I��
		SetPause(FALSE);
		// ���艹
		PlaySound(SOUND_LABEL_SE_CANCEL);
		// ���t���b�V��
		return;
	}

	// ��ʑJ��
	if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_A)) {
		switch (g_ButtonNow) {
		case RESUME:
			// PAUSE���I��
			SetPause(FALSE);
			// ���艹
			PlaySound(SOUND_LABEL_SE_CANCEL);
			// ���d�g���K�[�h�~
			return;
			break;

		case RESTART:
			// PAUSE���I��
			SetPause(FALSE);
			// GAME MODE���ĊJ
			SetFade(FADE_OUT, MODE_GAME);
			// ���艹
			PlaySound(SOUND_LABEL_SE_SELECT000);
			// ���d�g���K�[�h�~
			return;
			break;

		case BACK_TO_TITLE:
			// PAUSE���I��
			SetPause(FALSE);
			// �^�C�g����
			SetFade(FADE_OUT, MODE_TITLE);
			// ���艹
			PlaySound(SOUND_LABEL_SE_SELECT001);
			// ���d�g���K�[�h�~
			return;
			break;

		case EXIT_GAME:
			// EXIT��ʂ��Z�b�g
			SetExit(TRUE);
			// ���艹
			PlaySound(SOUND_LABEL_SE_SELECT001);
			// ���d�g���K�[�h�~
			return;
			break;

		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPause(void)
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
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BG]);

		SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f));		// �������X�v���C�g���Z�b�g

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// BUTTON
	for (int i = 0; i < BUTTON_MAX; i++) {

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i + 1]);

		float px = g_Pos[i].x;
		float py = g_Pos[i].y;

		float pw, ph;
		// �A�N�e�B�u�{�^���H
		if (i == g_ButtonNow) {
			pw = TEXTURE_SIZE_W * g_Scale;
			ph = TEXTURE_SIZE_H * g_Scale;
		}
		// ����ȊO
		else {
			pw = TEXTURE_SIZE_W;
			ph = TEXTURE_SIZE_H;
		}

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer,
			px, py, pw, ph,
			0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// MANUAL
	{
		// �e�N�X�`���ݒ�
		if (isPadActive) {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MANUAL_PAD]);
		}
		else {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MANUAL]);
		}

		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);
		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}