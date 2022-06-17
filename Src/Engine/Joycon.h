//#pragma once
//#include <hidsdi.h>
//#include <SetupAPI.h>
//
////このクラスではまだスティックを実装していない
//
///*-----構造体宣言-----*/
////joycon(R)のボタン情報
//struct JoyconRightButton {
//	bool a;
//	bool b;
//	bool x;
//	bool y;
//	bool sr;
//	bool sl;
//	bool r;
//	bool zr;
//};
////joycon(L)のボタン情報
//struct JoyconLightButton {
//	bool down;
//	bool up;
//	bool left;
//	bool right;
//	bool sr;
//	bool sl;
//	bool l;
//	bool zl;
//};
////joyconのジャイロ情報
//struct JoyconGyro {
//	float x;
//	float y;
//	float z;
//};
////joyconの1フレームごとの入力情報構造体
//struct JoyconFrameData{
//	JoyconRightButton rButton;			//joycon(R)のボタン情報
//	JoyconLightButton lButton;			//joycon(L)のボタン情報
//	JoyconGyro gyro;					//joyconのジャイロ情報
//	float horizontal;					//スティックの水平入力情報
//	float vertical;						//スティックの垂直入力情報
//};
////joyconのデータの構造体
//struct JoyconData {
//	JoyconFrameData now;				//今のフレームの入力情報
//	JoyconFrameData old;				//前のフレームの入力情報
//};
//
//class Joycon {
//public:
//	/*メンバ変数*/
//	JoyconData joyconData;				//joyconの入力情報
//	HANDLE joyconHandle{};				//joyconのハンドルを保存する変数
//	HIDP_CAPS joyconCaps{};				//joyconから送られてくるデータのサイズなどが入った構造体
//	BYTE* joyconInputBuffer{};			//joyconから送られてくるデータ
//	BYTE* joyconOutputBuffer{};			//joyconに送るデータ
//	OVERLAPPED joyconOverlapped{};		//joyconの非同期処理用
//
//	//コンストラクタ
//	Joycon();
//	//joyconをロード		現状WirelessGamepadでしか読み取れないのでjoycon(R)や(L)として読み取れるようにする。
//	void LoadJoycon();
//	//joyconのデータをアップデート
//	void UpdateJoyconData();
//
//	//hidのリストから取得可能なデバイスを探し、パス名を取得して配列に保存する
//	void GetPathName(vector<wstring>& devicePath, GUID& guid, HDEVINFO& deviceInfo, SP_DEVICE_INTERFACE_DATA& deviceInfoData);
//	//joyconのhandleを取得する
//	void GetJoyconHandle(vector<wstring> devicePath);
//	//joyconの入力レポートを標準入力レポート形式にする出力レポートを送信
//	void ChangeInputReport();
//	//joyconの6軸センサーを有効化させる
//	void Enable6axisSensor();
//};
//
////unsigned intからint型への変換
//int16_t SelfUintToInt(uint16_t target);