#include "DevDXR.h"

#define COLORHEX(hex) hex / 255.0f

#define SCREEN_VIRTUAL_WIDTH 300

<<<<<<< HEAD
// fpsçXêV
void FPS();

XMFLOAT2 CalRepulsionRatio(const float& v1, const float& m1, const float& v2, const float& m2)
{
	XMFLOAT2 honraiVel = { 0,0 };

	//îΩî≠åWêî
	float e = 1.0f;

	float honraiV1 = ((m1 - m2) * v1 + (2 * m2 * v2)) / (m1 + m2);
	float honraiV2 = ((m2 - m1) * v2 + (2 * m1 * v1)) / (m1 + m2);


	honraiVel.x = honraiV1;
	honraiVel.y = honraiV2;

	return honraiVel;
}

struct RayPointLightData {

	Vec3 lightPos;
	float lightSize;
	Vec3 lightColor;
	float lightPower;
	int isActive;
	Vec3 pad;

};

struct KariConstBufferData {

	XMMATRIX mtxView;			// ÉrÉÖÅ[çsóÒÅB
	XMMATRIX mtxProj;			// ÉvÉçÉWÉFÉNÉVÉáÉìçsóÒÅB
	XMMATRIX mtxViewInv;		// ÉrÉÖÅ[ãtçsóÒÅB
	XMMATRIX mtxProjInv;		// ÉvÉçÉWÉFÉNÉVÉáÉìãtçsóÒÅB
	XMVECTOR lightDirection;	// ïΩçsåıåπÇÃå¸Ç´ÅB
	XMVECTOR lightColor;		// ïΩçsåıåπêFÅB
	XMVECTOR ambientColor;		// ä¬ã´åıÅB
	RayPointLightData pointLight;
	int seed;
	int counter;
	int aoSampleCount;
	int isNoiseScene;
	int isNoiseOnlyScene;
	int isLightHitScene;
	int isNormalScene;
	int isMeshScene;
	int isNoAO;

};

// ÉfÉoÉbÉOópÇÃÉpÉCÉvÉâÉCÉìÇêÿÇËë÷Ç¶ÇÈÇ‚Ç¬ÅB
enum DEGU_PIPLINE_ID {
	DEF_PIPLINE,
	AO_PIPLINE,
	DENOISE_AO_PIPLINE,
};

// ì¸óÕëÄçÏ
void Input(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& degugPiplineID);

=======
>>>>>>> AtmosphericScattering
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	/*----------DirectXÂàùÊúüÂåñÂá¶ÁêÜ----------*/
	ImGuiWindow::Ins()->Init();
	DirectXBase::Ins()->Init();									// DirectXÂü∫Áõ§„ÅÆÂàùÊúüÂåñ
	SoundManager::Ins()->SettingSoundManager();	// „Çµ„Ç¶„É≥„Éâ„Éû„Éç„Éº„Ç∏„É£„Éº„Çí„Çª„ÉÉ„Éà„Åô„Çã

	/*----------„Éë„Ç§„Éó„É©„Ç§„É≥ÁîüÊàê----------*/
	PiplineManager::Ins()->Init();

	/*----------Â§âÊï∞ÂÆ£Ë®Ä----------*/
	srand(time(NULL));

	// „Éá„Ç£„Çπ„ÇØ„É™„Éó„Çø„Éí„Éº„Éó„ÇíÂàùÊúüÂåñ„ÄÇ
	DescriptorHeapMgr::Ins()->GenerateDescriptorHeap();

	// FBXLoader„ÇíÂàùÊúüÂåñ„ÄÇ
	FbxLoader::Ins()->Init();

	// „Éí„ÉÉ„Éà„Ç∞„É´„Éº„Éó„ÇíË®≠ÂÆö„ÄÇ
	HitGroupMgr::Ins()->Setting();

	// „Éá„Éé„Ç§„Ç∫Áî®„ÅÆ„ÇØ„É©„Çπ„ÇíÂàùÊúüÂåñ„ÄÇ
	Denoiser::Ins()->Setting();

