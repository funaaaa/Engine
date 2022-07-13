#pragma once

class PlayerModel {

public:

	/*===== �����o�ϐ� =====*/

	// �ԑ�
	int carBodyBlasIndex;		// �Ԃ̃��f����BLAS�̃C���f�b�N�X
	int carBodyInsIndex;		// �Ԃ̃��f���̃C���X�^���X�̃C���f�b�N�X
	// �o���p�[
	int carBumperBlasIndex;
	int carBumperInsIndex;
	// ���C�g
	int carLightBlasIndex;
	int carLightInsIndex;
	// ���^�C��
	int carBehindTireBlasIndex;
	int carBehindTireInsIndex;
	// ���^�C���̃t���[��
	int carBehindTireFrameBlasIndex;
	int carBehindTireFrameInsIndex;
	// �E�O�^�C��
	int carRightTireBlasIndex;
	int carRightTireInsIndex;
	// �E�O�^�C���̃t���[��
	int carRightTireFrameBlasIndex;
	int carRightTireFrameInsIndex;
	// ���O�^�C��
	int carLeftTireBlasIndex;
	int carLeftTireInsIndex;
	// ���O�^�C���̃t���[��
	int carLeftTireFrameBlasIndex;
	int carLeftTireFrameInsIndex;
	// ��
	int carMirrorBlasIndex;
	int carMirrorInsIndex;
	// ���̃J�o�[
	int carMirrorCoverBlasIndex;
	int carMirrorCoverInsIndex;
	// ��
	int carWindowBlasIndex;
	int carWindowInsIndex;

public:

	/*===== �����o�֐� =====*/

	// ���[�h
	void Load();

};