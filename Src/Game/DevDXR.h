#pragma once
#include "SoundManager.h"
#include "PiplineManager.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "Input.h"
#include "FbxLoader.h"

#include "RayDenoiser.h"
#include "BakeAO.h"

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

#include "KariConstBuffer.h"

#include "FHelper.h"

class DevDXR {

private:

	/*===== メンバ変数 =====*/

	// 仮の定数バッファを宣言
	KariConstBufferData constBufferData;
	DynamicConstBuffer constBuff;

	// デノイズAO用のパイプラインを設定。
	vector<RayPiplineShaderData> dAOuseShaders;
	RaytracingPipline deAOPipline;

	// SPONZAを読み込む。
	std::vector<int> sponzaInstance;

	// ライト用のスフィアを読み込む。
	int sphereBlas;
	int sphereIns;


	// TLASを生成。
	TLAS tlas;

	// レイトレ出力用クラスをセット。
	RaytracingOutput aoOutput;

	// レイトレ出力用クラスをセット。
	RaytracingOutput colorOutput;

	// レイトレ出力用クラスをセット。
	RaytracingOutput lightOutput;

	// GI出力用クラスをセット。
	RaytracingOutput giOutput;

	// デノイズの結果出力用クラスをセット。
	RaytracingOutput denoiseMixTextureOutput;


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


public:

	/*===== メンバ関数 =====*/

	void Init();
	void Update();
	void Draw();


private:



	// fps更新
	void FPS();

	// 入力操作
	void Input(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& degugPiplineID);
	void InputImGUI(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& degugPiplineID, bool& isMove);



};