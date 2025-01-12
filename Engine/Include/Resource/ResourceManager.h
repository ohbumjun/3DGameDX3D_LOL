#pragma once

#include "Mesh/MeshManager.h"
#include "Shader/ShaderManager.h"
#include "Material/MaterialManager.h"
#include "Texture/TextureManager.h"
#include "Animation/AnimationManager.h"
#include "Animation/AnimationManager3D.h"
#include "Sound/SoundManager.h"
#include "Font/FontManager.h"
#include "Particle/ParticleManager.h"

class CResourceManager
{
private:
	CMeshManager* m_MeshManager;
	CShaderManager* m_ShaderManager;
	CMaterialManager* m_MaterialManager;
	CTextureManager* m_TextureManager;
	CAnimationManager* m_AnimationManager;
	CAnimationManager3D* m_AnimationManager3D;
	CSoundManager* m_SoundManager;
	CFontManager* m_FontManager;
	CParticleManager* m_ParticleManager;

public:
	bool Init();
	void Update();
	void RenderTarget();

public:	// =================== Mesh =====================
	bool CreateMesh(Mesh_Type Type, const std::string& Name,
		void* VtxData, int Size, int Count, D3D11_USAGE Usage,
		D3D11_PRIMITIVE_TOPOLOGY Primitive, void* IdxData = nullptr, int IdxSize = 0, int IdxCount = 0,
		D3D11_USAGE IdxUsage = D3D11_USAGE_DEFAULT, DXGI_FORMAT Fmt = DXGI_FORMAT_UNKNOWN,
		class CScene* Scene = nullptr);
	bool LoadMesh(Mesh_Type Type, const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = MESH_PATH, class CScene* Scene = nullptr);
	bool LoadMeshFullPath(Mesh_Type Type, const std::string& Name,
		const TCHAR* FullPath, class CScene* Scene = nullptr);
	bool LoadMeshMultibyte(Mesh_Type Type, const std::string& Name, const char* FileName,
		const std::string& PathName = MESH_PATH, class CScene* Scene = nullptr);
	bool LoadMeshFullPathMultibyte(Mesh_Type Type, const std::string& Name,
		const char* FullPath, class CScene* Scene = nullptr);
	bool SetMeshSkeleton(const std::string& Name, const std::string& SkeletonName);
	bool SetMeshSkeleton(const std::string& Name, class CSkeleton* Skeleton);

	class CMesh* FindMesh(const std::string& Name);

	void ReleaseMesh(const std::string& Name);

	
public:	// =================== Shader =====================
	template <typename T>
	bool CreateShader(const std::string& Name)
	{
		return m_ShaderManager->CreateShader<T>(Name);
	}
	class CShader* FindShader(const std::string& Name);

	void ReleaseShader(const std::string& Name);

	bool CreateConstantBuffer(const std::string& Name, int Size, int Register,
		int ConstantBufferShaderType = (int)Buffer_Shader_Type::All);

	class CConstantBuffer* FindConstantBuffer(const std::string& Name);


public:	// =================== Material =====================
	CMaterial* FindMaterial(const std::string& Name);
	CMaterialConstantBuffer* GetMaterialConstantBuffer()	const;

	void ReleaseMaterial(const std::string& Name);

public:
	template <typename T>
	bool CreateMaterial(const std::string& Name)
	{
		return m_MaterialManager->CreateMaterial<T>(Name);
	}

