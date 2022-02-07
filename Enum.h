#pragma once

//3D図形データID
enum ShapeDataID {
	SHAPEDATA_CONE,
	SHAPEDETA_CLYINDER,
	SHAPEDATA_SPHERE
};

//どんな投影をするかのID
enum projectionID {
	PROJECTIONID_UI,						//平行投影
	PROJECTIONID_OBJECT,					//透視投影
	PROJECTIONID_BILLBOARD					//ビルボード
};

//どんなブレンドをするかのID
enum blendID {
	BLENDMODE_ADD,							//加算合成
	BLENDMODE_ALPHA							//アルファブレンド
};

//パイプラインID	PIPLINE _ (型名) _ (処理) _ (合成)
enum PiplineID{
	/*ライン用*/
	PIPLINE_LINE_ALPHA,								//ライン用												アルファブレンド		定数1つ、テクスチャ1つ
	/*スプライト用*/
	PIPLINE_SPRITE_ADD,								//スプライト用												加算合成				定数1つ、テクスチャ1つ
	PIPLINE_SPRITE_ALPHA,							//スプライト用												アルファブレンド		定数1つ、テクスチャ1つ
	/*オブジェクト用*/
	PIPLINE_OBJECT_LIGHT_ALPHA,						//3Dオブジェクト用			ライティング						アルファブレンド		定数3つ、テクスチャ1つ
	PIPLINE_OBJECT_LIGHT_FOG_ALPHA,					//3Dオブジェクト用			ライティング フォグ					アルファブレンド		定数3つ、テクスチャ1つ
	PIPLINE_OBJECT_LIGHT_FOG_BUMP_ALPHA,			//3Dオブジェクト用			ライティング フォグ	 バンプマッピング	アルファブレンド		定数3つ、テクスチャ1つ
	PIPLINE_OBJECT_NOLIGHT_ALPHA,					//3Dオブジェクト用			ライティング無し					アルファブレンド		定数3つ、テクスチャ1つ
	PIPLINE_OBJECT_NOLIGHT_ALPHA_F,					//3Dオブジェクト用			ライティング無し 前面カリング			アルファブレンド		定数3つ、テクスチャ1つ
	PIPLINE_OBJECT_OUTLINE_ALPHA,					//3Dオブジェクト用			アウトライン用						アルファブレンド		定数3つ、テクスチャ1つ
	PIPLINE_OBJECT_TOONSHADER_ALPHA,				//3Dオブジェクト用			トゥーンシェーダー					アルファブレンド		定数3つ、テクスチャ1つ
	PIPLINE_OBJECT_TOONSHADER_SHADOW_ALPHA,			//3Dオブジェクト用			トゥーンシェーダー 影あり			アルファブレンド		定数3つ、テクスチャ1つ
	PIPLINE_OBJECT_TOONSHADER_SHADOW_FOG_ALPHA,		//3Dオブジェクト用			トゥーンシェーダー 影あり フォグ		アルファブレンド		定数3つ、テクスチャ1つ
	PIPLINE_OBJECT_TOONSHADER_SHADOW_FOG_ALPHA_F,	//3Dオブジェクト用			トゥーンシェーダー 影あり フォグ 前面	アルファブレンド		定数3つ、テクスチャ1つ
	PIPLINE_OBJECT_TOONSHADER_SHADOW_MRT_D_ALPHA,	//3Dオブジェクト用			トゥーンシェーダー 影あり MRT(D)		アルファブレンド		定数3つ、テクスチャ1つ
	PIPLINE_OBJECT_OUTLINE_MRT_D_ALPHA,				//3Dオブジェクト用			アウトライン用 MRT(D)				アルファブレンド		定数3つ、テクスチャ1つ
	PIPLINE_OBJECT_BUMP_ALPHA,						//3Dオブジェクト用			バンプマッピング					アルファブレンド		定数3つ、テクスチャ2つ
	PIPLINE_OBJECT_GET_SHADOWMAP,					//3Dオブジェクト用			シャドウマップを取得用									定数1つ、テクスチャ1つ
	PIPLINE_OBJECT_LIGHT_SHADOWMAP_ALPHA,			//3Dオブジェクト用			ライティング シャドウマップを仕様		アルファブレンド		定数3つ、テクスチャ2つ
	/*オブジェクトインスタンシング用*/
	PIPLINE_OBJECT_INSTANCE_LIGHT_ALPHA,			//インスタンシング描画用		ライティング						アルファブレンド		定数3つ、テクスチャ1つ
	PIPLINE_OBJECT_INSTANCE_NOLIGHT_ALPHA,			//インスタンシング描画用		ライティング無し					アルファブレンド		定数3つ、テクスチャ1つ
	PIPLINE_OBJECT_INSTANCE_OUTLINE_ALPHA,			//インスタンシング描画用		アウトライン用						アルファブレンド		定数3つ、テクスチャ1つ
	PIPLINE_OBJECT_INSTANCE_TOONSHADER_ALPHA,		//インスタンシング描画用		トゥーンシェーダー					アルファブレンド		定数3つ、テクスチャ1つ
	/*GSパーティクル用*/
	PIPLINE_GSPARTICLE_NOLIGHT_ALPHA,				//GSパーティクル用			点から立体を生成					アルファブレンド		定数1つ、テクスチャ1つ
	PIPLINE_GSPARTICLE_INSTANCE_NOLIGHT_ALPHA,		//GSパーティクル用			点から立方体を生成 インスタンシング描画	アルファブレンド		定数1つ、テクスチャ1つ
	/*マルチパスレンダリング用*/
	PIPLINE_MULTIPATH_NORMAL_ADD,					//マルチパスレンダリング用									加算合成				定数2つ、テクスチャ1つ
	PIPLINE_MULTIPATH_NORMAL_ALPHA,					//マルチパスレンダリング用									アルファブレンド		定数2つ、テクスチャ1つ
	PIPLINE_MULTIPATH_MONOCHROME_ADD,				//マルチパスレンダリング用	モノクロ化						加算合成				定数2つ、テクスチャ1つ
	PIPLINE_MULTIPATH_MONOCHROME_ALPHA,				//マルチパスレンダリング用	モノクロ化						アルファブレンド		定数2つ、テクスチャ1つ
	PIPLINE_MULTIPATH_BLUR_ADD,						//マルチパスレンダリング用	ブラーをかける						加算合成				定数2つ、テクスチャ1つ
	PIPLINE_MULTIPATH_BLUR_ALPHA,					//マルチパスレンダリング用	ブラーをかける						アルファブレンド		定数2つ、テクスチャ1つ
	PIPLINE_MULTIPATH_GAUSSIAN_X_ALPHA,				//マルチパスレンダリング用	ガウスぼかしをかける				アルファブレンド		定数2つ、テクスチャ1つ
	PIPLINE_MULTIPATH_GAUSSIAN_Y_ALPHA,				//マルチパスレンダリング用	ガウスぼかしをかける				アルファブレンド		定数2つ、テクスチャ1つ
	PIPLINE_MULTIPATH_FOG_ALPHA,					//マルチパスレンダリング用	フォグ							アルファブレンド		定数2つ、テクスチャ2つ
	/*便利*/
	PIPLINE_SPRITE_SAMPLING_LUMINANCE_ADD,			//スプライト用				輝度を抽出する						加算合成				定数2つ、テクスチャ1つ
	PIPLINE_SPRITE_MIXE_8TEXTURE_ADD_TITLE,			//スプライト用				輝度を抽出する						加算合成				定数2つ、テクスチャ1つ
	PIPLINE_SPRITE_MIXE_8TEXTURE_ADD,				//スプライト用				8つのテクスチャを合成				加算合成				定数2つ、テクスチャ8つ
	PIPLINE_SPRITE_MIXE_2TEXTURE_ADD,				//スプライト用				2つのテクスチャを合成				加算合成				定数2つ、テクスチャ2つ
};