
#include "ResourceManager.h"

DEFINITION_SINGLE(CResourceManager)

CResourceManager::CResourceManager()	:
	m_MeshManager(nullptr),
	m_ShaderManager(nullptr),
	m_MaterialManager(nullptr),
	m_TextureManager(nullptr),
	m_AnimationManager(nullptr),
	m_SoundManager(nullptr),
	m_FontManager(nullptr),
	m_ParticleManager(nullptr),
	m_AnimationManager3D(nullptr)
{
}

CResourceManager::~CResourceManager()
{
	SAFE_DELETE(m_ParticleManager);
	SAFE_DELETE(m_FontManager);
	SAFE_DELETE(m_SoundManager);
	SAFE_DELETE(m_AnimationManager);
	SAFE_DELETE(m_MaterialManager);
	SAFE_DELETE(m_ShaderManager);
	SAFE_DELETE(m_MeshManager);
	SAFE_DELETE(m_TextureManager);
	SAFE_DELETE(m_AnimationManager3D);
}

bool CResourceManager::Init()
{
	m_MeshManager = new CMeshManager;

	if (!m_MeshManager->Init())
		return false;

	m_ShaderManager = new CShaderManager;

	if (!m_ShaderManager->Init())
		return false;

	m_TextureManager = new CTextureManager;

	if (!m_TextureManager->Init())
		return false;

	m_MaterialManager = new CMaterialManager;

	if (!m_MaterialManager->Init())
		return false;

	m_AnimationManager = new CAnimationManager;

	if (!m_AnimationManager->Init())
		return false;

	m_SoundManager = new CSoundManager;

	if (!m_SoundManager->Init())
		return false;

	m_FontManager = new CFontManager;

	if (!m_FontManager->Init())
		return false;

	m_ParticleManager = new CParticleManager;

	if (!m_ParticleManager->Init())
		return false;

	m_AnimationManager3D = new CAnimationManager3D;

	if (!m_AnimationManager3D->Init())
		return false;

	return true;
}

void CResourceManager::Update()
{
	m_SoundManager->Update();
}

void CResourceManager::RenderTarget()
{
	CMesh* Mesh = m_MeshManager->FindMesh("WidgetMesh");
	CShader* Shader = m_ShaderManager->FindShader("RenderTargetShader");

	m_TextureManager->RenderTarget(Mesh, Shader);
}

bool CResourceManager::CreateMesh(Mesh_Type Type, 
	const std::string& Name, void* VtxData, int Size, 
	int Count, D3D11_USAGE Usage,
	D3D11_PRIMITIVE_TOPOLOGY Primitive, void* IdxData,
	int IdxSize, int IdxCount, D3D11_USAGE IdxUsage,
	DXGI_FORMAT Fmt, CScene* Scene)
{
	return m_MeshManager->CreateMesh(Type, Name, VtxData, Size, Count,
		Usage, Primitive, IdxData, IdxSize, IdxCount, IdxUsage,
		Fmt, Scene);
}

bool CResourceManager::LoadMesh(Mesh_Type Type, const std::string& Name,
	const TCHAR* FileName, const std::string& PathName, class CScene* Scene)
{
	return m_MeshManager->LoadMesh(Type, Name, FileName, PathName, Scene);
}

bool CResourceManager::LoadMeshFullPath(Mesh_Type Type, const std::string& Name,
	const TCHAR* FullPath, class CScene* Scene)
{
	return m_MeshManager->LoadMeshFullPath(Type, Name, FullPath, Scene);
}

bool CResourceManager::LoadMeshMultibyte(Mesh_Type Type, const std::string& Name,
	const char* FileName, const std::string& PathName, class CScene* Scene)
{
	return m_MeshManager->LoadMeshMultibyte(Type, Name, FileName, PathName, Scene);
}

bool CResourceManager::LoadMeshFullPathMultibyte(
	Mesh_Type Type, const std::string& Name, const char* FullPath, class CScene* Scene)
{
	return m_MeshManager->LoadMeshFullPathMultibyte(Type, Name, FullPath, Scene);
}

bool CResourceManager::SetMeshSkeleton(const std::string& Name, 
	const std::string& SkeletonName)
{
	CSkeleton* Skeleton = FindSkeleton(SkeletonName);

	if (!Skeleton)
		return false;

	return SetMeshSkeleton(Name, Skeleton);
}

bool CResourceManager::SetMeshSkeleton(const std::string& Name, CSkeleton* Skeleton)
{
	return m_MeshManager->SetMeshSkeleton(Name, Skeleton);
}

CMesh* CResourceManager::FindMesh(const std::string& Name)
{
	return m_MeshManager->FindMesh(Name);
}

void CResourceManager::ReleaseMesh(const std::string& Name)
{
	m_MeshManager->ReleaseMesh(Name);
}

