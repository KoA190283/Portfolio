#pragma once
#include<string>
#include<vector>

//CSVファイルのデータを扱うクラス
class Csv
{
private:

	std::vector<std::vector<std::string>> dataList_;		//読み込んだCSVファイルのデータを分割したリスト

public:

	Csv();
	~Csv();

	//CSVファイルの読み込み
	//引数：fileName	読み込むファイルの名前
	//戻値：読み込みの成否
	bool Load(const std::string& fileName);
	
	//指定位置のstring型データを取得
	//引数１：row		取得するデータのある行
	//引数２：column	取得するデータのある列
	//戻値　：指定位置のデータ
	std::string GetStringDeta(int row, int column) const;

	//指定位置のint型データを取得
	//引数１：row		取得するデータのある行
	//引数２：column	取得するデータのある列
	//戻値　：指定位置のデータ
	int GetIntegerDeta(int row, int column) const;

	//指定位置のfloat型データを取得
	//引数１：row		取得するデータのある行
	//引数２：column	取得するデータのある列
	//戻値　：指定位置のデータ
	float GetFloatDeta(int row, int column) const;

	//ファイルの列数を取得
	//引数：row	列数を取得する行
	//戻値：列数
	size_t GetWidth(int row) const;

	//ファイルの行数を取得
	//戻値：行数
	size_t GetHeight() const;

private:

	//指定文字で文字列を分割
	//引数１：inData	分割する文字列
	//引数２：sign		区切り文字
	//戻値　：分割した文字列リスト
	std::vector<std::string> Split(std::string const& inData, char sign) const;
};

