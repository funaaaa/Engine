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

	std::vector<std::pair<int, int>> blasID;	// テクスチャのハンドルと、それを使用したBLASのIDを保存するコンテナ
	std::vector<Vec3> vertexData;			// BLASの先頭要素を保存。
	std::vector<float> vertexFirstLength;	// BLASの第一要素と第二要素の頂点距離

	using string = std::string;


public:

	/*===== メンバ関数 =====*/

	// レイトレ用OBJ複数メッシュ読み込み関数 BLASのアドレスを返す。
	std::vector<int> RayMultiLeshLoadOBJ(const string& DirectryPath, const string& FilePath, const LPCWSTR& HitGroupName);

private:

	// マテリアルファイルからテクスチャ情報を抜き出す。
	int LoadMaterial(const string& DirectryPath, const string& MaterialFileName, const string& MaterialName);

	// stringをwstringに変換。
	std::wstring StringToWString(std::string oString);

};