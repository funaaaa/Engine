#include "DebugText.h"
#include "Enum.h"

DebugText::DebugText()
{
	for (int i = 0; i < MAX_CHAR; ++i) {
		chars[i].DoNotDisplay();
	}
}

void DebugText::Init()
{
	for (int i = 0; i < MAX_CHAR; ++i) {
		chars[i].DisplayOnScreen();
	}
}

void DebugText::Print(LPCWSTR fontFile, string text, float x, float y, float scaleX, float scaleY)
{
	//入力された文字数分for文を回す
	for (int i = 0; i < text.size(); ++i) {

		//i番目の文字列を取り出す
		const unsigned char& charactor = text[i];

		//ASCIIコードの上2段を飛ばした番号を計算
		UINT fontIndex = charactor - 32;
		if (charactor >= 0x7F) {
			fontIndex = 0;
		}

		//フォントテクスチャの指定の文字のがあるインデックスを求める
		UINT fontIndexY = fontIndex / FONT_LINE_COUNT_X;
		UINT fontIndexX = fontIndex % FONT_LINE_COUNT_X;

		//各情報を計算
		XMFLOAT3 centerPos = { x + (FONT_WIDTH + FONT_WIDTH / 3.0f * 2.0f) * scaleX * i, y, 0 };
		XMFLOAT2 size = { FONT_WIDTH * scaleX, FONT_HEIGHT * scaleY };

		//生成
		Sprite buff;
		buff.GenerateForTexture(centerPos, size, PROJECTIONID_UI, PIPLINE_SPRITE_ALPHA, fontFile);

		//空いている要素を探して代入
		for (int j = 0; j < MAX_CHAR; ++j) {

			if (chars[j].GetIsDisplay() == true) continue;

			chars[j] = buff;
			break;
		}
	}
}

void DebugText::PrintBack(LPCWSTR fontFile, string text, float x, float y, float scaleX, float scaleY)
{
	//入力された文字数分for文を回す
	for (int i = 0; i < text.size(); ++i) {

		//i番目の文字列を取り出す
		const unsigned char& charactor = text[i];

		//ASCIIコードの上2段を飛ばした番号を計算
		UINT fontIndex = charactor - 32;
		if (charactor >= 0x7F) {
			fontIndex = 0;
		}

		//フォントテクスチャの指定の文字のがあるインデックスを求める
		UINT fontIndexY = fontIndex / FONT_LINE_COUNT_X;
		UINT fontIndexX = fontIndex % FONT_LINE_COUNT_X;

		//各情報を計算
		XMFLOAT3 centerPos = { x + (FONT_WIDTH + FONT_WIDTH / 3.0f * 2.0f) * scaleX * i - (text.size() * ((FONT_WIDTH + FONT_WIDTH / 3.0f * 2.0f) * scaleX)), y, 0 };
		XMFLOAT2 size = { FONT_WIDTH * scaleX, FONT_HEIGHT * scaleY };

		//生成
		Sprite buff;
		buff.GenerateForTexture(centerPos, size, PROJECTIONID_UI, PIPLINE_SPRITE_ALPHA, fontFile);

		//空いている要素を探して代入
		for (int j = 0; j < MAX_CHAR; ++j) {

			if (chars[j].GetIsDisplay() == true) continue;

			chars[j] = buff;
			break;
		}
	}
}

void DebugText::Draw()
{
	//生成されている文字列分for文を回す
	for (int i = 0; i < MAX_CHAR; ++i) {

		if (chars[i].GetIsDisplay() == false) continue;

		//描画
		chars[i].Draw();

	}
}
