#pragma once
#include "SoundManager.h"
#include "Camera.h"
#include "Input.h"
#include "FbxLoader.h"

#include "RayDenoiser.h"

#include "BLASRegister.h"
#include "PorygonInstance.h"
#include "TLAS.h"
#include "RayRootsignature.h"
#include "DynamicConstBuffer.h"
#include "DescriptorHeapMgr.h"
#include "Vec.h"
#include "PorygonInstanceRegister.h"
#include "HitGroupMgr.h"
#include "RaytracingPipline.h"
#include "RaytracingOutput.h"
#include "MultiMeshLoadOBJ.h"
#include "ComputeShader.h"
#include "BLAS.h"

#include "HitGroup.h"

#include <utilapiset.h>

#include "ConstBuffers.h"
#include "FHelper.h"

#include "Player.h"
#include <array>

class DevDXR {

private:

	/*===== メンバ変数 =====*/


	// 定数バッファ
	RayConstBufferData constBufferData;
	DynamicConstBuffer constBuffer;

	// デノイズAO用のパイプラインを設定。
	std::vector<RayPiplineShaderData> dAOuseShaders;
	RaytracingPipline deAOPipline;

	// SPONZAを読み込む。
	std::vector<int> sponzaInstance;

	// ライト用のスフィアを読み込む。
	int sphereBlas;
	std::array<int, RayLightConstBufferData::POINT_LIGHT_COUNT> sphereIns;

	// 天球用のスフィア
	int skyDomeBlas;
	int skyDomeIns;

	// ステージ
	int stageBlas;
	int stageIns;


	// TLASを生成。
	TLAS tlas;

	// アンビエントオクルージョン出力用クラスをセット。
	RaytracingOutput aoOutput;

	// 色出力用クラスをセット。
	RaytracingOutput colorOutput;

	// デノイズするライト出力用クラスをセット。
	RaytracingOutput lightOutput;

	// GI出力用クラスをセット。
	RaytracingOutput giOutput;

	// デノイズの結果出力用クラスをセット。
	RaytracingOutput denoiseMixTextureOutput;

	// FPS表示をするか否か
	bool isDisplayFPS;


	// デバッグ用のパイプラインを切り替えるやつ。
	enum DEGU_PIPLINE_ID {
		DEF_PIPLINE,
		AO_PIPLINE,
		DENOISE_AO_PIPLINE,
	};

	// デバッグ用でノイズ画面を出すフラグ。
	DEGU_PIPLINE_ID debugPiplineID;

	// ライトが動いたか
	bool isMoveLight;

	// プレイヤー
	Player player;


public:

	/*===== メンバ関数 =====*/

	void Init();
	void Update();
	void Draw();


private:



	// fps更新
	void FPS();

	// 入力操作
	void Input();
	void InputImGUI(bool& isMove);



};