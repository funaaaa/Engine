#pragma once
#include "Sprite.h"

#define MAX_CHAR 256				//1フレームに描画する最大文字数
/*以下はメンバ変数として持たせて、フォントテクスチャによって変えられるようにしてもいいかも*/
#define FONT_WIDTH 9				//フォントの幅
#define FONT_HEIGHT 18				//フォントの高さ
#define FONT_LINE_COUNT_X 14		//フォントテクスチャの一列の文字数
#define FONT_LINE_COUNT_Y 7			//フォントテクスチャの一列の文字数

class DebugText {
private:
	Sprite chars[MAX_CHAR];			//文字列
public:
	//コンストラクタ
	DebugText();
	//初期化
	void Init();
	//文字列生成
	void Print(LPCWSTR fontFile, string text, float x, float y, float scaleX, float scaleY);
	void PrintBack(LPCWSTR fontFile, string text, float x, float y, float scaleX, float scaleY);
	//描画
	void Draw();
};