<<<<<<< HEAD
	// SPONZAÇì«Ç›çûÇﬁÅB
	//std::vector<int> sponzaInstance = MultiMeshLoadOBJ::Ins()->RayMultiMeshLoadOBJ("Resource/", "sponza.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF_HIT_GROUP]);

	// ÉâÉCÉgópÇÃÉXÉtÉBÉAÇì«Ç›çûÇﬁÅB
	int sphereBlas = BLASRegister::Ins()->GenerateObj("Resource/", "sphere.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::AO_HIT_GROUP], { L"Resource/white.png" });
	int sphereIns = PorygonInstanceRegister::Ins()->CreateInstance(sphereBlas, 3);
	PorygonInstanceRegister::Ins()->AddScale(sphereIns, Vec3(15, 15, 15));
	PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, Vec3(0, 350, 100));

	int sphereIns2 = PorygonInstanceRegister::Ins()->CreateInstance(sphereBlas, 3);
	PorygonInstanceRegister::Ins()->AddScale(sphereIns2, Vec3(10, 10, 10));
	PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns2, Vec3(0, 250, 0));

	int sphereIns3 = PorygonInstanceRegister::Ins()->CreateInstance(sphereBlas, 3);
	PorygonInstanceRegister::Ins()->AddScale(sphereIns3, Vec3(12, 12, 12));
	PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns3, Vec3(0, 350, 100));




	Vec3 pos1 = Vec3(0, 10, -60);

	Vec3 pos2 = Vec3(0, -10, 60);

	Vec3 pos3 = Vec3(0, -10, 60);

	float angle = 0;
	float radius = 100;



	PorygonInstanceRegister::Ins()->CalWorldMat();

	// TLASÇê∂ê¨ÅB
	TLAS tlas;
	tlas.GenerateTLAS(L"TlasDescriptorHeap");

	// ÉåÉCÉgÉåèoóÕópÉNÉâÉXÇÉZÉbÉgÅB
	RaytracingOutput raytracingOutput;
	raytracingOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	RaytracingOutput raytracingOutputData;
	raytracingOutputData.Setting(DXGI_FORMAT_R32G32B32A32_FLOAT);

	// ÉVÉFÅ[É_Å[ÉeÅ[ÉuÉãÇê∂ê¨ÅB
	aoPipline.ConstructionShaderTable();
	deAOPipline.ConstructionShaderTable();
	defPipline.ConstructionShaderTable();

	// ÉfÉmÉCÉYópÇÃÉKÉEÉVÉAÉìÉuÉâÅ[ÉRÉìÉsÉÖÅ[ÉgÉVÉFÅ[É_Å[ÇÉZÉbÉgÅB
	//ComputeShader blurX;
	//blurX.Init(L"Resource/hlsl/Raytracing/GaussianBlur.hlsl",)

	// âºÇÃíËêîÉoÉbÉtÉ@ÇêÈåæ
	KariConstBufferData constBufferData;
	constBufferData.ambientColor = { 1,1,1,1 };
	constBufferData.lightColor = { 1,1,1,1 };
	constBufferData.lightDirection = { 0,1,-0.27f,0 };
	constBufferData.lightDirection = XMVector4Normalize(constBufferData.lightDirection);
	constBufferData.mtxProj = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),				//âÊäp(60ìx)
		(float)window_width / window_height,	//ÉAÉXÉyÉNÉgî‰
		0.1f, 1000000.0f							//ëOí[ÅAâúí[
	);
	constBufferData.mtxProjInv = XMMatrixInverse(nullptr, constBufferData.mtxProj);
	Vec3 eye = { 0,0,-10 };
	Vec3 target = { 0,0,0 };
	Vec3 up = { 0,1,0 };
	constBufferData.mtxView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	constBufferData.mtxViewInv = XMMatrixInverse(nullptr, constBufferData.mtxView);
	constBufferData.counter = 0;
	constBufferData.isNoiseScene = true;
	constBufferData.isNoiseOnlyScene = false;
	constBufferData.pointLight.lightPos = Vec3(0, 300, 0);
	constBufferData.pointLight.lightSize = 30.0f;
	constBufferData.pointLight.lightPower = 300.0f;
	constBufferData.aoSampleCount = 1;
	constBufferData.isLightHitScene = false;
	constBufferData.isNormalScene = false;
	constBufferData.isMeshScene = false;
	constBufferData.isNoAO = false;

	DynamicConstBuffer constBuff;
	constBuff.Generate(sizeof(KariConstBufferData), L"constBuffer");

	// ÉfÉoÉbÉOópÇ≈ÉmÉCÉYâÊñ ÇèoÇ∑ÉtÉâÉOÅB
	DEGU_PIPLINE_ID debugPiplineID = AO_PIPLINE;

	// ÉJÉÅÉâÇèâä˙âªÅB
	Camera::Ins()->Init();

	bool isHit = false;

	/*----------ÉQÅ[ÉÄÉãÅ[Év----------*/
	while (true) {
		/*----------ñàÉtÉåÅ[ÉÄèàóù(ï`âÊëOèàóù)----------*/
		directXBase.processBeforeDrawing();

		/*----- çXêVèàóù -----*/

		// ÉrÉÖÅ[çsóÒÇê∂ê¨ÅB
		Camera::Ins()->GenerateMatView();

		FPS();




		// ëÂÇ´Ç¢ï˚
		PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, pos1);
		// è¨Ç≥Ç¢ï˚
		PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns2, pos2);
		PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns3, pos3);

		// è¨Ç≥Ç¢ï˚ÇâÒì]Ç≥ÇπÇÈÅB
		angle += 0.1f;

		radius += 0.1f;

		pos2 = pos1 + Vec3(cosf(angle), 0, sinf(angle)) * radius;
		pos3 = pos1 + Vec3(cosf(angle), 0, sinf(angle)) * (radius * 2.0f);


		// óêêîÇÃéÌÇçXêVÅB
		constBufferData.seed = FHelper::GetRand(0, 1000);

		// ÉâÉCÉgÇ™ìÆÇ¢ÇΩÇ©
		bool isMoveLight = false;
		constBufferData.counter = 0;

		Input(constBufferData, isMoveLight, debugPiplineID);

		// ÉJÉÅÉâÇçXêVÅB
		Camera::Ins()->Update();

		eye = Camera::Ins()->eye;
		target = Camera::Ins()->target;
		up = Camera::Ins()->up;

		// ÉâÉCÉgÇ™ìÆÇ¢ÇΩÇ∆Ç´ÇÃÇ›ÅAÉèÅ[ÉãÉhçsóÒÇçƒåvéZÇµÇƒTLASÇçXêVÇ∑ÇÈÅB
		//if (isMoveLight) {

			//PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, constBufferData.pointLight.lightPos);
			//PorygonInstanceRegister::Ins()->ChangeScale(sphereIns, constBufferData.pointLight.lightSize);
			//PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns2, constBufferData.pointLight.lightPos);
			//PorygonInstanceRegister::Ins()->ChangeScale(sphereIns2, constBufferData.pointLight.lightSize);

		tlas.Update();

		//}

		/*----- ï`âÊèàóù -----*/

		// âÊñ Ç…ï\é¶Ç≥ÇÍÇÈÉåÉìÉ_Å[É^Å[ÉQÉbÉgÇ…ñﬂÇ∑ÅB
		//DirectXBase::Ins()->SetRenderTarget();

		RaytracingPipline setPipline = {};

		// ÉfÉoÉbÉOópÇÃÉpÉCÉvÉâÉCÉìIDÇ…âûÇ∂ÇΩÉpÉCÉvÉâÉCÉìÇÉZÉbÉgÇ∑ÇÈÅB
		if (debugPiplineID == DEF_PIPLINE) {

			constBufferData.counter = 0;
			setPipline = defPipline;

		}
		else if (debugPiplineID == AO_PIPLINE) {

			setPipline = aoPipline;

		}
		else if (debugPiplineID == DENOISE_AO_PIPLINE) {

			setPipline = deAOPipline;

		}

		auto frameIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();
		constBufferData.mtxView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
		constBufferData.mtxViewInv = XMMatrixInverse(nullptr, constBufferData.mtxView);
		// íËêîÉoÉbÉtÉ@ÇÃíÜêgÇçXêVÇ∑ÇÈÅB
		constBuff.Write(frameIndex, &constBufferData, sizeof(KariConstBufferData));
		auto sceneConstantBuffer = constBuff.GetBuffer(frameIndex);

		// ÉOÉçÅ[ÉoÉãÉãÅ[ÉgÉVÉOÉlÉ`ÉÉÇ≈égÇ§Ç∆êÈåæÇµÇƒÇ¢ÇÈÉäÉ\Å[ÉXÇÁÇÉZÉbÉgÅB
		ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
		DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		DirectXBase::Ins()->cmdList->SetComputeRootSignature(setPipline.GetGlobalRootSig()->GetRootSig().Get());
		DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas.GetDescriptorHeapIndex()));
		raytracingOutput.SetComputeRootDescriptorTalbe(2);
		raytracingOutputData.SetComputeRootDescriptorTalbe(3);
		DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, sceneConstantBuffer->GetGPUVirtualAddress());


		// ÉåÉCÉgÉåÅ[ÉVÉìÉOåãâ ÉoÉbÉtÉ@ÇUAVèÛë‘Ç÷
		raytracingOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		raytracingOutputData.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		DirectXBase::Ins()->cmdList->SetPipelineState1(setPipline.GetStateObject().Get());

		DirectXBase::Ins()->cmdList->DispatchRays(&setPipline.GetDispatchRayDesc());

		// ÉfÉoÉbÉOópÇÃÉpÉCÉvÉâÉCÉìÇ™ÉfÉmÉCÉYópÉpÉCÉvÉâÉCÉìÇæÇ¡ÇΩÇÁÅAÉRÉìÉsÉÖÅ[ÉgÉVÉFÅ[É_Å[ÇégÇ¡ÇƒÉfÉmÉCÉYÇÇ©ÇØÇÈÅB
		if (debugPiplineID == DENOISE_AO_PIPLINE) {



		}

		// ÉoÉbÉNÉoÉbÉtÉ@ÇÃÉCÉìÉfÉbÉNÉXÇéÊìæÇ∑ÇÈÅB
		UINT backBufferIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();
