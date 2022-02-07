//#pragma once
//#include <cmath>
//#include <d3dcompiler.h>
//#include <map>
//#include "ShapeManager.h"
//#include "Camera.h"
//#include "Struct.h"
//#include "TextureManager.h"
//#include "ModelDataManager.h"
//#include "PiplineManager.h"
//#include "LightManager.h"
////
//#pragma comment(lib, "d3d12.lib")
//#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3dcompiler.lib")

////臨時
//struct ParticleData {
//	XMMATRIX matTrans;
//	XMMATRIX matRot;
//	XMMATRIX matScale;
//
//	bool dirtyFlag = false;
//
//	tuple<XMMATRIX, XMMATRIX, XMMATRIX> GetTuple() {
//		tuple<XMMATRIX, XMMATRIX, XMMATRIX> returnBuff = { matTrans,matRot, matScale };
//		return returnBuff;
//	}
//
//};
//
//class ObjectManager : public Singleton<ObjectManager> {
//private:
//	//コンストラクタ
//	friend Singleton<ObjectManager>;
//	ObjectManager() {};
//
//public:
//
//	/*-----Line関係の初期化処理-----*/
//	//Line GenerateLine(const XMFLOAT3& pos1 = XMFLOAT3(0, 0, 0), const XMFLOAT3& pos2 = XMFLOAT3(0, 0, 0), const XMFLOAT4& color = XMFLOAT4(1, 1, 1, 1), int projectionID = PROJECTIONID_OBJECT, int piplineID = PIPLINE_LINE_ALPHA);
//
//	/*-----Sprite関係の初期化処理-----*/
//	//Sprite GenerateSprite(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, XMFLOAT4 color, LPCWSTR fileName = 0);
//	//Sprite GenerateSpriteTextureID(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, int textureID);
//	//SpriteMultiTexture GenerateSpriteMultiTexture(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, vector<LPCWSTR>* fileName = nullptr);;
//
//	/*-----マルチパスレンダリング関係の初期化処理-----*/
//	//SpriteMultiPath GenerateSPriteMultiPath(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, int textureID);
//
//	/*-----object関係の初期化処理-----*/
//	//Object3D GenerateModelObj(XMFLOAT3 centerPos, int projectionID, int piplineID, string directoryPath, string nodelFileName, LPCWSTR textureFileName, bool isSmoothing);
//	//Object3DShadow GenerateModelObjShadow(XMFLOAT3 centerPos, int projectionID, int piplineID, string directoryPath, string nodelFileName, LPCWSTR textureFileName, bool isSmoothing);
//	//Object3D GenerateModelObjInstance(XMFLOAT3 centerPos, int projectionID, int piplineID, string directoryPath, string nodelFileName, LPCWSTR textureFileName, bool isSmoothing);
//
//	/*-----GSParticle関係の初期化処理-----*/
//	//GSParticleData GenerateGSParticle(XMFLOAT3 pos, int piplineID, XMFLOAT4 color, LPCWSTR fileName = 0);
//	//GSParticleData GenerateGSParticleInstance(XMFLOAT3 pos, int piplineID, XMFLOAT4 color, LPCWSTR fileName = 0);
//
//	/*-----各種描画処理-----*/
//	//void DrawLine(Line& data);
//	//void DrawSprite(Sprite& data);
//	//void DrawSpriteMultiPath(SpriteMultiPath& data);
//	//void DrawSpriteMultiTexture(SpriteMultiTexture& data);
//	//void DrawObject(Object3D& data);
//	//void DrawObject(Object3DShadow& data);
//	//void DrawObjectInstance(const Object3D& instanceData, const vector<Object3D>& data, const int& piplineID);
//	//void DrawGSParticle(GSParticleData& data);
//	//void DrawGSParticleInstance(const GSParticleData& instanceData, vector<ParticleData*>& data, const int& piplinID);
//
//	/*-----各種行列処理-----*/
//	/*void ChangeScale(XMMATRIX& scaleMat, XMFLOAT3 amount);
//	void ChangeRotation(XMMATRIX& rotationMat, XMFLOAT3 amount);
//	void ChangeRotationInit(XMMATRIX& rotationMat);
//	void ChangePosition(XMMATRIX& positionMat,XMFLOAT3& pos, XMFLOAT3 amount);
//	void ChangePositionAdd(XMMATRIX& positionMat, XMFLOAT3& pos, XMFLOAT3 amount);*/
//
//
//	//void ChangeLinePosition(const XMFLOAT3& pos1, const XMFLOAT3& pos2, Line& line);
//};

