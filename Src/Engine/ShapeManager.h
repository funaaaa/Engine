#pragma once
#include "ModelDataManager.h"
#include "Singleton.h"

//class ShapeManager : public Singleton<ShapeManager> {
//private:
//	static vector<ShapeData> shapeData;
//	friend Singleton<ShapeManager>;
//	ShapeManager() {};
//
//public:
//
//	//図形をロードして値をコピーする。ロード済みの図形だったらロードせずにコピーだけ行う。
//	void LoadShape(int shapeID, Sprite& data);
//
//	void CreateCone(Sprite& data);
//	void CreateCylinder(Sprite& data);
//	void CreateSphere(Sprite& data);
//};