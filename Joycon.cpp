#include "Joycon.h"

#define MAX_PRODUCTNAME 126
#define CHECK_ERROR HRESULT_FROM_WIN32(GetLastError())

Joycon::Joycon()
{
	joyconData = {};
	joyconHandle = {};
	joyconCaps = {};
	joyconInputBuffer = {};
	joyconOutputBuffer = {};
	joyconOverlapped = {};
}

void Joycon::LoadJoycon()
{
	/*-----変数宣言-----*/
	vector<wstring> devicePath;						//デバイスのパス名を保存しておくためのコンテナ
	GUID guid{};									//hidドライバの識別を保存しておくための変数
	HDEVINFO deviceInfo{};							//hidデバイスのリスト
	SP_DEVICE_INTERFACE_DATA deviceInfoData{};		//hidデバイスのインターフェースデータ

	//HIDドライバの識別子を返す
	HidD_GetHidGuid(&guid);

	//HIDデバイスのリストを取得するために、HIDドライバに問い合わせる
	deviceInfo = SetupDiGetClassDevs(&guid, NULL, DirectXBase::Instance()->windowsAPI.hwnd, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	//インターフェースを示す構造体を構築する
	deviceInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	//デバイスのパス名を取得
	GetPathName(devicePath, guid, deviceInfo, deviceInfoData);

	//HIDデバイスリストを解放
	SetupDiDestroyDeviceInfoList(deviceInfo);

	//joyconのhandleを取得
	GetJoyconHandle(devicePath);

	//入力レポートを標準入力レポート形式に変更
	ChangeInputReport();

	//レポートとレポートの間は多少時間を開けたほうがいいらしいので、一定時間スリープする
	//Sleep(1 * 1000);

	//6軸センサーを有効化
	//Enable6axisSensor();
}

void Joycon::UpdateJoyconData()
{
	HRESULT error{};

	if (joyconInputBuffer) {
		//前フレームの入力情報を保存
		joyconData.old = joyconData.now;

		//joyconの入力レポートを取得
		auto a = ReadFile(joyconHandle, joyconInputBuffer, joyconCaps.InputReportByteLength, NULL, &joyconOverlapped);
		//エラーチェック
		error = CHECK_ERROR;

		//デバッグ用 入力レポートの形式を取得
		auto report = joyconInputBuffer[0];

		/*-----ボタンステータスの割当-----*/ //ここではjoycon(R)のステータスを割当しているが、現在のjoyconがRLどちらなのかを取得して条件式で分けたい
		joyconData.now.rButton.a = (joyconInputBuffer[1] << 0) & 1;
		joyconData.now.rButton.x = (joyconInputBuffer[1] >> 1) & 1;
		joyconData.now.rButton.b = (joyconInputBuffer[1] >> 2) & 1;
		joyconData.now.rButton.y = (joyconInputBuffer[1] >> 3) & 1;
		joyconData.now.rButton.sl = (joyconInputBuffer[1] >> 4) & 1;
		joyconData.now.rButton.sr = (joyconInputBuffer[1] >> 5) & 1;
		joyconData.now.rButton.r = (joyconInputBuffer[1] >> 6) & 1;
		joyconData.now.rButton.zr = (joyconInputBuffer[1] >> 7) & 1;

		/*-----ジャイロの割当-----*/
		//ジャイロの生データを取得
		//int16_t rawGyroDataX = SelfUintToInt((joyconInputBuffer[20] << 8) | (joyconInputBuffer[19]));
		//int16_t rawGyroDataY = SelfUintToInt((joyconInputBuffer[22] << 8) | (joyconInputBuffer[21]));
		//int16_t rawGyroDataZ = SelfUintToInt((joyconInputBuffer[24] << 8) | (joyconInputBuffer[23]));
		////ジャイロの生データを校正済みジャイロデータに変換させるためのオフセットと係数
		//auto calGyroOffset = SelfUintToInt(0x000E);
		//auto gyroCalcoeff = (float)(816.0f / (float)(0x343B - calGyroOffset));
		////オフセットと係数を用いて生データを変換
		//joyconData.now.gyro.x = (float)(rawGyroDataX - calGyroOffset) * gyroCalcoeff;
		//fabs(joyconData.now.gyro.x) < 10 ? joyconData.now.gyro.x = 0 : joyconData.now.gyro.x = joyconData.now.gyro.x / 1000.0f;
		//joyconData.now.gyro.y = (float)(rawGyroDataY - calGyroOffset) * gyroCalcoeff;
		//fabs(joyconData.now.gyro.y) < 10 ? joyconData.now.gyro.y = 0 : joyconData.now.gyro.y = joyconData.now.gyro.y / 1000.0f;
		//joyconData.now.gyro.z = (float)(rawGyroDataZ - calGyroOffset) * gyroCalcoeff;
		//fabs(joyconData.now.gyro.z) < 10 ? joyconData.now.gyro.z = 0 : joyconData.now.gyro.z = joyconData.now.gyro.z / 1000.0f;
		////joycon上の座標軸とDirectXの座標軸が違うため、合わせるためにy軸とz軸を入れ替える
		//float buff = joyconData.now.gyro.y;
		//joyconData.now.gyro.y = joyconData.now.gyro.z;
		//joyconData.now.gyro.z = buff;
		////ジャイロの動きが体感の動きとは反対になってしまうため、マイナスをかける
		//joyconData.now.gyro.x *= -1;
		//joyconData.now.gyro.y *= -1;
		//joyconData.now.gyro.z *= -1;

		/*-----標準入力レポート用-----*/
		////スティック情報の割り当て
		//joyconData.now.horizontal = SelfUintToInt((joyconInputBuffer[10] >> 4) | (joyconInputBuffer[11] << 4));
		//joyconData.now.horizontal -= 2000;
		////デッドラインを設ける
		//if (abs(joyconData.now.horizontal) < 200) {
		//	joyconData.now.horizontal = 0;
		//}
		////0じゃなかったらオフセット値で割って-1~1の範囲にする
		//if (joyconData.now.horizontal != 0) {
		//	joyconData.now.horizontal = joyconData.now.horizontal / 1200.0f;
		//}
		//joyconData.now.vertical = SelfUintToInt(joyconInputBuffer[9] | ((joyconInputBuffer[10] & 0xF) << 8));
		//joyconData.now.vertical -= 2000;
		////デッドラインを設ける
		//if (abs(joyconData.now.vertical) < 200) {
		//	joyconData.now.vertical = 0;
		//}
		////0じゃなかったらオフセット値で割って-1~1の範囲にする
		//if (joyconData.now.vertical != 0) {
		//	joyconData.now.vertical = joyconData.now.vertical / 1200.0f;
		//}
		////垂直が逆のほうがわかりやすいので反転させる
		//joyconData.now.vertical *= -1;
		//int b = 0;

		int stickInputData = joyconInputBuffer[3];
		switch (stickInputData) {
		case 0:
			joyconData.now.horizontal = 0;
			joyconData.now.vertical = 1;
			break;
		case 1:
			joyconData.now.horizontal = 1;
			joyconData.now.vertical = 1;
			break;
		case 2:
			joyconData.now.horizontal = 1;
			joyconData.now.vertical = 0;
			break;
		case 3:
			joyconData.now.horizontal = 1;
			joyconData.now.vertical = -1;
			break;
		case 4:
			joyconData.now.horizontal = 0;
			joyconData.now.vertical = -1;
			break;
		case 5:
			joyconData.now.horizontal = -1;
			joyconData.now.vertical = -1;
			break;
		case 6:
			joyconData.now.horizontal = -1;
			joyconData.now.vertical = 0;
			break;
		case 7:
			joyconData.now.horizontal = -1;
			joyconData.now.vertical = 1;
			break;
		case 8:
			joyconData.now.horizontal = 0;
			joyconData.now.vertical = 0;
			break;
		}
	}
}

void Joycon::GetPathName(vector<wstring>& devicePath, GUID& guid, HDEVINFO& deviceInfo, SP_DEVICE_INTERFACE_DATA& deviceInfoData)
{
	HRESULT error{};
	bool boolError{};

	//関数の処理が失敗するまで呼び出す
	for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfo, 0, &guid, i, &deviceInfoData); ++i) {
		//エラー情報を取得
		error = CHECK_ERROR;
		//得られるデータの大きさを取得する
		DWORD size{};
		boolError = SetupDiGetDeviceInterfaceDetail(deviceInfo, &deviceInfoData, NULL, 0, &size, NULL);
		//エラー情報を取得
		error = CHECK_ERROR;
		//データを一時的に保存しておくためのバッファ
		LPBYTE dataBuffer = new BYTE[size];
		PSP_DEVICE_INTERFACE_DETAIL_DATA data = (PSP_DEVICE_INTERFACE_DETAIL_DATA)dataBuffer;
		data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		//エラー情報を取得
		error = CHECK_ERROR;
		//パス名を取得
		boolError = SetupDiGetDeviceInterfaceDetail(deviceInfo, &deviceInfoData, data, size, NULL, NULL);
		//パス名を保存
		devicePath.push_back(data->DevicePath);

		//バッファの解放
		delete[] dataBuffer;
	}
}

