//#pragma once
//#include "Singleton.h"
//#include "Vec.h"
//#include <vector>
//#include <string>
//#include <array>
//#include <wtypes.h>
//#include <memory>
//
//class PolygonMeshInstance;
//class BLAS;
//
//// OBJ複数メッシュクラス 実質SPONZA読み込み用クラス 今は使用していません。
//class MultiMeshLoadOBJ : public Singleton<MultiMeshLoadOBJ> {
//
//private:
//
//	/*===== メンバ変数 =====*/
//
//	std::vector<std::wstring> texturePath_;
//
//	std::vector<std::pair<std::vector<int>, std::weak_ptr<BLAS>>> blasID_;	// テクスチャのハンドルと、それを使用したBLASのIDを保存するコンテナ
//
//	using string = std::string;
//
//	bool isOpaque_;		// SPONZA読み込み用のデバッグ機能 不透明フラグ
//	bool isFloor_;		// SPONZA読み込み用のデバッグ機能 GIを床のみに使用するため。
//
//
//public:
//
//	/*===== メンバ関数 =====*/
//
//	// レイトレ用OBJ複数メッシュ読み込み関数 BLASのアドレスを返す。
//	std::vector<std::weak_ptr<PolygonMeshInstance>> RayMultiMeshLoadOBJ(const string& DirectryPath, const string& FilePath, const LPCWSTR& HitGroupName);
//
//private:
//
//	// マテリアルファイルからテクスチャ情報を抜き出す。
//	void LoadMaterial(const string& DirectryPath, const string& MaterialFileName, const string& MaterialName, std::vector<int>& TextureHandle);
//
//	// stringをwstringに変換。
//	std::wstring StringToWString(std::string oString);
//
//};