CShader* CResourceManager::FindShader(const std::string& Name)
{
	return m_ShaderManager->FindShader(Name);
}

void CResourceManager::ReleaseShader(const std::string& Name)
{
	m_ShaderManager->ReleaseShader(Name);
}

bool CResourceManager::CreateConstantBuffer(const std::string& Name, int Size, int Register,
	int ConstantBufferShaderType)
{
	return m_ShaderManager->CreateConstantBuffer(Name, Size, Register, ConstantBufferShaderType);
}

CConstantBuffer* CResourceManager::FindConstantBuffer(const std::string& Name)
{
	return m_ShaderManager->FindConstantBuffer(Name);
}

CMaterial* CResourceManager::FindMaterial(const std::string& Name)
{
	return m_MaterialManager->FindMaterial(Name);
}

CMaterialConstantBuffer* CResourceManager::GetMaterialConstantBuffer() const
{
	return m_MaterialManager->GetMaterialConstantBuffer();
}

void CResourceManager::ReleaseMaterial(const std::string& Name)
{
	m_MaterialManager->ReleaseMaterial(Name);
}

bool CResourceManager::LoadTexture(const std::string& Name, const TCHAR* FileName, const std::string& PathName)
{
	return m_TextureManager->LoadTexture(Name, FileName, PathName);
}

bool CResourceManager::LoadTextureFullPath(const std::string& Name, const TCHAR* FullPath)
{
	return m_TextureManager->LoadTextureFullPath(Name, FullPath);
}

bool CResourceManager::LoadTexture(const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName)
{
	return m_TextureManager->LoadTexture(Name, vecFileName, PathName);
}

bool CResourceManager::LoadTextureFullPath(const std::string& Name, const std::vector<TCHAR*>& vecFullPath)
{
	return m_TextureManager->LoadTextureFullPath(Name, vecFullPath);
}

bool CResourceManager::CreateTarget(const std::string& Name, unsigned int Width, 
	unsigned int Height, DXGI_FORMAT PixelFormat,
	DXGI_FORMAT DepthFormat)
{
	return m_TextureManager->CreateTarget(Name, Width, Height, PixelFormat, DepthFormat);
}

bool CResourceManager::LoadTextureArray(const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName)
{
	return m_TextureManager->LoadTextureArray(Name, vecFileName, PathName);
}

bool CResourceManager::LoadTextureArrayFullPath(const std::string& Name, const std::vector<TCHAR*>& vecFullPath)
{
	return m_TextureManager->LoadTextureArrayFullPath(Name, vecFullPath);
}

CTexture* CResourceManager::FindTexture(const std::string& Name)
{
	return m_TextureManager->FindTexture(Name);
}

void CResourceManager::ReleaseTexture(const std::string& Name)
{
	m_TextureManager->ReleaseTexture(Name);
}

bool CResourceManager::CreateAnimationSequence2D(const std::string& Name,
	const std::string& TextureName, const TCHAR* FileName, const std::string& PathName)
{
	return m_AnimationManager->CreateAnimationSequence2D(Name, TextureName, FileName, PathName);
}

bool CResourceManager::CreateAnimationSequence2D(const std::string& Name, CTexture* Texture)
{
	return m_AnimationManager->CreateAnimationSequence2D(Name, Texture);
}

void CResourceManager::AddAnimationSequence2DFrame(const std::string& Name, const Vector2& Start,
	const Vector2& Size)
{
	m_AnimationManager->AddFrame(Name, Start, Size);
}

void CResourceManager::AddAnimationSequence2DFrame(const std::string& Name, float StartX,
	float StartY, float Width, float Height)
{
	m_AnimationManager->AddFrame(Name, StartX, StartY, Width, Height);
}

bool CResourceManager::SaveSequence2DFullPath(const std::string& Name, const char* FullPath)
{
	return m_AnimationManager->SaveSequenceFullPath(Name, FullPath);
}

bool CResourceManager::LoadSequence2DFullPath(std::string& resultName, const char* FullPath, class CScene* Scene)
{
	return m_AnimationManager->LoadSequenceFullPath(resultName, FullPath, Scene);
}

bool CResourceManager::LoadSequence2DFullPath(const char* FullPath, CScene* Scene)
{
	std::string	Name;
	return m_AnimationManager->LoadSequenceFullPath(Name, FullPath, Scene);
}

bool CResourceManager::SaveSequence2D(const std::string& Name, const char* FileName, const std::string& PathName)
{
	return m_AnimationManager->SaveSequence(Name, FileName, PathName);
}

bool CResourceManager::LoadSequence2D(const char* FileName, const std::string& PathName, CScene* Scene)
{
	std::string	Name;
	return m_AnimationManager->LoadSequence(Name, FileName, PathName, Scene);
}

