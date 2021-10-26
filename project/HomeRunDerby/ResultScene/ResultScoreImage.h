#pragma once
#include "../Engine/IUserInterface.h"

//Œ‹‰Ê‰æ‘œ
class ResultScoreImage : public IUserInterface
{
public:

	ResultScoreImage();

	//‰Šú‰»
	void Initialize() override;

	//XV
	void Update() override;

	//•`‰æ
	void Draw() override;

	//‰ğ•ú
	void Release() override;
};
