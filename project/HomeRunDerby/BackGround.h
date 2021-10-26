#pragma once

//”wŒi•`‰æ
namespace BackGround
{
	enum class BACK_GROUND_TYPE
	{
		SKY = 0,
		MAX,
	};

	//‰Šú‰»
	void Initialize();

	//•`‰æ
	void Draw();

	//g—p‚·‚é”wŒi‚ğ•ÏX‚·‚é
	void ChangeBackGround(BACK_GROUND_TYPE type);

	//‰ğ•ú
	void Release();
}