bool CResourceManager::LoadSequence2D(std::string& resultName, const char* FileName, const std::string& PathName, CScene* Scene)
{
	return m_AnimationManager->LoadSequence(resultName, FileName, PathName, Scene);
}

CAnimationSequence2D* CResourceManager::FindAnimationSequence2D(const std::string& Name)
{
	return m_AnimationManager->FindSequence(Name);
}

void CResourceManager::ReleaseAnimationSequence2D(const std::string& Name)
{
	m_AnimationManager->ReleaseSequence(Name);
}

CAnimation2DConstantBuffer* CResourceManager::GetAnimation2DCBuffer() const
{
	return m_AnimationManager->GetAnimation2DCBuffer();
}

bool CResourceManager::LoadSound(const std::string& ChannelGroupName, bool Loop, const std::string& Name,
	const char* FileName, const std::string& PathName)
{
	return m_SoundManager->LoadSound(ChannelGroupName, Loop, Name, FileName, PathName);
}

bool CResourceManager::CreateSoundChannelGroup(const std::string& Name)
{
	return m_SoundManager->CreateSoundChannelGroup(Name);
}

bool CResourceManager::SetVolume(int Volume)
{
	return m_SoundManager->SetVolume(Volume);
}

bool CResourceManager::SetVolume(const std::string& ChannelGroupName, int Volume)
{
	return m_SoundManager->SetVolume(ChannelGroupName, Volume);
}

bool CResourceManager::SoundPlay(const std::string& Name)
{
	return m_SoundManager->SoundPlay(Name);
}

bool CResourceManager::SoundStop(const std::string& Name)
{
	return m_SoundManager->SoundStop(Name);
}

bool CResourceManager::SoundPause(const std::string& Name)
{
	return m_SoundManager->SoundPause(Name);
}

bool CResourceManager::SoundResume(const std::string& Name)
{
	return m_SoundManager->SoundResume(Name);
}

CSound* CResourceManager::FindSound(const std::string& Name)
{
	return m_SoundManager->FindSound(Name);
}

FMOD::ChannelGroup* CResourceManager::FindChannelGroup(const std::string& Name)
{
	return m_SoundManager->FindChannelGroup(Name);
}

void CResourceManager::ReleaseSound(const std::string& Name)
{
	m_SoundManager->ReleaseSound(Name);
}

bool CResourceManager::CreateFontFile(const std::string& Name, const TCHAR* FileName, 
	const std::string& PathName)
{
	return m_FontManager->CreateFontFile(Name, FileName, PathName);
}

bool CResourceManager::LoadFont(const std::string& Name, const TCHAR* FontName, 
	int Weight, float FontSize, const TCHAR* LocalName, int Stretch)
{
	return m_FontManager->LoadFont(Name, FontName, Weight, FontSize, LocalName, Stretch);
}

const TCHAR* CResourceManager::GetFontFaceName(const std::string& Name)
{
	return m_FontManager->GetFontFaceName(Name);
}

const char* CResourceManager::GetFontFaceNameMultibyte(const std::string& Name)
{
	return m_FontManager->GetFontFaceNameMultibyte(Name);
}

bool CResourceManager::CreateFontColor(float r, float g, float b, float a)
{
	return m_FontManager->CreateFontColor(r, g, b, a);
}

bool CResourceManager::CreateFontColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return m_FontManager->CreateFontColor(r, g, b, a);
}

bool CResourceManager::CreateFontColor(const Vector4& Color)
{
	return m_FontManager->CreateFontColor(Color);
}

bool CResourceManager::CreateFontColor(unsigned int Color)
{
	return m_FontManager->CreateFontColor(Color);
}

ID2D1SolidColorBrush* CResourceManager::FindFontColor(float r, float g, float b, float a)
{
	return m_FontManager->FindFontColor(r, g, b, a);
}

ID2D1SolidColorBrush* CResourceManager::FindFontColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return m_FontManager->FindFontColor(r, g, b, a);
}

ID2D1SolidColorBrush* CResourceManager::FindFontColor(const Vector4& Color)
{
	return m_FontManager->FindFontColor(Color);
}

ID2D1SolidColorBrush* CResourceManager::FindFontColor(unsigned int Color)
{
	return m_FontManager->FindFontColor(Color);
}

unsigned int CResourceManager::CreateFontColorKey(float r, float g, float b, float a)
{
	return m_FontManager->CreateFontColorKey(r, g, b, a);
}

unsigned int CResourceManager::CreateFontColorKey(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return m_FontManager->CreateFontColorKey(r, g, b, a);
}

unsigned int CResourceManager::CreateFontColorKey(const Vector4& Color)
{
	return m_FontManager->CreateFontColorKey(Color);
}

IDWriteTextLayout* CResourceManager::CreateTextLayout(const TCHAR* Text, IDWriteTextFormat* Font,
	float Width, float Height)
{
	return m_FontManager->CreateTextLayout(Text, Font, Width, Height);
}

