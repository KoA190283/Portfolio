#pragma once
#include<string>
#include<vector>

//CSV�t�@�C���̃f�[�^�������N���X
class Csv
{
private:

	std::vector<std::vector<std::string>> dataList_;		//�ǂݍ���CSV�t�@�C���̃f�[�^�𕪊��������X�g

public:

	Csv();
	~Csv();

	//CSV�t�@�C���̓ǂݍ���
	//�����FfileName	�ǂݍ��ރt�@�C���̖��O
	//�ߒl�F�ǂݍ��݂̐���
	bool Load(const std::string& fileName);
	
	//�w��ʒu��string�^�f�[�^���擾
	//�����P�Frow		�擾����f�[�^�̂���s
	//�����Q�Fcolumn	�擾����f�[�^�̂����
	//�ߒl�@�F�w��ʒu�̃f�[�^
	std::string GetStringDeta(int row, int column) const;

	//�w��ʒu��int�^�f�[�^���擾
	//�����P�Frow		�擾����f�[�^�̂���s
	//�����Q�Fcolumn	�擾����f�[�^�̂����
	//�ߒl�@�F�w��ʒu�̃f�[�^
	int GetIntegerDeta(int row, int column) const;

	//�w��ʒu��float�^�f�[�^���擾
	//�����P�Frow		�擾����f�[�^�̂���s
	//�����Q�Fcolumn	�擾����f�[�^�̂����
	//�ߒl�@�F�w��ʒu�̃f�[�^
	float GetFloatDeta(int row, int column) const;

	//�t�@�C���̗񐔂��擾
	//�����Frow	�񐔂��擾����s
	//�ߒl�F��
	size_t GetWidth(int row) const;

	//�t�@�C���̍s�����擾
	//�ߒl�F�s��
	size_t GetHeight() const;

private:

	//�w�蕶���ŕ�����𕪊�
	//�����P�FinData	�������镶����
	//�����Q�Fsign		��؂蕶��
	//�ߒl�@�F�������������񃊃X�g
	std::vector<std::string> Split(std::string const& inData, char sign) const;
};