=======
	// „Ç´„É°„É©„ÇíÂàùÊúüÂåñ„ÄÇ
	Camera::Ins()->Init();

	// ÈñãÁô∫Áî®
	DevDXR dev;
	dev.Init();
>>>>>>> AtmosphericScattering

	/*----------„Ç≤„Éº„É†„É´„Éº„Éó----------*/
	while (true) {

		
		dev.Update();
		dev.Draw();
		

	}

	return 0;
<<<<<<< HEAD
}


// É^ÉCÉgÉãÉoÅ[ÇÃFPSÇÃçXêV
void FPS()
{
	static DWORD prev_time = timeGetTime();	// ëOâÒÇÃéûä‘
	static int frame_count = 0;		// ÉtÉåÅ[ÉÄÉJÉEÉìÉg
	DWORD now_time = timeGetTime();		// ç°âÒÇÃÉtÉåÅ[ÉÄÇÃéûä‘

	frame_count++;	// ÉtÉåÅ[ÉÄêîÇÉJÉEÉìÉgÇ∑ÇÈ

	// åoâﬂéûä‘Ç™ÇPïbÇí¥Ç¶ÇΩÇÁÉJÉEÉìÉgÇ∆éûä‘ÇÉäÉZÉbÉg
	if (now_time - prev_time >= 1000)
	{
		wchar_t fps[1000];
		_itow_s(frame_count, fps, 10);
		wchar_t moji[] = L"FPS";
		wcscat_s(fps, moji);
		SetWindowText(DirectXBase::Ins()->windowsAPI.hwnd, fps);
		//OutputDebugString(fps);

		prev_time = now_time;
		frame_count = 0;
	}
}

