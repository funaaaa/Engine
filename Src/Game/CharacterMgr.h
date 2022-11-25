#pragma once
#include <memory>
#include <vector>
#include "Character.h"
#include "ConstBuffers.h"
#include "FHelper.h"

class Character;
class BaseStage;

class CharacterMgr {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<Character>> character_;
	int playerIndex_;

	// ���C�ƃ��b�V���̓����蔻��̍ۂɎg�p����A���C�̊J�n�n�_�Ǝˏo�������܂Ƃ߂��֐��B
	struct RayPosDir {
		Vec3 pos_;
		Vec3 dir_;
	};


public:

	/*===== �����o�֐� =====*/

	CharacterMgr();
	void Init();
	void Update(std::weak_ptr<BaseStage> Stage, bool IsBeforeStart, bool IsGameFinish);
	void Draw();

	void AddChara(int CharaID, bool IsPlayer, int Param = 0);
	bool CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, std::vector<Character::TireMaskUV>& TireMaskUVData);
	inline std::weak_ptr<Character> GetPlayerIns() { return character_[playerIndex_]; }

	// �����ꂩ�̃L�������S�[�����������`�F�b�N����B
	bool CheckGoal();

private:

	// �w��̎Ԃ̃��C�̎ˏo�ʒu�����߂�B
	void CalRayPosDir(int Index, RayPosDir& FrontRight, RayPosDir& FrontLeft, RayPosDir& BehindRight, RayPosDir& BehindLeft);

	// �ԂƂ̓����蔻��
	void CheckHitCar(const FHelper::RayToModelCollisionData& CollisionData, std::weak_ptr<Character> IndexCharacter);

};