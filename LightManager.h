#pragma once
#include "Struct.h"
#include "Singleton.h"
#include "CircleShadow.h"

class LightManager : public Singleton<LightManager> {
public:
	/*-----光源-----*/
	static PointLightData pointlights[POINTLIGHT_NUM];			//点光源
	static DirLightData dirlights[DIRLIGHT_NUM];				//面光源
	static SpotLightData spotlights[SPOTLIGHT_NUM];				//スポットライト
	/*-----影-----*/
	static CircleShadowData circleShadows[CIRCLESHADOW_NUM];	//丸影光源

	friend Singleton<LightManager>;
	LightManager() {};

	/*-----光源-----*/
	static void GeneratePointlight(XMFLOAT3 lightpos, XMFLOAT3 lightcolor, XMFLOAT3 lightatten);
	static void GenerateDirlight(XMFLOAT3 lightvec, XMFLOAT3 lightcolor);
	static void GenerateSpotlight(XMFLOAT3 lightv,XMFLOAT3 lightpos, XMFLOAT3 lightcolor, XMFLOAT3 lightatten, XMFLOAT2 lightfactoranglecos);
	/*-----影-----*/
	static int GenerateCircleShadow(XMFLOAT3 dir, XMFLOAT3 centerPos,float distanceCasterLight,XMFLOAT3 atten,XMFLOAT2 facterAngleCos,unsigned int active);
};