void Input(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& debugPiplineID) {

	bool isMove = false;

	float speed = 5.0f;
	float rot = 0.03f;
	if (Input::isKey(DIK_W)) {
		Camera::Ins()->Move(speed);
		isMove = true;
	}
	if (Input::isKey(DIK_S)) {
		Camera::Ins()->Move(-speed);
		isMove = true;
	}
	if (Input::isKey(DIK_A)) {
		Camera::Ins()->MoveRight(speed);
		isMove = true;
	}
	if (Input::isKey(DIK_D)) {
		Camera::Ins()->MoveRight(-speed);
		isMove = true;
	}
	if (Input::isKey(DIK_UP)) {
		Camera::Ins()->forwardVec.y += rot;
		isMove = true;
	}
	if (Input::isKey(DIK_DOWN)) {
		Camera::Ins()->forwardVec.y -= rot;
		isMove = true;
	}
	if (Input::isKey(DIK_LEFT)) {
		Camera::Ins()->AddRotationXZ(rot);
		isMove = true;
	}
	if (Input::isKey(DIK_RIGHT)) {
		Camera::Ins()->AddRotationXZ(-rot);
		isMove = true;
	}
	if (Input::isKey(DIK_LSHIFT)) {
		Camera::Ins()->eye.y -= 10.0f;
		isMove = true;
	}
	if (Input::isKey(DIK_SPACE)) {
		Camera::Ins()->eye.y += 10.0f;
		isMove = true;
	}

	// DirLightÇ…Ç¬Ç¢Çƒ

	// äKëwç\ë¢Ç…Ç∑ÇÈÅB
	if (ImGui::TreeNode("Lighting")) {

		// ílÇï€ë∂Ç∑ÇÈÅB
		float dirX = constBufferData.pointLight.lightPos.x;
		float dirY = constBufferData.pointLight.lightPos.y;
		float dirZ = constBufferData.pointLight.lightPos.z;
		float lightSize = constBufferData.pointLight.lightSize;
		float aoSampleCount = constBufferData.aoSampleCount;
		float pointLightPower = constBufferData.pointLight.lightPower;
		float MOVE_LENGTH = 1500.0f;
		ImGui::SliderFloat("PointLightX", &constBufferData.pointLight.lightPos.x, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("PointLightY", &constBufferData.pointLight.lightPos.y, 0.0f, 1000.0f);
		ImGui::SliderFloat("PointLightZ", &constBufferData.pointLight.lightPos.z, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("PointLightRadius", &constBufferData.pointLight.lightSize, 1.0f, 50.0f);
		ImGui::SliderFloat("PointLightPower", &constBufferData.pointLight.lightPower, 300.0f, 1000.0f);
		ImGui::SliderFloat("AOSampleCount", &aoSampleCount, 1.0f, 30.0f);
		constBufferData.aoSampleCount = aoSampleCount;

		// ïœÇÌÇ¡ÇƒÇ¢ÇΩÇÁ
		if (dirX != constBufferData.pointLight.lightPos.x || dirY != constBufferData.pointLight.lightPos.y || dirZ != constBufferData.pointLight.lightPos.z || lightSize != constBufferData.pointLight.lightSize || pointLightPower != constBufferData.pointLight.lightPower) {

			isMove = true;
			isMoveLight = true;

		}

		// ÉâÉCÉgÇÃêFÇê›íËÅB
		array<float, 3> lightColor = { constBufferData.pointLight.lightColor.x,constBufferData.pointLight.lightColor.y,constBufferData.pointLight.lightColor.z };
		ImGui::ColorPicker3("LightColor", lightColor.data());
		// êFÇ™ïœÇÌÇ¡ÇƒÇ¢ÇΩÇÁÅB
		if (lightColor[0] != constBufferData.pointLight.lightColor.x || lightColor[1] != constBufferData.pointLight.lightColor.y || lightColor[2] != constBufferData.pointLight.lightColor.z) {
			isMove = true;
		}
		constBufferData.pointLight.lightColor.x = lightColor[0];
		constBufferData.pointLight.lightColor.y = lightColor[1];
		constBufferData.pointLight.lightColor.z = lightColor[2];

		ImGui::TreePop();

	}

	if (isMove) {
		constBufferData.counter = 0;
	}
	else {
		++constBufferData.counter;
	}

	// ÉÅÉbÉVÉÖÇï\é¶Ç∑ÇÈÅB
	bool isMesh = constBufferData.isMeshScene;
	bool prevIsMesh = isMesh;
	ImGui::Checkbox("Mesh Scene", &isMesh);
	constBufferData.isMeshScene = isMesh;
	// ílÇ™èëÇ´ä∑Ç¶ÇÁÇÍÇƒÇ¢ÇΩÇÁÅAÉTÉìÉvÉäÉìÉOÇèâä˙âªÇ∑ÇÈÅB
	if (isMesh != prevIsMesh) {
		constBufferData.counter = 0;
	}

	// ñ@ê¸Çï\é¶Ç∑ÇÈÅB
	bool isNormal = constBufferData.isNormalScene;
	bool prevIsNormal = isNormal;
	ImGui::Checkbox("Normal Scene", &isNormal);
	constBufferData.isNormalScene = isNormal;
	// ílÇ™èëÇ´ä∑Ç¶ÇÁÇÍÇƒÇ¢ÇΩÇÁÅAÉTÉìÉvÉäÉìÉOÇèâä˙âªÇ∑ÇÈÅB
	if (isNormal != prevIsNormal) {
		constBufferData.counter = 0;
	}

	// ÉâÉCÉgÇ™Ç†ÇΩÇ¡ÇΩñ ÇæÇØï\é¶Ç∑ÇÈÉtÉâÉOÇçXêVÅB
	bool isLightHit = constBufferData.isLightHitScene;
	bool prevIsLightHit = isLightHit;
	ImGui::Checkbox("LightHit Scene", &isLightHit);
	constBufferData.isLightHitScene = isLightHit;
	// ílÇ™èëÇ´ä∑Ç¶ÇÁÇÍÇƒÇ¢ÇΩÇÁÅAÉTÉìÉvÉäÉìÉOÇèâä˙âªÇ∑ÇÈÅB
	if (isLightHit != prevIsLightHit) {
		constBufferData.counter = 0;
	}

	// ÉpÉCÉvÉâÉCÉìÇëIëÅB
	int debugPiplineBuff = debugPiplineID;
	ImGui::RadioButton("DEF PIPLINE", &debugPiplineBuff, 0);
	ImGui::SameLine();
	ImGui::RadioButton("AO PIPLINE", &debugPiplineBuff, 1);
	ImGui::SameLine();
	ImGui::RadioButton("DENOISE AO PIPLINE", &debugPiplineBuff, 2);
	debugPiplineID = (DEGU_PIPLINE_ID)debugPiplineBuff;

	// AOÇÃÉpÉCÉvÉâÉCÉìÇëIëÇ≥ÇÍÇƒÇ¢ÇΩÇ∆Ç´ÇÃÇ›ÅAÉmÉCÉYÇèoÇ∑Ç©ÇÃÉtÉâÉOÇï\é¶Ç∑ÇÈÅB
	if (debugPiplineID == AO_PIPLINE) {

		// ÉfÉoÉbÉOópÇ≈ÉmÉCÉYâÊñ ÇèoÇ∑ÇΩÇﬂÇÃÉtÉâÉOÇÉZÉbÉgÅB
		bool isNoise = constBufferData.isNoiseScene;
		ImGui::Checkbox("Noise Scene", &isNoise);
		constBufferData.isNoiseScene = isNoise;

		// ÉfÉoÉbÉOópÇ≈ÉmÉCÉYâÊñ ÇÃÇ›ÇèoÇ∑ÇΩÇﬂÇÃÉtÉâÉOÇÉZÉbÉgÅB
		bool isNoiseOnly = constBufferData.isNoiseOnlyScene;
		ImGui::Checkbox("NoiseOnly Scene", &isNoiseOnly);
		// ÉtÉâÉOÇ™èëÇ´ä∑ÇÌÇ¡ÇƒÇ¢ÇΩÇÁÉfÉmÉCÉYÉJÉEÉìÉ^Å[Çèâä˙âªÇ∑ÇÈÅB
		if (isNoiseOnly != constBufferData.isNoiseOnlyScene) {
			constBufferData.counter = 0;
		}
		constBufferData.isNoiseOnlyScene = isNoiseOnly;

		// ÉAÉìÉrÉGÉìÉgÉIÉNÉäÉÖÅ[ÉWÉáÉìÇçsÇ§Ç©ÇÃÉtÉâÉOÇÉZÉbÉgÅB
		bool isNoAO = constBufferData.isNoAO;
		ImGui::Checkbox("NoAO Scene", &isNoAO);
		// ÉtÉâÉOÇ™èëÇ´ä∑ÇÌÇ¡ÇƒÇ¢ÇΩÇÁÉfÉmÉCÉYÉJÉEÉìÉ^Å[Çèâä˙âªÇ∑ÇÈÅB
		if (isNoAO != constBufferData.isNoAO) {
			constBufferData.counter = 0;
		}
		constBufferData.isNoAO = isNoAO;

	}

=======
>>>>>>> AtmosphericScattering
}