	template <typename T>
	T* CreateMaterialEmpty()
	{
		return m_MaterialManager->CreateMaterialEmpty<T>();
	}

public:	// =================== Texture =====================
	bool LoadTexture(const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = TEXTURE_PATH);
	bool LoadTextureFullPath(const std::string& Name, const TCHAR* FullPath);
	bool LoadTexture(const std::string& Name, const std::vector<TCHAR*>& vecFileName,
		const std::string& PathName = TEXTURE_PATH);
	bool LoadTextureFullPath(const std::string& Name, const std::vector<TCHAR*>& vecFullPath);
	bool CreateTarget(const std::string& Name, unsigned int Width,
		unsigned int Height, DXGI_FORMAT PixelFormat,
		DXGI_FORMAT DepthFormat = DXGI_FORMAT_UNKNOWN);
	bool LoadTextureArray(const std::string& Name, const std::vector<TCHAR*>& vecFileName,
		const std::string& PathName = TEXTURE_PATH);
	bool LoadTextureArrayFullPath(const std::string& Name, const std::vector<TCHAR*>& vecFullPath);
	class CTexture* FindTexture(const std::string& Name);
	void ReleaseTexture(const std::string& Name);

public:	// =================== Sequence2D =====================
	bool CreateAnimationSequence2D(const std::string& Name, const std::string& TextureName,
		const TCHAR* FileName, const std::string& PathName = TEXTURE_PATH);
	bool CreateAnimationSequence2D(const std::string& Name, class CTexture* Texture);
	void AddAnimationSequence2DFrame(const std::string& Name, const Vector2& Start, const Vector2& Size);
	void AddAnimationSequence2DFrame(const std::string& Name, float StartX, float StartY, float Width, float Height);
	bool SaveSequence2DFullPath(const std::string& Name, const char* FullPath);
	bool LoadSequence2DFullPath(std::string& resultName, const char* FullPath, class CScene* Scene = nullptr);
	bool LoadSequence2DFullPath(const char* FullPath, class CScene* Scene = nullptr);
	bool SaveSequence2D(const std::string& Name, const char* FileName, const std::string& PathName = ANIMATION_PATH);
	bool LoadSequence2D(const char* FileName, const std::string& PathName = ANIMATION_PATH, class CScene* Scene = nullptr);
	bool LoadSequence2D(std::string& resultName, const char* FileName, const std::string& PathName = ANIMATION_PATH, class CScene* Scene = nullptr);
	CAnimationSequence2D* FindAnimationSequence2D(const std::string& Name);
	void ReleaseAnimationSequence2D(const std::string& Name);
	class CAnimation2DConstantBuffer* GetAnimation2DCBuffer()	const;

public:	// =================== Sound =====================
	bool LoadSound(const std::string& ChannelGroupName, bool Loop, const std::string& Name, const char* FileName,
		const std::string& PathName = SOUND_PATH);
	bool CreateSoundChannelGroup(const std::string& Name);
	bool SetVolume(int Volume);
	bool SetVolume(const std::string& ChannelGroupName, int Volume);
	bool SoundPlay(const std::string& Name);
	bool SoundStop(const std::string& Name);
	bool SoundPause(const std::string& Name);
	bool SoundResume(const std::string& Name);
	class CSound* FindSound(const std::string& Name);
	FMOD::ChannelGroup* FindChannelGroup(const std::string& Name);
	void ReleaseSound(const std::string& Name);


public:	// =================== Font =====================
	bool CreateFontFile(const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = FONT_PATH);
	bool LoadFont(const std::string& Name, const TCHAR* FontName, int Weight, float FontSize,
		const TCHAR* LocalName, int Stretch = DWRITE_FONT_STRETCH_NORMAL);
	const TCHAR* GetFontFaceName(const std::string& Name);
	const char* GetFontFaceNameMultibyte(const std::string& Name);
	bool CreateFontColor(float r, float g, float b, float a);
	bool CreateFontColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	bool CreateFontColor(const Vector4& Color);
	bool CreateFontColor(unsigned int Color);
	ID2D1SolidColorBrush* FindFontColor(float r, float g, float b, float a);
	ID2D1SolidColorBrush* FindFontColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	ID2D1SolidColorBrush* FindFontColor(const Vector4& Color);
	ID2D1SolidColorBrush* FindFontColor(unsigned int Color);
	unsigned int CreateFontColorKey(float r, float g, float b, float a);
	unsigned int CreateFontColorKey(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	unsigned int CreateFontColorKey(const Vector4& Color);
	IDWriteTextLayout* CreateTextLayout(const TCHAR* Text, IDWriteTextFormat* Font, float Width,
		float Height);
	IDWriteTextLayout* CreateTextLayout(const TCHAR* Text, const std::string& FontName, float Width,
		float Height);

	IDWriteFontCollection1* FindFontFile(const std::string& Name);
	IDWriteTextFormat* FindFont(const std::string& Name);


public:	// =================== Particle =====================
	bool CreateParticle(const std::string& Name);
	CParticle* FindParticle(const std::string& Name);
	void ReleaseParticle(const std::string& Name);

public:
	bool LoadAnimationSequence(const std::string& Name, bool Loop,
		struct _tagFbxAnimationClip* Clip, class CScene* Scene = nullptr);
	bool LoadAnimationSequence(const std::string& Name, bool Loop,
		int StartFrame, int EndFrame, float PlayTime,
		const std::vector<BoneKeyFrame*>& vecFrame, class CScene* Scene = nullptr);
	bool LoadAnimationSequence(bool Loop, const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = ANIMATION_PATH, class CScene* Scene = nullptr);
	bool LoadAnimationSequenceFullPath(bool Loop, const std::string& Name, const TCHAR* FullPath, class CScene* Scene = nullptr);
	bool LoadAnimationSequenceMultibyte(bool Loop, const std::string& Name, const char* FileName,
		const std::string& PathName = ANIMATION_PATH, class CScene* Scene = nullptr);
	bool LoadAnimationSequenceFullPathMultibyte(bool Loop, const std::string& Name, const char* FullPath, class CScene* Scene = nullptr);
	CAnimationSequence* FindAnimationSequence(const std::string& Name);
	void ReleaseAnimationSequence3D(const std::string& Name);

	bool LoadSkeleton(const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = ANIMATION_PATH,
		class CScene* Scene = nullptr);
	bool LoadSkeletonFullPath(const std::string& Name, const TCHAR* FullPath,
		class CScene* Scene = nullptr);
	bool LoadSkeletonMultibyte(const std::string& Name, const char* FileName,
		const std::string& PathName = ANIMATION_PATH,
		class CScene* Scene = nullptr);
	bool LoadSkeletonFullPathMultibyte(const std::string& Name, const char* FullPath,
		class CScene* Scene = nullptr);
	bool AddSocket(const std::string& SkeletonName, const std::string& BoneName, const std::string& SocketName,
		const Vector3& Offset = Vector3::Zero, const Vector3& OffsetRot = Vector3::Zero);
	CSkeleton* FindSkeleton(const std::string& Name);
	void ReleaseSkeleton(const std::string& Name);

	DECLARE_SINGLE(CResourceManager)
};
