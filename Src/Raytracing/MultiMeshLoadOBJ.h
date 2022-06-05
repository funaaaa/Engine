#pragma once
#include "Singleton.h"
#include "Vec.h"
#include <vector>
#include <string>
#include <array>
#include <wtypes.h>

// OBJ複数メッシュクラス
class MultiMeshLoadOBJ : public Singleton<MultiMeshLoadOBJ> {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::wstring> texturePath;

	std::vector<std::pair<std::vector<int>, int>> blasID;	// テクスチャのハンドルと、それを使用したBLASのIDを保存するコンテナ

	using string = std::string;

	bool isOpaque;		// SPONZA読み込み用のデバッグ機能


public:

	/*===== メンバ関数 =====*/

	// レイトレ用OBJ複数メッシュ読み込み関数 BLASのアドレスを返す。
	std::vector<int> RayMultiMeshLoadOBJ(const string& DirectryPath, const string& FilePath, const LPCWSTR& HitGroupName);

private:

	// マテリアルファイルからテクスチャ情報を抜き出す。
	void LoadMaterial(const string& DirectryPath, const string& MaterialFileName, const string& MaterialName, std::vector<int>& TextureHandle);

	// stringをwstringに変換。
	std::wstring StringToWString(std::string oString);

};