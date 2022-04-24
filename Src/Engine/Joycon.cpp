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
	/*-----�ϐ��錾-----*/
	vector<wstring> devicePath;						//�f�o�C�X�̃p�X����ۑ����Ă������߂̃R���e�i
	GUID guid{};									//hid�h���C�o�̎��ʂ�ۑ����Ă������߂̕ϐ�
	HDEVINFO deviceInfo{};							//hid�f�o�C�X�̃��X�g
	SP_DEVICE_INTERFACE_DATA deviceInfoData{};		//hid�f�o�C�X�̃C���^�[�t�F�[�X�f�[�^

	//HID�h���C�o�̎��ʎq��Ԃ�
	HidD_GetHidGuid(&guid);

	//HID�f�o�C�X�̃��X�g���擾���邽�߂ɁAHID�h���C�o�ɖ₢���킹��
	deviceInfo = SetupDiGetClassDevs(&guid, NULL, DirectXBase::Instance()->windowsAPI.hwnd, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	//�C���^�[�t�F�[�X�������\���̂��\�z����
	deviceInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	//�f�o�C�X�̃p�X�����擾
	GetPathName(devicePath, guid, deviceInfo, deviceInfoData);

	//HID�f�o�C�X���X�g�����
	SetupDiDestroyDeviceInfoList(deviceInfo);

	//joycon��handle���擾
	GetJoyconHandle(devicePath);

	//���̓��|�[�g��W�����̓��|�[�g�`���ɕύX
	ChangeInputReport();

	//���|�[�g�ƃ��|�[�g�̊Ԃ͑������Ԃ��J�����ق��������炵���̂ŁA��莞�ԃX���[�v����
	//Sleep(1 * 1000);

	//6���Z���T�[��L����
	//Enable6axisSensor();
}

