#include "Csv.h"

#include<fstream>

Csv::Csv()
{
	dataList_.clear();
}

Csv::~Csv()
{
	for (int i = 0; i < dataList_.size(); i++)
		for (int j = 0; j < dataList_[i].size(); j++)
			dataList_[i][j].clear();
}

//CSV�t�@�C���̓ǂݍ���
bool Csv::Load(const std::string& fileName)
{
	std::ifstream file(fileName);									//�ǂݍ��ރt�@�C�����w��

	if (!file)	return false;										//�ǂݍ��ݕs�Ȃ牽�����Ȃ�

	//�t�@�C������1�s���f�[�^��ǂ݂���','��؂�ɂ��Ċi�[����
	std::string line = "";											//�s�f�[�^
	while (std::getline(file, line))								//1�s���ǂݍ���
		dataList_.push_back(Split(line, ','));						//','�ŕ������Ċi�[
	
	return true;
}

//�w��ʒu�̕�����f�[�^���擾
std::string Csv::GetStringDeta(int row, int column) const
{
	bool IS_NOT_EXIST = (column < 0) || (column >= GetWidth(row)) || (row < 0) || (row >= GetHeight());	//���݂���f�[�^��
	if (IS_NOT_EXIST) return "";																		//�Ȃ����""��Ԃ�

	return dataList_[row][column];
}

//�w��ʒu�̐����l�f�[�^���擾
int Csv::GetIntegerDeta(int row, int column) const
{
	return std::stoi(GetStringDeta(row, column).c_str());			
}

//�w��ʒu�̏����l�f�[�^���擾
float Csv::GetFloatDeta(int row, int column) const
{
	return std::stof(GetStringDeta(row, column).c_str());			
}

//�t�@�C���̗񐔂��擾
size_t Csv::GetWidth(int row) const
{
	return dataList_[row].size();
}

//�t�@�C���̍s�����擾
size_t Csv::GetHeight() const
{
	return dataList_.size();
}

//�w�蕶���ŕ�����𕪊�
std::vector<std::string> Csv::Split(std::string const& inData, char sign) const
{
	std::vector<std::string> resultList;									//��؂���������̃��X�g	
	int	first = 0;															//�������镶����̐擪
	int	last  = (int)inData.find_first_of(sign);							//�����͈͂̐擪�ɂ����؂蕶���̈ʒu

	//�������S�ċ�؂�܂Ń��[�v
	while (first < inData.size()) 
	{
		std::string substr = inData.substr(first, (size_t)last - first);	//first����last�܂ł̕�������擾
		resultList.push_back(substr);										//���X�g�Ɋi�[

		first = last + 1;													//������̐擪�������ς݂̋�؂蕶���̎��̈ʒu��
		last  = (int)inData.find_first_of(sign, (size_t)first);				//���m�F�͈̔͂ɂ���擪�̋�؂蕶����T��

		if (last == std::string::npos) last = (int)inData.size();			//��؂蕶����������Ȃ���Ε�����̏I�[�ŋ�؂�
	}

	return resultList;
}
