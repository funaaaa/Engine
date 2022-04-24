#pragma once
#include "SpriteGaussian.h"
#include "RenderTarget.h"

#define NUM_GAUSS_WEIGHT 8

class Gaussian {
public:
	RenderTarget gaussX;
	RenderTarget gaussY;
	SpriteGaussian gaussXSprite;
	SpriteGaussian gaussYSprite;

	//�R���X�g���N�^
	Gaussian() {};

	void Init();
};

class MultiPathFunction : public Singleton<MultiPathFunction> {
private:
	//�R���X�g���N�^
	friend Singleton<MultiPathFunction>;

public:

	static Gaussian gaussian;			//�K�E�V�A���ڂ���������ۂɎg�p����ϐ�

	MultiPathFunction() {
		gaussian.Init();
	};

public:

	//���\�[�X�o���A�֌W
	void ResourceBarrierBefore(RenderTarget renderTarget);
	void ResourceBarrierAfter(RenderTarget renderTarget);
	void ResourceBarriersAfter(UINT num, RenderTarget* renderTargets[]);
	//�����_�[�^�[�Q�b�g�Ɛ[�x�o�b�t�@���N���A
	void ClearRtvDsv(RenderTarget renderTarget);
	//�����_�[�^�[�Q�b�g��ݒ�
	void SetRenderTargetAndClear(RenderTarget renderTarget);
	void SetRenderTarget(RenderTarget renderTarget);
	//MRT�p�̃����_�[�^�[�Q�b�g�ݒ�
	void SetRenderTargetsAndClear(UINT num, RenderTarget* renderTargets[]);
	void SetRenderTargets(UINT num, RenderTarget* renderTargets[]);

	//�K�E�X�ڂ���
	//void GaussianBlur(SpriteMultiPath xBlur, RenderTarget xBlurTarget, SpriteMultiPath yBlur, RenderTarget yBlurTarget);
	void GaussianBlur(XMFLOAT2 textureSize, float blurAmount, int blurCount, int targetTextureID, RenderTarget& exportTarget);
	//�K�E�X�ڂ����̂ڂ����ʂ�ݒ�
	void SetGaussianAmount(SpriteGaussian& xBlur, SpriteGaussian& yBlur, float amount);
	//�d�݂̌v�Z
	void CalcWeightsTableFromGaussian(vector<float>& weightsTbl, int sizeOfWeightsTbl, float sigma);

};