IDWriteTextLayout* CResourceManager::CreateTextLayout(const TCHAR* Text, const std::string& FontName, 
	float Width, float Height)
{
	return m_FontManager->CreateTextLayout(Text, FontName, Width, Height);
}

IDWriteFontCollection1* CResourceManager::FindFontFile(const std::string& Name)
{
	return m_FontManager->FindFontFile(Name);
}

IDWriteTextFormat* CResourceManager::FindFont(const std::string& Name)
{
	return m_FontManager->FindFont(Name);
}

bool CResourceManager::CreateParticle(const std::string& Name)
{
	return m_ParticleManager->CreateParticle(Name);
}

CParticle* CResourceManager::FindParticle(const std::string& Name)
{
	return m_ParticleManager->FindParticle(Name);
}

void CResourceManager::ReleaseParticle(const std::string& Name)
{
	m_ParticleManager->ReleaseParticle(Name);
}

bool CResourceManager::LoadAnimationSequence(const std::string& Name, bool Loop,
	_tagFbxAnimationClip* Clip, class CScene* Scene)
{
	return m_AnimationManager3D->LoadAnimationSequence(Name, Loop, Clip, Scene);
}

bool CResourceManager::LoadAnimationSequence(const std::string& Name, bool Loop, int StartFrame,
	int EndFrame, float PlayTime, const std::vector<BoneKeyFrame*>& vecFrame, class CScene* Scene)
{
	return m_AnimationManager3D->LoadAnimationSequence(Name, Loop, StartFrame, EndFrame, 
		PlayTime, vecFrame, Scene);
}

bool CResourceManager::LoadAnimationSequence(bool Loop, const std::string& Name, const TCHAR* FileName,
	const std::string& PathName, class CScene* Scene)
{
	return m_AnimationManager3D->LoadAnimationSequence(Loop, Name, FileName, PathName, Scene);
}

bool CResourceManager::LoadAnimationSequenceFullPath(bool Loop, const std::string& Name,
	const TCHAR* FullPath, class CScene* Scene)
{
	return m_AnimationManager3D->LoadAnimationSequenceFullPath(Loop, Name, FullPath, Scene);
}

bool CResourceManager::LoadAnimationSequenceMultibyte(bool Loop, const std::string& Name,
	const char* FileName, const std::string& PathName, class CScene* Scene)
{
	return m_AnimationManager3D->LoadAnimationSequenceMultibyte(Loop, Name, FileName, PathName, Scene);
}

bool CResourceManager::LoadAnimationSequenceFullPathMultibyte(bool Loop, const std::string& Name,
	const char* FullPath, class CScene* Scene)
{
	return m_AnimationManager3D->LoadAnimationSequenceFullPathMultibyte(Loop, Name, FullPath, Scene);
}

CAnimationSequence* CResourceManager::FindAnimationSequence(const std::string& Name)
{
	return m_AnimationManager3D->FindAnimationSequence(Name);
}

void CResourceManager::ReleaseAnimationSequence3D(const std::string& Name)
{
	m_AnimationManager3D->ReleaseSequence(Name);
}

bool CResourceManager::LoadSkeleton(const std::string& Name, 
	const TCHAR* FileName, const std::string& PathName, CScene* Scene)
{
	return m_AnimationManager3D->LoadSkeleton(Name, FileName, PathName, Scene);
}

bool CResourceManager::LoadSkeletonFullPath(const std::string& Name,
	const TCHAR* FullPath, CScene* Scene)
{
	return m_AnimationManager3D->LoadSkeletonFullPath(Name, FullPath, Scene);
}

bool CResourceManager::LoadSkeletonMultibyte(const std::string& Name, const char* FileName, const std::string& PathName, CScene* Scene)
{
	return m_AnimationManager3D->LoadSkeletonMultibyte(Name, FileName, PathName, Scene);
}

bool CResourceManager::LoadSkeletonFullPathMultibyte(const std::string& Name, const char* FullPath, CScene* Scene)
{
	return m_AnimationManager3D->LoadSkeletonFullPathMultibyte(Name, FullPath, Scene);
}

bool CResourceManager::AddSocket(const std::string& SkeletonName, 
	const std::string& BoneName, const std::string& SocketName, 
	const Vector3& Offset, const Vector3& OffsetRot)
{
	return m_AnimationManager3D->AddSocket(SkeletonName, BoneName, SocketName,
		Offset, OffsetRot);
}

CSkeleton* CResourceManager::FindSkeleton(const std::string& Name)
{
	return m_AnimationManager3D->FindSkeleton(Name);
}

void CResourceManager::ReleaseSkeleton(const std::string& Name)
{
	m_AnimationManager3D->ReleaseSkeleton(Name);
}
