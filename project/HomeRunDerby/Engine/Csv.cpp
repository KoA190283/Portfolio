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

//CSVファイルの読み込み
bool Csv::Load(const std::string& fileName)
{
	std::ifstream file(fileName);									//読み込むファイルを指定

	if (!file)	return false;										//読み込み不可なら何もしない

	//ファイルから1行ずつデータを読みこみ','区切りにして格納する
	std::string line = "";											//行データ
	while (std::getline(file, line))								//1行ずつ読み込む
		dataList_.push_back(Split(line, ','));						//','で分割して格納
	
	return true;
}

//指定位置の文字列データを取得
std::string Csv::GetStringDeta(int row, int column) const
{
	bool IS_NOT_EXIST = (column < 0) || (column >= GetWidth(row)) || (row < 0) || (row >= GetHeight());	//存在するデータか
	if (IS_NOT_EXIST) return "";																		//なければ""を返す

	return dataList_[row][column];
}

//指定位置の整数値データを取得
int Csv::GetIntegerDeta(int row, int column) const
{
	return std::stoi(GetStringDeta(row, column).c_str());			
}

//指定位置の小数値データを取得
float Csv::GetFloatDeta(int row, int column) const
{
	return std::stof(GetStringDeta(row, column).c_str());			
}

//ファイルの列数を取得
size_t Csv::GetWidth(int row) const
{
	return dataList_[row].size();
}

//ファイルの行数を取得
size_t Csv::GetHeight() const
{
	return dataList_.size();
}

//指定文字で文字列を分割
std::vector<std::string> Csv::Split(std::string const& inData, char sign) const
{
	std::vector<std::string> resultList;									//区切った文字列のリスト	
	int	first = 0;															//検索する文字列の先頭
	int	last  = (int)inData.find_first_of(sign);							//検索範囲の先頭にある区切り文字の位置

	//文字列を全て区切るまでループ
	while (first < inData.size()) 
	{
		std::string substr = inData.substr(first, (size_t)last - first);	//firstからlastまでの文字列を取得
		resultList.push_back(substr);										//リストに格納

		first = last + 1;													//文字列の先頭を検索済みの区切り文字の次の位置に
		last  = (int)inData.find_first_of(sign, (size_t)first);				//未確認の範囲にある先頭の区切り文字を探す

		if (last == std::string::npos) last = (int)inData.size();			//区切り文字が見つからなければ文字列の終端で区切る
	}

	return resultList;
}
