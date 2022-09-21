#pragma once
#include <vector>
#include <memory>

class PolygonMeshInstance;
class BLAS;

class PlayerModel {

public:

	/*===== �����o�ϐ� =====*/

	// �ԑ�
	std::weak_ptr<BLAS> carBodyBlas_;		// �Ԃ̃��f����BLAS�̃C���f�b�N�X
	std::weak_ptr<PolygonMeshInstance> carBodyInstance;		// �Ԃ̃��f���̃C���X�^���X�̃C���f�b�N�X
	// �o���p�[
	std::weak_ptr<BLAS> carBumperBlas_;
	std::weak_ptr<PolygonMeshInstance> carBumperInstance;
	// ���C�g
	std::weak_ptr<BLAS> carLeftLightBlas_;
	std::weak_ptr<PolygonMeshInstance> carLeftLightInstance;
	// ���C�g
	std::weak_ptr<BLAS> carRightLightBlas_;
	std::weak_ptr<PolygonMeshInstance> carRightLightInstance;
	// ���C�g
	std::weak_ptr<BLAS> carFrontLightBlas_;
	std::weak_ptr<PolygonMeshInstance> carFrontLightInstance;
	// ��
	std::weak_ptr<BLAS> carMirrorBlas_;
	std::weak_ptr<PolygonMeshInstance> carMirrorInstance;
	// ���̃J�o�[
	std::weak_ptr<BLAS> carMirrorCoverBlas_;
	std::weak_ptr<PolygonMeshInstance> carMirrorCoverInstance;
	// ��
	std::weak_ptr<BLAS> carWindowBlas_;
	std::weak_ptr<PolygonMeshInstance> carWindowInsInstance;
	// ���^�C��
	std::weak_ptr<BLAS> carBehindTireBlas_;
	std::weak_ptr<PolygonMeshInstance> carBehindTireInstance;
	// ���^�C���̃t���[��
	std::weak_ptr<BLAS> carBehindTireFrameBlas_;
	std::weak_ptr<PolygonMeshInstance> carBehindTireFrameInstance;
	// �E�O�^�C��
	std::weak_ptr<BLAS> carRightTireBlas_;
	std::weak_ptr<PolygonMeshInstance> carRightTireInstance;
	// �E�O�^�C���̃t���[��
	std::weak_ptr<BLAS> carRightTireFrameBlas_;
	std::weak_ptr<PolygonMeshInstance> carRightTireFrameInstance;
	// ���O�^�C��
	std::weak_ptr<BLAS> carLeftTireBlas_;
	std::weak_ptr<PolygonMeshInstance> carLeftTireInstance;
	// ���O�^�C���̃t���[��
	std::weak_ptr<BLAS> carLeftTireFrameBlas_;
	std::weak_ptr<PolygonMeshInstance> carLeftTireFrameInstance;

	// �ԑ̂ƃ^�C���ŏ����𕪂��邽�߂̕ϐ�
	std::vector<std::weak_ptr<PolygonMeshInstance>> bodyInstance;
	std::vector<std::weak_ptr<PolygonMeshInstance>> tireInstance;

	// ���g���C��BLAS����p�ϐ�
	std::vector<std::weak_ptr<BLAS>> carBlasIndex_;

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