void Joycon::UpdateJoyconData()
{
	HRESULT error{};

	if (joyconInputBuffer) {
		//�O�t���[���̓��͏���ۑ�
		joyconData.old = joyconData.now;

		//joycon�̓��̓��|�[�g���擾
		auto a = ReadFile(joyconHandle, joyconInputBuffer, joyconCaps.InputReportByteLength, NULL, &joyconOverlapped);
		//�G���[�`�F�b�N
		error = CHECK_ERROR;

		//�f�o�b�O�p ���̓��|�[�g�̌`�����擾
		auto report = joyconInputBuffer[0];

		/*-----�{�^���X�e�[�^�X�̊���-----*/ //�����ł�joycon(R)�̃X�e�[�^�X���������Ă��邪�A���݂�joycon��RL�ǂ���Ȃ̂����擾���ď������ŕ�������
		joyconData.now.rButton.a = (joyconInputBuffer[1] << 0) & 1;
		joyconData.now.rButton.x = (joyconInputBuffer[1] >> 1) & 1;
		joyconData.now.rButton.b = (joyconInputBuffer[1] >> 2) & 1;
		joyconData.now.rButton.y = (joyconInputBuffer[1] >> 3) & 1;
		joyconData.now.rButton.sl = (joyconInputBuffer[1] >> 4) & 1;
		joyconData.now.rButton.sr = (joyconInputBuffer[1] >> 5) & 1;
		joyconData.now.rButton.r = (joyconInputBuffer[1] >> 6) & 1;
		joyconData.now.rButton.zr = (joyconInputBuffer[1] >> 7) & 1;

		/*-----�W���C���̊���-----*/
		//�W���C���̐��f�[�^���擾
		//int16_t rawGyroDataX = SelfUintToInt((joyconInputBuffer[20] << 8) | (joyconInputBuffer[19]));
		//int16_t rawGyroDataY = SelfUintToInt((joyconInputBuffer[22] << 8) | (joyconInputBuffer[21]));
		//int16_t rawGyroDataZ = SelfUintToInt((joyconInputBuffer[24] << 8) | (joyconInputBuffer[23]));
		////�W���C���̐��f�[�^���Z���ς݃W���C���f�[�^�ɕϊ������邽�߂̃I�t�Z�b�g�ƌW��
		//auto calGyroOffset = SelfUintToInt(0x000E);
		//auto gyroCalcoeff = (float)(816.0f / (float)(0x343B - calGyroOffset));
		////�I�t�Z�b�g�ƌW����p���Đ��f�[�^��ϊ�
		//joyconData.now.gyro.x = (float)(rawGyroDataX - calGyroOffset) * gyroCalcoeff;
		//fabs(joyconData.now.gyro.x) < 10 ? joyconData.now.gyro.x = 0 : joyconData.now.gyro.x = joyconData.now.gyro.x / 1000.0f;
		//joyconData.now.gyro.y = (float)(rawGyroDataY - calGyroOffset) * gyroCalcoeff;
		//fabs(joyconData.now.gyro.y) < 10 ? joyconData.now.gyro.y = 0 : joyconData.now.gyro.y = joyconData.now.gyro.y / 1000.0f;
		//joyconData.now.gyro.z = (float)(rawGyroDataZ - calGyroOffset) * gyroCalcoeff;
		//fabs(joyconData.now.gyro.z) < 10 ? joyconData.now.gyro.z = 0 : joyconData.now.gyro.z = joyconData.now.gyro.z / 1000.0f;
		////joycon��̍��W����DirectX�̍��W�����Ⴄ���߁A���킹�邽�߂�y����z�������ւ���
		//float buff = joyconData.now.gyro.y;
		//joyconData.now.gyro.y = joyconData.now.gyro.z;
		//joyconData.now.gyro.z = buff;
		////�W���C���̓������̊��̓����Ƃ͔��΂ɂȂ��Ă��܂����߁A�}�C�i�X��������
		//joyconData.now.gyro.x *= -1;
		//joyconData.now.gyro.y *= -1;
		//joyconData.now.gyro.z *= -1;

		/*-----�W�����̓��|�[�g�p-----*/
		////�X�e�B�b�N���̊��蓖��
		//joyconData.now.horizontal = SelfUintToInt((joyconInputBuffer[10] >> 4) | (joyconInputBuffer[11] << 4));
		//joyconData.now.horizontal -= 2000;
		////�f�b�h���C����݂���
		//if (abs(joyconData.now.horizontal) < 200) {
		//	joyconData.now.horizontal = 0;
		//}
		////0����Ȃ�������I�t�Z�b�g�l�Ŋ�����-1~1�͈̔͂ɂ���
		//if (joyconData.now.horizontal != 0) {
		//	joyconData.now.horizontal = joyconData.now.horizontal / 1200.0f;
		//}
		//joyconData.now.vertical = SelfUintToInt(joyconInputBuffer[9] | ((joyconInputBuffer[10] & 0xF) << 8));
		//joyconData.now.vertical -= 2000;
		////�f�b�h���C����݂���
		//if (abs(joyconData.now.vertical) < 200) {
		//	joyconData.now.vertical = 0;
		//}
		////0����Ȃ�������I�t�Z�b�g�l�Ŋ�����-1~1�͈̔͂ɂ���
		//if (joyconData.now.vertical != 0) {
		//	joyconData.now.vertical = joyconData.now.vertical / 1200.0f;
		//}
		////�������t�̂ق����킩��₷���̂Ŕ��]������
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

	//�֐��̏��������s����܂ŌĂяo��
	for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfo, 0, &guid, i, &deviceInfoData); ++i) {
		//�G���[�����擾
		error = CHECK_ERROR;
		//������f�[�^�̑傫�����擾����
		DWORD size{};
		boolError = SetupDiGetDeviceInterfaceDetail(deviceInfo, &deviceInfoData, NULL, 0, &size, NULL);
		//�G���[�����擾
		error = CHECK_ERROR;
		//�f�[�^���ꎞ�I�ɕۑ����Ă������߂̃o�b�t�@
		LPBYTE dataBuffer = new BYTE[size];
		PSP_DEVICE_INTERFACE_DETAIL_DATA data = (PSP_DEVICE_INTERFACE_DETAIL_DATA)dataBuffer;
		data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		//�G���[�����擾
		error = CHECK_ERROR;
		//�p�X�����擾
		boolError = SetupDiGetDeviceInterfaceDetail(deviceInfo, &deviceInfoData, data, size, NULL, NULL);
		//�p�X����ۑ�
		devicePath.push_back(data->DevicePath);

		//�o�b�t�@�̉��
		delete[] dataBuffer;
	}
}

