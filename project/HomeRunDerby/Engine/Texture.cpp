#include "Texture.h"

#include "Direct3D.h"
#include "Global.h"

//������
Texture::Texture():pSampler_(nullptr), pSRV_(nullptr),imgWidth_(0), imgHeight_(0)
{
}

Texture::~Texture()
{
	Release();
}

//�摜�t�@�C���̃��[�h
HRESULT Texture::Load(const std::string& fileName)
{
	IWICFormatConverter* pFormatConverter = nullptr;	//�R���o�[�^�[

	//�R���o�[�^�[�̏�����
	if (FAILED(InitCOM(fileName, pFormatConverter)))
	{
		MessageBox(nullptr, "2D�e�L�X�`���[�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	//�摜�T�C�Y�𒲂ׂ�
	pFormatConverter->GetSize(&imgWidth_, &imgHeight_);

	//�e�N�X�`�����쐬
	ID3D11Texture2D*	pTexture;
	CreateTexture(pTexture);

	//�e�N�X�`�����R���e�L�X�g�ɓn��
	ID3D11DeviceContext*	 pContext = Direct3D::GetContext();
	D3D11_MAPPED_SUBRESOURCE hMappedres;
	pContext->Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &hMappedres);
	pFormatConverter->CopyPixels(nullptr, imgWidth_ * 4, imgWidth_ * imgHeight_ * 4, (BYTE*)hMappedres.pData);
	pContext->Unmap(pTexture, 0);

	//pTexture�̃f�[�^����K�v�Ȃ��̂𔲂��o���ď���������
	InitializeTexture2D(pTexture);

	//�ꎞ�I�Ɏ擾�����f�[�^�̉��
	SAFE_RELEASE(pTexture);

	return S_OK;
}

//�e�N�X�`���̏�����
HRESULT Texture::InitializeTexture2D(ID3D11Texture2D * pTexture)
{
	//�T���v���[�쐬
	if (FAILED(CreateSampler())) return E_FAIL;

	//�V�F�[�_�[���\�[�X�r���[�쐬
	if (FAILED(CreateShaderResourceView(pTexture))) return E_FAIL;

	return S_OK;
}

//�T���v���[�̎擾
ID3D11SamplerState* Texture::GetSampler() const
{
	return pSampler_;
}

//�V�F�[�_�[���\�[�X�r���[�̎擾
ID3D11ShaderResourceView* Texture::GetSRV() const
{
	return pSRV_;
}

//���̎擾
UINT Texture::GetWidth() const
{
	return imgWidth_; 
}

//�����̎擾
UINT Texture::GetHeight() const
{
	return imgHeight_; 
}

//���
void Texture::Release()
{
	SAFE_RELEASE(pSRV_);
	SAFE_RELEASE(pSampler_);
}

//�t�@�C�����J��
HRESULT Texture::InitCOM(const std::string& fileName, IWICFormatConverter*& pFormatConverter)
{
	//�t�@�C���������C�h������
	wchar_t wtext[FILENAME_MAX];
	size_t	ret;
	mbstowcs_s(&ret, wtext, fileName.c_str(), fileName.length());

	//COM�̏�����
	if (FAILED(CoInitialize(nullptr)))
	{
		MessageBox(nullptr, "COM�̏������Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	//COM�̃C���X�^���X
	IWICImagingFactory*		pFactory = nullptr;
	if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory, reinterpret_cast<void **>(&pFactory))))
	{
		MessageBox(nullptr, "COM�C���X�^���X�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	//�摜�t�@�C���̍쐬
	IWICBitmapDecoder*		pDecoder = nullptr;
	if (FAILED(pFactory->CreateDecoderFromFilename(wtext, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder)))
	{
		MessageBox(nullptr, "�摜�t�@�C���̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	//�R���o�[�^�[�̍쐬
	if (FAILED(pFactory->CreateFormatConverter(&pFormatConverter)))
	{
		MessageBox(nullptr, "�R���o�[�^�[�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	//�R���o�[�^�[�̏�����
	IWICBitmapFrameDecode*	pFrame = nullptr;
	pDecoder->GetFrame(0, &pFrame);
	pFormatConverter->Initialize(pFrame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, NULL, 1.0f, WICBitmapPaletteTypeMedianCut);

	//COM�̏I��
	CoUninitialize();

	return S_OK;
}

//�e�N�X�`���̍쐬
HRESULT Texture::CreateTexture(ID3D11Texture2D *& pTexture)
{
	D3D11_TEXTURE2D_DESC texdec;
	texdec.Width				= imgWidth_;					//�e�N�X�`���̕��i�e�N�Z���P�ʁj
	texdec.Height				= imgHeight_;					//�e�N�X�`���̍���
	texdec.MipLevels			= 1;							//�e�N�X�`�����̃~�b�v�}�b�v���x���̍ő吔
	texdec.ArraySize			= 1;							//�e�N�X�`���z����̃e�N�X�`���̐�
	texdec.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;	//�e�N�X�`���t�H�[�}�b�g
	texdec.SampleDesc.Count		= 1;							//�s�N�Z��������̃}���`�T���v���̐�
	texdec.SampleDesc.Quality	= 0;							//�掿���x��
	texdec.Usage				= D3D11_USAGE_DYNAMIC;			//�e�N�X�`���̓ǂݏ������@�����ʂ���l
	texdec.BindFlags			= D3D11_BIND_SHADER_RESOURCE;	//�p�C�v���C���X�e�[�W�Ƀo�C���h���邽�߂̃t���O
	texdec.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;		//�������CPU�A�N�Z�X�̃^�C�v���w�肷��t���O
	texdec.MiscFlags			= 0;							//���̂��܂��ʓI�ł͂Ȃ����\�[�X�I�v�V���������ʂ���t���O

	//2D�e�L�X�`���[�̍쐬
	if (FAILED(Direct3D::GetDevice()->CreateTexture2D(&texdec, nullptr, &pTexture)))
	{
		MessageBox(nullptr, "2D�e�L�X�`���[�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//�T���v���[�̍쐬
HRESULT Texture::CreateSampler()
{
	D3D11_SAMPLER_DESC  SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	SamDesc.Filter	 = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	//�e�N�X�`�����T���v�����O����Ƃ��Ɏg�p����t�B���^�����O���@
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		//0����1�͈̔͊O��au�e�N�X�`�����W���������邽�߂Ɏg�p���郁�\�b�h
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		//0����1�͈̔͊O��v�e�N�X�`�����W���������邽�߂Ɏg�p���郁�\�b�h
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		//0����1�͈̔͊O��w�e�N�X�`�����W���������邽�߂Ɏg�p���郁�\�b�h

	if (FAILED(Direct3D::GetDevice()->CreateSamplerState(&SamDesc, &pSampler_)))
	{
		MessageBox(nullptr, "�T���v���[�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//�V�F�[�_�[���\�[�X�r���[�̍쐬
HRESULT Texture::CreateShaderResourceView(ID3D11Texture2D * pTexture)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.Format				 = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv.ViewDimension		 = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels	 = 1;

	if (FAILED(Direct3D::GetDevice()->CreateShaderResourceView(pTexture, &srv, &pSRV_)))
	{
		MessageBox(nullptr, "�V�F�[�_�[���\�[�X�r���[�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}
