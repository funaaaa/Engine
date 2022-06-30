#include "ShaderData.h"
#include "FString.h"
#include <d3dcompiler.h>
#include <system_error>
#include <fstream>
#include <iostream>
#include <sstream>

#pragma comment(lib, "dxcompiler.lib")

ShaderData::ShaderData(const std::string& shaderPath, const std::string& entryPoint, const std::string& shaderModel, const bool& isDXC)
{

	/*-- �R���X�g���N�^ --*/

	// ������ۑ��B
	this->shaderPath = shaderPath;
	this->entryPoint = entryPoint;
	this->shaderModel = shaderModel;
	this->shaderBlob = nullptr;

	if (isDXC) {

		//�V�F�[�_�[�����[�h�B
		LoadShaderDXC();

	}
	else {

		//�V�F�[�_�[�����[�h�B
		LoadShader();

	}

}

void ShaderData::LoadShader()
{

	/*-- �V�F�[�_�[�����[�h���鏈�� --*/

	wchar_t shaderPathBuff[128];
	FString::ConvertStringToWchar_t(shaderPath, shaderPathBuff, 128);

	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT result = D3DCompileFromFile(
		shaderPathBuff,										//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//�C���N���[�h�\�ɂ���
		entryPoint.c_str(), shaderModel.c_str(),			//�G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//�f�o�b�O�p�ݒ�
		0,
		&shaderBlob, &errorBlob);

	// �V�F�[�_�[�̃G���[�`�F�b�N
	if (FAILED(result)) {

		// hresult����G���[���b�Z�[�W���擾
		std::string errorMsg = std::system_category().message(result);

		//errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

}

void ShaderData::LoadShaderDXC()
{
	HRESULT hr;
	std::ifstream infile(shaderPath, std::ifstream::binary);
	if (!infile) {
		throw std::runtime_error("failed shader compile.");
	}

	std::wstring fileName = StringToWString(shaderPath);
	std::stringstream strstream;

	strstream << infile.rdbuf();

	std::string shaderCode = strstream.str();
	Microsoft::WRL::ComPtr<IDxcLibrary> library;
	DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
	Microsoft::WRL::ComPtr<IDxcCompiler> compiler;
	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> source;

	library->CreateBlobWithEncodingFromPinned(
		(LPBYTE)shaderCode.c_str(), (UINT32)shaderCode.size(), CP_UTF8, &source);
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

	// �C���N���[�h���g���ꍇ�ɂ͓K�؂ɐݒ肷�邱��.
	library->CreateIncludeHandler(&includeHandler);
	// �R���p�C���I�v�V�����̎w��.
	std::vector<LPCWSTR> arguments;

	arguments.emplace_back(L"/Od");
	arguments.emplace_back(L"/Zi");
	arguments.emplace_back(L"-Qembed_debug");

	//

	const auto target = L"lib_6_4";

	Microsoft::WRL::ComPtr<IDxcOperationResult> dxcResult;
	hr = compiler->Compile(source.Get(), fileName.c_str(),
		L"", target, arguments.data(), UINT(arguments.size()),
		nullptr, 0, includeHandler.Get(), &dxcResult);

	if (FAILED(hr)) {
		throw std::runtime_error("failed shader compile.");
	}

	dxcResult->GetStatus(&hr);
	if (FAILED(hr)) {
		Microsoft::WRL::ComPtr<IDxcBlobEncoding> errBlob;
		dxcResult->GetErrorBuffer(&errBlob);
		// ... errBlob �̓��e���G���[���b�Z�[�W�Ƃ��ĕ\�� (�ȗ�)
		throw std::runtime_error("failed shader compile");
	}

	Microsoft::WRL::ComPtr<IDxcBlob> blob;
	dxcResult->GetResult(&blob);

	shaderBlobDxc = blob;

	std::vector<char> result;
	auto size = blob->GetBufferSize();
	result.resize(size);
	memcpy(result.data(), blob->GetBufferPointer(), size);
	shaderBin = result;

}