void Joycon::GetJoyconHandle(vector<wstring> devicePath)
{
	HRESULT error{};
	bool boolError{};

	//パスをもとにファイルを開き、そのファイルの属性(attribute)を調べる
	for (int i = 0; i < devicePath.size(); ++i) {
		HANDLE madeHandle = CreateFileW(devicePath[i].c_str(), FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		//エラー情報を取得	
		error = CHECK_ERROR;
		//解析データを取得
		PHIDP_PREPARSED_DATA preparsedData{};
		if (HidD_GetPreparsedData(madeHandle, &preparsedData)) {
			//解析データが取得できた場合、製品名を取得
			WCHAR productName[MAX_PRODUCTNAME];
			ULONG productNameSize = sizeof(WCHAR) * MAX_PRODUCTNAME;
			bool result = HidD_GetProductString(madeHandle, productName, productNameSize);
			//取得した製品名をstring型に治す
			wstring product = productName;
			//デバイスの機能を取得
			NTSTATUS ntstatus = HidP_GetCaps(preparsedData, &joyconCaps);
			//joyconを探す
			if (product == L"Wireless Gamepad" && joyconCaps.NumberInputValueCaps == 10) {
				//ハンドルを保存
				joyconHandle = madeHandle;
				//エラー情報を取得	
				error = CHECK_ERROR;
				//OVERLAPPED構造体の初期化
				joyconOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
				//エラー情報を取得	
				error = CHECK_ERROR;
				//入力バッファのサイズを変える
				joyconInputBuffer = new BYTE[joyconCaps.InputReportByteLength];
				//出力バッファのサイズを変える
				joyconOutputBuffer = new BYTE[joyconCaps.OutputReportByteLength];
				//中身を初期化
				ZeroMemory(joyconInputBuffer, joyconCaps.InputReportByteLength);
				ZeroMemory(joyconOutputBuffer, joyconCaps.OutputReportByteLength);
				error = CHECK_ERROR;
				break;
			}
		}
		//解放
		if (preparsedData != NULL) {
			HidD_FreePreparsedData(preparsedData);
		}
		//ファイルを閉じる
		if (madeHandle != NULL) {
			CloseHandle(madeHandle);
		}
	}
}

void Joycon::ChangeInputReport()
{
	if (joyconOutputBuffer == nullptr) return;
	//出力レポートの中身を初期化
	ZeroMemory(joyconOutputBuffer, joyconCaps.OutputReportByteLength);
	//joyconの出力レポートの中身を6軸センサーを有効化させる命令に変える
	joyconOutputBuffer[0] = 1;				//出力レポートのid。ランブルデータとサブコマンドを送ることを示す
	joyconOutputBuffer[1] = 0;				//グローバルパケットナンバー。送信するたびにインクリメントされる。最初なので0
	/*2~10にはランブルデータ(多分振動とか)が入る*/
	joyconOutputBuffer[10] = 0x03;			//サブコマンドのid。入力レポートの形式を変更するコマンド。
	joyconOutputBuffer[11] = 0x3F;			//サブコマンド。入力レポートを標準のフルモードに変更する。
	//出力レポートを送信
	auto wirteResult = WriteFile(joyconHandle, joyconOutputBuffer, joyconCaps.OutputReportByteLength, NULL, &joyconOverlapped);
}

void Joycon::Enable6axisSensor()
{
	if (joyconOutputBuffer == nullptr) return;
	//出力レポートの中身を初期化
	ZeroMemory(joyconOutputBuffer, joyconCaps.OutputReportByteLength);
	//joyconの6軸データを有効化する
	joyconOutputBuffer[0] = 1;
	joyconOutputBuffer[1] = 1;
	joyconOutputBuffer[10] = 0x40;			//サブコマンドのid。6軸センサーの状態を指定する。
	joyconOutputBuffer[11] = 0x01;			//6軸センサーを起動。
	//出力レポートを送信
	auto wirteResult = WriteFile(joyconHandle, joyconOutputBuffer, joyconCaps.OutputReportByteLength, NULL, &joyconOverlapped);
}

int16_t SelfUintToInt(uint16_t target) {
	int16_t buff{};
	char* targetPointer = (char*)&target;
	char* bPointer = (char*)&buff;
	memcpy(bPointer, targetPointer, sizeof(target));
	return buff;
}
