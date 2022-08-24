#pragma once
#include <vector>

class PlayerModel {

public:

	/*===== �����o�ϐ� =====*/

	// �ԑ�
	int carBodyBlasIndex_;		// �Ԃ̃��f����BLAS�̃C���f�b�N�X
	int carBodyInsIndex_;		// �Ԃ̃��f���̃C���X�^���X�̃C���f�b�N�X
	// �o���p�[
	int carBumperBlasIndex_;
	int carBumperInsIndex_;
	// ���C�g
	int carLeftLightBlasIndex_;
	int carLeftLightInsIndex_;
	// ���C�g
	int carRightLightBlasIndex_;
	int carRightLightInsIndex_;
	// ���C�g
	int carFrontLightBlasIndex_;
	int carFrontLightInsIndex_;
	// ��
	int carMirrorBlasIndex_;
	int carMirrorInsIndex_;
	// ���̃J�o�[
	int carMirrorCoverBlasIndex_;
	int carMirrorCoverInsIndex_;
	// ��
	int carWindowBlasIndex_;
	int carWindowInsIndex_;
	// ���^�C��
	int carBehindTireBlasIndex_;
	int carBehindTireInsIndex_;
	// ���^�C���̃t���[��
	int carBehindTireFrameBlasIndex_;
	int carBehindTireFrameInsIndex_;
	// �E�O�^�C��
	int carRightTireBlasIndex_;
	int carRightTireInsIndex_;
	// �E�O�^�C���̃t���[��
	int carRightTireFrameBlasIndex_;
	int carRightTireFrameInsIndex_;
	// ���O�^�C��
	int carLeftTireBlasIndex_;
	int carLeftTireInsIndex_;
	// ���O�^�C���̃t���[��
	int carLeftTireFrameBlasIndex_;
	int carLeftTireFrameInsIndex_;

	// �ԑ̂ƃ^�C���ŏ����𕪂��邽�߂̕ϐ�
	std::vector<int> bodyInsIndex_;
	std::vector<int> tireInsIndex_;

public:

	/*===== �����o�֐� =====*/

	enum class COLOR {

		RED,
		BLUE,

	};

	// ���[�h
	void Load(COLOR ColorID, bool IsGhost);

};

/*

�E�ݒu�����u�Ԃɒ��߂鏈���������B����

*/