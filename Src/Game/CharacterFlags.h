#pragma once

// Character�Ŏg�p����t���O���܂Ƃ߂��\���́B
struct CharacterFlags {

public:

	/*===== �����o�ϐ� =====*/

	/*-- ��ʓI�ȃt���O --*/
	bool isShotBehind_;			// ��둤�ɍb���𓊂��邩�̃t���O
	bool onGround_;				// �n��ɂ��邩 t=�n�� f=��
	bool onGroundPrev_;			// �O�t���[����onGround_
	bool onGrass_;				// ���̏�ɂ��邩 t=���̏� f=���̏ザ��Ȃ�
	bool isConcentrationLine_;	// �W�������o�����t���O�B
	
	/*-- �A�C�e���֌W --*/
	bool isGetItem_;	// �A�C�e�����擾�����t���[���̔���
	bool isUseItem_;			// �A�C�e�����g�����u�ԁB
	
	/*-- �W�����v�A�N�V�����A�M�~�b�N�֌W --*/
	bool isHitJumpActionGimmick_;
	bool isJumpActionTrigger_;	// �W�����v�A�N�V�����̃g���K�[
	bool isJumpAction_;			// �W�����v�A�N�V�������s��ꂽ���B	

	/*-- �Q�[���J�n�O�֌W --*/
	bool isAccel_;
	bool isBeforeStartPrev_;


public:

	/*===== �����o�֐� =====*/

	void Init();

};