void Joycon::GetJoyconHandle(vector<wstring> devicePath)
{
	HRESULT error{};
	bool boolError{};

	//�p�X�����ƂɃt�@�C�����J���A���̃t�@�C���̑���(attribute)�𒲂ׂ�
	for (int i = 0; i < devicePath.size(); ++i) {
		HANDLE madeHandle = CreateFileW(devicePath[i].c_str(), FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		//�G���[�����擾	
		error = CHECK_ERROR;
		//��̓f�[�^���擾
		PHIDP_PREPARSED_DATA preparsedData{};
		if (HidD_GetPreparsedData(madeHandle, &preparsedData)) {
			//��̓f�[�^���擾�ł����ꍇ�A���i�����擾
			WCHAR productName[MAX_PRODUCTNAME];
			ULONG productNameSize = sizeof(WCHAR) * MAX_PRODUCTNAME;
			bool result = HidD_GetProductString(madeHandle, productName, productNameSize);
			//�擾�������i����string�^�Ɏ���
			wstring product = productName;
			//�f�o�C�X�̋@�\���擾
			NTSTATUS ntstatus = HidP_GetCaps(preparsedData, &joyconCaps);
			//joycon��T��
			if (product == L"Wireless Gamepad" && joyconCaps.NumberInputValueCaps == 10) {
				//�n���h����ۑ�
				joyconHandle = madeHandle;
				//�G���[�����擾	
				error = CHECK_ERROR;
				//OVERLAPPED�\���̂̏�����
				joyconOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
				//�G���[�����擾	
				error = CHECK_ERROR;
				//���̓o�b�t�@�̃T�C�Y��ς���
				joyconInputBuffer = new BYTE[joyconCaps.InputReportByteLength];
				//�o�̓o�b�t�@�̃T�C�Y��ς���
				joyconOutputBuffer = new BYTE[joyconCaps.OutputReportByteLength];
				//���g��������
				ZeroMemory(joyconInputBuffer, joyconCaps.InputReportByteLength);
				ZeroMemory(joyconOutputBuffer, joyconCaps.OutputReportByteLength);
				error = CHECK_ERROR;
				break;
			}
		}
		//���
		if (preparsedData != NULL) {
			HidD_FreePreparsedData(preparsedData);
		}
		//�t�@�C�������
		if (madeHandle != NULL) {
			CloseHandle(madeHandle);
		}
	}
}

void Joycon::ChangeInputReport()
{
	if (joyconOutputBuffer == nullptr) return;
	//�o�̓��|�[�g�̒��g��������
	ZeroMemory(joyconOutputBuffer, joyconCaps.OutputReportByteLength);
	//joycon�̏o�̓��|�[�g�̒��g��6���Z���T�[��L���������閽�߂ɕς���
	joyconOutputBuffer[0] = 1;				//�o�̓��|�[�g��id�B�����u���f�[�^�ƃT�u�R�}���h�𑗂邱�Ƃ�����
	joyconOutputBuffer[1] = 0;				//�O���[�o���p�P�b�g�i���o�[�B���M���邽�тɃC���N�������g�����B�ŏ��Ȃ̂�0
	/*2~10�ɂ̓����u���f�[�^(�����U���Ƃ�)������*/
	joyconOutputBuffer[10] = 0x03;			//�T�u�R�}���h��id�B���̓��|�[�g�̌`����ύX����R�}���h�B
	joyconOutputBuffer[11] = 0x3F;			//�T�u�R�}���h�B���̓��|�[�g��W���̃t�����[�h�ɕύX����B
	//�o�̓��|�[�g�𑗐M
	auto wirteResult = WriteFile(joyconHandle, joyconOutputBuffer, joyconCaps.OutputReportByteLength, NULL, &joyconOverlapped);
}

void Joycon::Enable6axisSensor()
{
	if (joyconOutputBuffer == nullptr) return;
	//�o�̓��|�[�g�̒��g��������
	ZeroMemory(joyconOutputBuffer, joyconCaps.OutputReportByteLength);
	//joycon��6���f�[�^��L��������
	joyconOutputBuffer[0] = 1;
	joyconOutputBuffer[1] = 1;
	joyconOutputBuffer[10] = 0x40;			//�T�u�R�}���h��id�B6���Z���T�[�̏�Ԃ��w�肷��B
	joyconOutputBuffer[11] = 0x01;			//6���Z���T�[���N���B
	//�o�̓��|�[�g�𑗐M
	auto wirteResult = WriteFile(joyconHandle, joyconOutputBuffer, joyconCaps.OutputReportByteLength, NULL, &joyconOverlapped);
}

int16_t SelfUintToInt(uint16_t target) {
	int16_t buff{};
	char* targetPointer = (char*)&target;
	char* bPointer = (char*)&buff;
	memcpy(bPointer, targetPointer, sizeof(target));
	return buff;
}
