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

	// AO用のパイプラインを設定。
	vector<RayPiplineShaderData> useShaders;
	RaytracingPipline aoPipline;

	// デノイズAO用のパイプラインを設定。
	vector<RayPiplineShaderData> dAOuseShaders;
	RaytracingPipline deAOPipline;

	// デフォルトのシェーダーを設定。
	vector<RayPiplineShaderData> defShaders;
	RaytracingPipline defPipline;

	// 影情報取得用パイプラインを設定。
	vector<RayPiplineShaderData> shadowShaders;
	RaytracingPipline shadowPipline;

	// SPONZAを読み込む。
	std::vector<int> sponzaInstance;

	// ライト用のスフィアを読み込む。
	int sphereBlas;
	int sphereIns;


	// TLASを生成。
	TLAS tlas;

	// レイトレ出力用クラスをセット。
	RaytracingOutput raytracingOutput;

	// レイトレ出力用クラスをセット。
	RaytracingOutput colorOutput;

	// レイトレ出力用クラスをセット。
	RaytracingOutput lightOutput;

	// デノイズの結果出力用クラスをセット。
	RaytracingOutput denoiseResultOutput;

	// 累積デノイズ用での保存用クラス。
	RaytracingOutput raytracingOutputData;

	// AOをベイク。
	RaytracingOutput bakeTex;

	// デノイザー受け取り用
	RaytracingOutput aoDenoiseOutput;
	RaytracingOutput aoDenoiseOutput1;
	RaytracingOutput aoDenoiseOutput2;

	// ライティングデノイズ受け取り用
	RaytracingOutput lightDenoiseOutput;
	RaytracingOutput lightDenoiseOutput1;
	RaytracingOutput lightDenoiseOutput2;


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