#pragma region 元コード
//class SpriteManager : public Singleton<SpriteManager> {
//private:
//	//コンストラクタ
//	friend Singleton<SpriteManager>;
//	SpriteManager();
//
//public:
//	//線の初期化処理
//	Sprite GenerateLine(XMFLOAT3 startPos, XMFLOAT3 endPos, int projectionID, string pipleName, XMFLOAT4 color);
//	//一定間隔に伸びる線の初期化処理
//	Sprite GenerateLines(XMFLOAT3 startPos, XMFLOAT3 endPos, XMFLOAT3 Directions, float lineInterval, int lineCount, int projectionID, string pipleName, XMFLOAT4 color);
//	//縦横たくさん線を引いてできる箱の初期化処理 第一回STGの壁用
//	Sprite GenerateLineBox(XMFLOAT3 centerPos, XMFLOAT3 size, float lineInterval, int lineCount, int projectionID, string pipleName, XMFLOAT4 color);
//	//2Dスプライト一枚分の初期化処理
//	Sprite Generate2DTexture(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, string pipleName, LPCWSTR fileName, UVs* uv = nullptr);
//	//3Dスプライト一枚分の初期化処理
//	Sprite GenerateBox(XMFLOAT3 centerPos, XMFLOAT3 size, string spriteName, int projectionID, string pipleName, XMFLOAT4 color, LPCWSTR fileName = 0);
//	//円錐の初期化処理
//	Sprite GenerateCone(XMFLOAT3 centerPos, float radius, float height, int projectionID, string pipleName, XMFLOAT4 color, LPCWSTR fileName = 0);
//	//円柱の初期化処理
//	Sprite GenerateCylinder(XMFLOAT3 centerPos, float radius, float height, int projectionID, string pipleName, XMFLOAT4 color, LPCWSTR fileName = 0);
//	//球の初期化処理
//	Sprite GenerateSphere(XMFLOAT3 centerPos, float radius, int projectionID, string pipleName, XMFLOAT4 color, LPCWSTR fileName = 0);
//	//fbxの初期化処理
//	Sprite GenerateModelFbx(XMFLOAT3 centerPos, string spriteName, int projectionID, string pipleName, const char* nodelFileName, LPCWSTR textureFileName);
//	//objの初期化処理
//	Sprite GenerateModelObj(XMFLOAT3 centerPos, string spriteName, int projectionID, string pipleName, const char* nodelFileName, LPCWSTR textureFileName, bool isSmoothing, bool isCircleShadow);
//	//スプライト一枚分の描画処理
//	void Draw(Sprite& sprite);
//
//	//スケーリング
//	void ChangeScale(Sprite& sprite, XMFLOAT3 scale);
//	//回転
//	void ChangeRotation(Sprite& sprite, XMFLOAT3 rotation);
//	//平行移動(代入)
//	void ChangePosition(Sprite& sprite, XMFLOAT3 position);
//	//平行移動(加算)
//	void ChangePositionAdd(Sprite& sprite, XMFLOAT3 position);
//
//	//GSパーティクル用初期化処理
//	GSParticleData GenerateGSParticle(XMFLOAT3 pos, string piplineName, LPCWSTR fileName);
//	//GSパーティクル用描画処理
//	void DrawGSParticle(GSParticleData data);
//};
#pragma endregion

