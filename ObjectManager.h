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

////�Վ�
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
//	//�R���X�g���N�^
//	friend Singleton<ObjectManager>;
//	ObjectManager() {};
//
//public:
//
//	/*-----Line�֌W�̏���������-----*/
//	//Line GenerateLine(const XMFLOAT3& pos1 = XMFLOAT3(0, 0, 0), const XMFLOAT3& pos2 = XMFLOAT3(0, 0, 0), const XMFLOAT4& color = XMFLOAT4(1, 1, 1, 1), int projectionID = PROJECTIONID_OBJECT, int piplineID = PIPLINE_LINE_ALPHA);
//
//	/*-----Sprite�֌W�̏���������-----*/
//	//Sprite GenerateSprite(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, XMFLOAT4 color, LPCWSTR fileName = 0);
//	//Sprite GenerateSpriteTextureID(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, int textureID);
//	//SpriteMultiTexture GenerateSpriteMultiTexture(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, vector<LPCWSTR>* fileName = nullptr);;
//
//	/*-----�}���`�p�X�����_�����O�֌W�̏���������-----*/
//	//SpriteMultiPath GenerateSPriteMultiPath(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, int textureID);
//
//	/*-----object�֌W�̏���������-----*/
//	//Object3D GenerateModelObj(XMFLOAT3 centerPos, int projectionID, int piplineID, string directoryPath, string nodelFileName, LPCWSTR textureFileName, bool isSmoothing);
//	//Object3DShadow GenerateModelObjShadow(XMFLOAT3 centerPos, int projectionID, int piplineID, string directoryPath, string nodelFileName, LPCWSTR textureFileName, bool isSmoothing);
//	//Object3D GenerateModelObjInstance(XMFLOAT3 centerPos, int projectionID, int piplineID, string directoryPath, string nodelFileName, LPCWSTR textureFileName, bool isSmoothing);
//
//	/*-----GSParticle�֌W�̏���������-----*/
//	//GSParticleData GenerateGSParticle(XMFLOAT3 pos, int piplineID, XMFLOAT4 color, LPCWSTR fileName = 0);
//	//GSParticleData GenerateGSParticleInstance(XMFLOAT3 pos, int piplineID, XMFLOAT4 color, LPCWSTR fileName = 0);
//
//	/*-----�e��`�揈��-----*/
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
//	/*-----�e��s�񏈗�-----*/
//	/*void ChangeScale(XMMATRIX& scaleMat, XMFLOAT3 amount);
//	void ChangeRotation(XMMATRIX& rotationMat, XMFLOAT3 amount);
//	void ChangeRotationInit(XMMATRIX& rotationMat);
//	void ChangePosition(XMMATRIX& positionMat,XMFLOAT3& pos, XMFLOAT3 amount);
//	void ChangePositionAdd(XMMATRIX& positionMat, XMFLOAT3& pos, XMFLOAT3 amount);*/
//
//
//	//void ChangeLinePosition(const XMFLOAT3& pos1, const XMFLOAT3& pos2, Line& line);
//};

#pragma region ���R�[�h
//class SpriteManager : public Singleton<SpriteManager> {
//private:
//	//�R���X�g���N�^
//	friend Singleton<SpriteManager>;
//	SpriteManager();
//
//public:
//	//���̏���������
//	Sprite GenerateLine(XMFLOAT3 startPos, XMFLOAT3 endPos, int projectionID, string pipleName, XMFLOAT4 color);
//	//���Ԋu�ɐL�т���̏���������
//	Sprite GenerateLines(XMFLOAT3 startPos, XMFLOAT3 endPos, XMFLOAT3 Directions, float lineInterval, int lineCount, int projectionID, string pipleName, XMFLOAT4 color);
//	//�c������������������Ăł��锠�̏��������� ����STG�̕Ǘp
//	Sprite GenerateLineBox(XMFLOAT3 centerPos, XMFLOAT3 size, float lineInterval, int lineCount, int projectionID, string pipleName, XMFLOAT4 color);
//	//2D�X�v���C�g�ꖇ���̏���������
//	Sprite Generate2DTexture(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, string pipleName, LPCWSTR fileName, UVs* uv = nullptr);
//	//3D�X�v���C�g�ꖇ���̏���������
//	Sprite GenerateBox(XMFLOAT3 centerPos, XMFLOAT3 size, string spriteName, int projectionID, string pipleName, XMFLOAT4 color, LPCWSTR fileName = 0);
//	//�~���̏���������
//	Sprite GenerateCone(XMFLOAT3 centerPos, float radius, float height, int projectionID, string pipleName, XMFLOAT4 color, LPCWSTR fileName = 0);
//	//�~���̏���������
//	Sprite GenerateCylinder(XMFLOAT3 centerPos, float radius, float height, int projectionID, string pipleName, XMFLOAT4 color, LPCWSTR fileName = 0);
//	//���̏���������
//	Sprite GenerateSphere(XMFLOAT3 centerPos, float radius, int projectionID, string pipleName, XMFLOAT4 color, LPCWSTR fileName = 0);
//	//fbx�̏���������
//	Sprite GenerateModelFbx(XMFLOAT3 centerPos, string spriteName, int projectionID, string pipleName, const char* nodelFileName, LPCWSTR textureFileName);
//	//obj�̏���������
//	Sprite GenerateModelObj(XMFLOAT3 centerPos, string spriteName, int projectionID, string pipleName, const char* nodelFileName, LPCWSTR textureFileName, bool isSmoothing, bool isCircleShadow);
//	//�X�v���C�g�ꖇ���̕`�揈��
//	void Draw(Sprite& sprite);
//
//	//�X�P�[�����O
//	void ChangeScale(Sprite& sprite, XMFLOAT3 scale);
//	//��]
//	void ChangeRotation(Sprite& sprite, XMFLOAT3 rotation);
//	//���s�ړ�(���)
//	void ChangePosition(Sprite& sprite, XMFLOAT3 position);
//	//���s�ړ�(���Z)
//	void ChangePositionAdd(Sprite& sprite, XMFLOAT3 position);
//
//	//GS�p�[�e�B�N���p����������
//	GSParticleData GenerateGSParticle(XMFLOAT3 pos, string piplineName, LPCWSTR fileName);
//	//GS�p�[�e�B�N���p�`�揈��
//	void DrawGSParticle(GSParticleData data);
//};
#pragma endregion

