#pragma once
#include "BaseOperationObject.h"
#include <memory>
#include <string>

class Character;

class PlayerOperationObject : public BaseOperationObject {

private:

	/*===== メンバ変数 =====*/

	int controllerIndex_;	// 操作するコントローラーのインデックス
	bool isWriteGhostInfo_;	// ゴースト情報を保存するかどうか。
	Character* character_;
	std::string filePath_;
	int frameTimer_;		// 始まってからのフレームを計測する用タイマー 

public:

	/*===== メンバ関数 =====*/

	PlayerOperationObject(const int& ControllerIndex, const bool& IsWriteGhostInfo, Character* CharacterPointer);

	BaseOperationObject::Operation Input(const BaseOperationObject::OperationInputData& InputData)override;

	void OnGoal()override;

private:

	void DecideFileName();
	void WriteGhostInfo(BaseOperationObject::Operation Operation);

};