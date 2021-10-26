#include "Image.h"

#include<string>
#include<vector>

#include"Input.h"
#include"Global.h"

namespace Image
{
	namespace
	{
		//�摜�f�[�^
		struct ImageData
		{
			std::string		fileName;	//�t�@�C�����i�p�X�j
			Transform		transform;	//��ԁi�ʒu�A�g��k���Ȃǁj
			Sprite*			pSprite;	//�摜�f�[�^

			//�R���X�g���N�^
			ImageData() :pSprite(nullptr)
			{}
		};

		std::vector<ImageData*> datas_;	//�摜�f�[�^�̏W����
	}

	//�摜�f�[�^�̓ǂݍ���
	int Load(const std::string& fileName)
	{
		//���[�h�ς݃f�[�^�������炻�̃f�[�^�̃n���h����Ԃ�
		const int HANDLE = GetHandle(fileName);
		if (HANDLE != ERROR_CORD)	return HANDLE;
		
		//�摜�f�[�^�̃��[�h
		ImageData* pData = new ImageData;
		pData->pSprite	 = new Sprite;
		if (FAILED(pData->pSprite->Initialize(fileName)))
		{
			SAFE_DELETE(pData->pSprite);
			SAFE_DELETE(pData);
			return ERROR_CORD;
		}

		//�t�@�C������ݒ�
		pData->fileName = fileName;

		//�摜�f�[�^���i�[
		datas_.push_back(pData);
		
		//�n���h����Ԃ�
		return (int)datas_.size() - 1;
	}
	
	//�\�������Ԃ�ݒ�
	void SetTransform(int handle, Transform& transform)
	{
		datas_[handle]->transform = transform;
	}
	
	//�`��
	void Draw(int handle)
	{
		//�g�p����V�F�[�_�[�̎w�肪�Ȃ����DEFAULT_2D�ŕ`��
		Draw(handle, Direct3D::SHADER_TYPE::DEFAULT_2D);
	}

	//�`��
	void Draw(int handle, Direct3D::SHADER_TYPE type)
	{
		//�g�p����V�F�[�_�[�̃Z�b�g���w�肷��
		Direct3D::SetShaderBundle(type);

		//�ϊ��s����Čv�Z
		datas_[handle]->transform.Calclation();

		//�`��
		datas_[handle]->pSprite->Draw(datas_[handle]->transform);
	}
	
	//���R���̕`��
	void DrawNaturalNumber(int handle[DECIMAL_BASE], int drawNumber, Transform const& lastDigitTransform, float intervalWide)
	{
		const std::string DRAW_NUMBERS = std::to_string(drawNumber);			//�`�悷�鐔���𕶎���ɕϊ���������

		//�`�悷��摜�̕ό`��Ԃ����߂�
		Transform transform			=  lastDigitTransform;						//�`�悷��ό`���
		transform.position_.vecX	-= DRAW_NUMBERS.size() * intervalWide;		//position���ŉ��ʌ��̕`��ʒu�ɂȂ�悤�ɕ`��ʒu�����炷
		transform.scale_			= lastDigitTransform.scale_;				//�����̊g��T�C�Y�̐ݒ�

		//��������ŏ�ʌ�����ꌅ���`�悷��
		for (auto number : DRAW_NUMBERS) 
		{
			//�`��
			int DRAW_NUMBER = number - '0';										//�`�悷�鐔��
			Image::SetTransform(handle[DRAW_NUMBER], transform);
			Image::Draw(handle[DRAW_NUMBER]);

			//1�����ʒu�����炷
			transform.position_.vecX += intervalWide;
		}
	}

	//���[�h�ς݉摜�̃n���h���ԍ����擾����
	int GetHandle(std::string const& fileName)
	{
		//���[�h�ς݂̉摜�f�[�^����t�@�C��������v������̂�T���ăn���h���ԍ���Ԃ�
		for (int i = 0; i < datas_.size(); i++)
			if (datas_[i]->fileName == fileName) return i;

		//���݂��Ȃ����-1��Ԃ�
		return ERROR_CORD;
	}

	//�����擾
	float GetWidth(int handle)
	{
		return datas_[handle]->pSprite->GetWidth() * datas_[handle]->transform.scale_.vecX;
	}

	//�������擾
	float GetHeight(int handle)
	{
		return datas_[handle]->pSprite->GetHeight() * datas_[handle]->transform.scale_.vecY;
	}

	//�摜���N���b�N����Ă��邩
	bool IsClicked(int handle)
	{
		if (!Input::IsMouseButtonDown(0))	return false;									//�E�N���b�N����Ă��Ȃ���Ή������Ȃ�

		const float HALF = 0.5f;															//����	

		//�}�E�X�̈ʒu��ݒ�
		XMVECTOR mousePosition	=  Input::GetMousePosition();								//�}�E�X�̈ʒu
		mousePosition.vecX		-= Global::WINDOW_WIDTH * HALF;								//X���W���E�B���h�E�̕��Œ���
		mousePosition.vecY		-= Global::WINDOW_HEIGHT * HALF;							//Y���W���E�B���h�E�̍����Œ���
		mousePosition.vecY		*= -1.0f;													//Y���W�𔽓]����

		//�摜�̏����擾
		const float	HALF_IMAGE_WIDTH	= GetWidth(handle) * HALF;							//�`��摜�̔����̕�
		const float HALF_IMAGE_HEIGHT	= GetHeight(handle) * HALF;							//�`��摜�̔����̍���

		//�摜�̈ʒu��ݒ�
		XMVECTOR ImagePosition	=  datas_[handle]->transform.position_;						//�摜�̈ʒu															
		ImagePosition.vecX		*= Global::WINDOW_WIDTH * HALF;								//X���W���E�B���h�E�̕��Œ���	
		ImagePosition.vecY		*= Global::WINDOW_HEIGHT * HALF;							//Y���W���E�B���h�E�̍����Œ���

		//�摜�̊O���Ƀ}�E�X�����邩���m�F���O�Ȃ�false��Ԃ�
		if (mousePosition.vecY > ImagePosition.vecY + HALF_IMAGE_HEIGHT)	return false;	//��
		if (mousePosition.vecY < ImagePosition.vecY - HALF_IMAGE_HEIGHT)	return false;	//��
		if (mousePosition.vecX < ImagePosition.vecX - HALF_IMAGE_WIDTH)		return false;	//��
		if (mousePosition.vecX > ImagePosition.vecX + HALF_IMAGE_WIDTH)		return false;	//�E

		//�N���b�N����Ă���
		return true;
	}

	//���
	void Release()
	{
		auto it = datas_.begin();
		while (it != datas_.end())
		{
			SAFE_DELETE((*it)->pSprite);
			SAFE_DELETE((*it));
			it++;
		}
		datas_.clear();
	}
}