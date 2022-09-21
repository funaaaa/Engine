#pragma once
#include <vector>
#include <memory>

class PolygonMeshInstance;

class PlayerModel {

public:

	/*===== �����o�ϐ� =====*/

	// �ԑ�
	int carBodyBlasIndex_;		// �Ԃ̃��f����BLAS�̃C���f�b�N�X
	std::weak_ptr<PolygonMeshInstance> carBodyInstance;		// �Ԃ̃��f���̃C���X�^���X�̃C���f�b�N�X
	// �o���p�[
	int carBumperBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carBumperInstance;
	// ���C�g
	int carLeftLightBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carLeftLightInstance;
	// ���C�g
	int carRightLightBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carRightLightInstance;
	// ���C�g
	int carFrontLightBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carFrontLightInstance;
	// ��
	int carMirrorBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carMirrorInstance;
	// ���̃J�o�[
	int carMirrorCoverBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carMirrorCoverInstance;
	// ��
	int carWindowBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carWindowInsInstance;
	// ���^�C��
	int carBehindTireBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carBehindTireInstance;
	// ���^�C���̃t���[��
	int carBehindTireFrameBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carBehindTireFrameInstance;
	// �E�O�^�C��
	int carRightTireBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carRightTireInstance;
	// �E�O�^�C���̃t���[��
	int carRightTireFrameBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carRightTireFrameInstance;
	// ���O�^�C��
	int carLeftTireBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carLeftTireInstance;
	// ���O�^�C���̃t���[��
	int carLeftTireFrameBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carLeftTireFrameInstance;

	// �ԑ̂ƃ^�C���ŏ����𕪂��邽�߂̕ϐ�
	std::vector<std::weak_ptr<PolygonMeshInstance>> bodyInstance;
	std::vector<std::weak_ptr<PolygonMeshInstance>> tireInstance;

	// ���g���C��BLAS����p�ϐ�
	std::vector<int> carBlasIndex_;

public:

	/*===== �����o�֐� =====*/

	enum class COLOR {

		RED,
		BLUE,

	};

	// ���[�h
	void Load(COLOR ColorID, bool IsGhost);
	// �폜
	void Delete();

};