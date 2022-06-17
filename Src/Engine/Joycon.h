//#pragma once
//#include <hidsdi.h>
//#include <SetupAPI.h>
//
////���̃N���X�ł͂܂��X�e�B�b�N���������Ă��Ȃ�
//
///*-----�\���̐錾-----*/
////joycon(R)�̃{�^�����
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
////joycon(L)�̃{�^�����
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
////joycon�̃W���C�����
//struct JoyconGyro {
//	float x;
//	float y;
//	float z;
//};
////joycon��1�t���[�����Ƃ̓��͏��\����
//struct JoyconFrameData{
//	JoyconRightButton rButton;			//joycon(R)�̃{�^�����
//	JoyconLightButton lButton;			//joycon(L)�̃{�^�����
//	JoyconGyro gyro;					//joycon�̃W���C�����
//	float horizontal;					//�X�e�B�b�N�̐������͏��
//	float vertical;						//�X�e�B�b�N�̐������͏��
//};
////joycon�̃f�[�^�̍\����
//struct JoyconData {
//	JoyconFrameData now;				//���̃t���[���̓��͏��
//	JoyconFrameData old;				//�O�̃t���[���̓��͏��
//};
//
//class Joycon {
//public:
//	/*�����o�ϐ�*/
//	JoyconData joyconData;				//joycon�̓��͏��
//	HANDLE joyconHandle{};				//joycon�̃n���h����ۑ�����ϐ�
//	HIDP_CAPS joyconCaps{};				//joycon���瑗���Ă���f�[�^�̃T�C�Y�Ȃǂ��������\����
//	BYTE* joyconInputBuffer{};			//joycon���瑗���Ă���f�[�^
//	BYTE* joyconOutputBuffer{};			//joycon�ɑ���f�[�^
//	OVERLAPPED joyconOverlapped{};		//joycon�̔񓯊������p
//
//	//�R���X�g���N�^
//	Joycon();
//	//joycon�����[�h		����WirelessGamepad�ł����ǂݎ��Ȃ��̂�joycon(R)��(L)�Ƃ��ēǂݎ���悤�ɂ���B
//	void LoadJoycon();
//	//joycon�̃f�[�^���A�b�v�f�[�g
//	void UpdateJoyconData();
//
//	//hid�̃��X�g����擾�\�ȃf�o�C�X��T���A�p�X�����擾���Ĕz��ɕۑ�����
//	void GetPathName(vector<wstring>& devicePath, GUID& guid, HDEVINFO& deviceInfo, SP_DEVICE_INTERFACE_DATA& deviceInfoData);
//	//joycon��handle���擾����
//	void GetJoyconHandle(vector<wstring> devicePath);
//	//joycon�̓��̓��|�[�g��W�����̓��|�[�g�`���ɂ���o�̓��|�[�g�𑗐M
//	void ChangeInputReport();
//	//joycon��6���Z���T�[��L����������
//	void Enable6axisSensor();
//};
//
////unsigned int����int�^�ւ̕ϊ�
//int16_t SelfUintToInt(uint16_t target);