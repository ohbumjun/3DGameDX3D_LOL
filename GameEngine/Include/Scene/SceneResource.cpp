
#include "SceneResource.h"

CSceneResource::CSceneResource()
{
}

CSceneResource::~CSceneResource()
{
	{
		auto	iter = m_mapMesh.begin();
		auto	iterEnd = m_mapMesh.end();

		for (; iter != iterEnd;)
		{
			std::string	Name = iter->first;

			iter = m_mapMesh.erase(iter);

			CResourceManager::GetInst()->ReleaseMesh(Name);
		}
	}

	{
		auto	iter = m_mapShader.begin();
		auto	iterEnd = m_mapShader.end();

		for (; iter != iterEnd;)
		{
			std::string	Name = iter->first;

			iter = m_mapShader.erase(iter);

			CResourceManager::GetInst()->ReleaseShader(Name);
		}
	}

	{
		auto	iter = m_mapTexture.begin();
		auto	iterEnd = m_mapTexture.end();

		for (; iter != iterEnd;)
		{
			std::string	Name = iter->first;

			iter = m_mapTexture.erase(iter);

			CResourceManager::GetInst()->ReleaseTexture(Name);
		}
	}

	{
		auto	iter = m_mapMaterial.begin();
		auto	iterEnd = m_mapMaterial.end();

		for (; iter != iterEnd;)
		{
			std::string	Name = iter->first;

			iter = m_mapMaterial.erase(iter);

			CResourceManager::GetInst()->ReleaseMaterial(Name);
		}
	}

	{
		auto	iter = m_mapSequence2D.begin();
		auto	iterEnd = m_mapSequence2D.end();

		for (; iter != iterEnd;)
		{
			std::string	Name = iter->first;

			iter = m_mapSequence2D.erase(iter);

			CResourceManager::GetInst()->ReleaseAnimationSequence2D(Name);
		}
	}

	{
		auto	iter = m_mapSequence.begin();
		auto	iterEnd = m_mapSequence.end();

		for (; iter != iterEnd;)
		{
			std::string	Name = iter->first;

			iter = m_mapSequence.erase(iter);

			CResourceManager::GetInst()->ReleaseAnimationSequence3D(Name);
		}
	}

	{
		auto	iter = m_mapParticle.begin();
		auto	iterEnd = m_mapParticle.end();

		for (; iter != iterEnd;)
		{
			std::string	Name = iter->first;

			iter = m_mapParticle.erase(iter);

			CResourceManager::GetInst()->ReleaseParticle(Name);
		}
	}
}

bool CSceneResource::CreateMesh(Mesh_Type Type, 
	const std::string& Name, void* VtxData, int Size, 
	int Count, D3D11_USAGE Usage,
	D3D11_PRIMITIVE_TOPOLOGY Primitive, void* IdxData,
	int IdxSize, int IdxCount, D3D11_USAGE IdxUsage, 
	DXGI_FORMAT Fmt)
{
	if (FindMesh(Name))
		return true;

	if (!CResourceManager::GetInst()->CreateMesh(Type, Name, VtxData, Size, Count,
		Usage, Primitive, IdxData, IdxSize, IdxCount, IdxUsage,
		Fmt, m_Scene))
		return false;

	m_mapMesh.insert(std::make_pair(Name, CResourceManager::GetInst()->FindMesh(Name)));

	return true;
}

bool CSceneResource::LoadMesh(Mesh_Type Type, const std::string& Name,
	const TCHAR* FileName, const std::string& PathName)
{
	if (FindMesh(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadMesh(Type, Name, FileName, PathName, m_Scene))
		return false;

	m_mapMesh.insert(std::make_pair(Name, CResourceManager::GetInst()->FindMesh(Name)));

	return true;
}

bool CSceneResource::LoadMeshFullPath(Mesh_Type Type, const std::string& Name,
	const TCHAR* FullPath)
{
	if (FindMesh(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadMeshFullPath(Type, Name, FullPath, m_Scene))
		return false;

	m_mapMesh.insert(std::make_pair(Name, CResourceManager::GetInst()->FindMesh(Name)));

	return true;
}

bool CSceneResource::LoadMeshMultibyte(Mesh_Type Type, const std::string& Name, const char* FileName, const std::string& PathName)
{
	if (FindMesh(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadMeshMultibyte(Type, Name, FileName, PathName, m_Scene))
		return false;

	m_mapMesh.insert(std::make_pair(Name, CResourceManager::GetInst()->FindMesh(Name)));

	return true;
}

bool CSceneResource::LoadMeshFullPathMultibyte(Mesh_Type Type, const std::string& Name, const char* FullPath)
{
	if (FindMesh(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadMeshFullPathMultibyte(Type, Name, FullPath, m_Scene))
		return false;

	m_mapMesh.insert(std::make_pair(Name, CResourceManager::GetInst()->FindMesh(Name)));

	return true;
}

bool CSceneResource::SetMeshSkeleton(const std::string& Name, const std::string& SkeletonName)
{
	return CResourceManager::GetInst()->SetMeshSkeleton(Name, SkeletonName);
}

bool CSceneResource::SetMeshSkeleton(const std::string& Name, CSkeleton* Skeleton)
{
	return CResourceManager::GetInst()->SetMeshSkeleton(Name, Skeleton);
}

CMesh* CSceneResource::FindMesh(const std::string& Name)
{
	auto	iter = m_mapMesh.find(Name);

	if (iter == m_mapMesh.end())
	{
		CMesh* Mesh = CResourceManager::GetInst()->FindMesh(Name);

		if (!Mesh)
			return nullptr;

		m_mapMesh.insert(std::make_pair(Name, Mesh));

		return Mesh;
	}

	return iter->second;
}

CShader* CSceneResource::FindShader(const std::string& Name)
{
	auto	iter = m_mapShader.find(Name);

	if (iter == m_mapShader.end())
	{
		CShader* Shader = CResourceManager::GetInst()->FindShader(Name);

		if (!Shader)
			return nullptr;

		m_mapShader.insert(std::make_pair(Name, Shader));

		return Shader;
	}

	return iter->second;
}

CMaterial* CSceneResource::FindMaterial(const std::string& Name)
{
	auto	iter = m_mapMaterial.find(Name);

	if (iter == m_mapMaterial.end())
	{
		CMaterial* Material = CResourceManager::GetInst()->FindMaterial(Name);

		if (!Material)
			return nullptr;

		m_mapMaterial.insert(std::make_pair(Name, Material));

		return Material;
	}

	return iter->second;
}

bool CSceneResource::LoadTexture(const std::string& Name, const TCHAR* FileName, const std::string& PathName)
{
	if (FindTexture(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadTexture(Name, FileName, PathName))
		return false;

	m_mapTexture.insert(std::make_pair(Name, CResourceManager::GetInst()->FindTexture(Name)));

	return true;
}

bool CSceneResource::LoadTextureFullPath(const std::string& Name, const TCHAR* FullPath)
{
	if (FindTexture(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadTextureFullPath(Name, FullPath))
		return false;

	m_mapTexture.insert(std::make_pair(Name, CResourceManager::GetInst()->FindTexture(Name)));

	return true;
}

bool CSceneResource::LoadTexture(const std::string& Name, const std::vector<TCHAR*>& vecFileName,
	const std::string& PathName)
{
	if (FindTexture(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadTexture(Name, vecFileName, PathName))
		return false;

	m_mapTexture.insert(std::make_pair(Name, CResourceManager::GetInst()->FindTexture(Name)));

	return true;
}

bool CSceneResource::LoadTextureFullPath(const std::string& Name, 
	const std::vector<TCHAR*>& vecFullPath)
{
	if (FindTexture(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadTextureFullPath(Name, vecFullPath))
		return false;

	m_mapTexture.insert(std::make_pair(Name, CResourceManager::GetInst()->FindTexture(Name)));

	return true;
}

bool CSceneResource::CreateTarget(const std::string& Name, 
	unsigned int Width, unsigned int Height, 
	DXGI_FORMAT PixelFormat,
	DXGI_FORMAT DepthFormat)
{
	if (FindTexture(Name))
		return true;

	if (!CResourceManager::GetInst()->CreateTarget(Name, Width, Height, PixelFormat, DepthFormat))
		return false;

	m_mapTexture.insert(std::make_pair(Name, CResourceManager::GetInst()->FindTexture(Name)));

	return true;
}

bool CSceneResource::LoadTextureArray(const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName)
{
	if (FindTexture(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadTextureArray(Name, vecFileName, PathName))
		return false;

	m_mapTexture.insert(std::make_pair(Name, CResourceManager::GetInst()->FindTexture(Name)));

	return true;
}

bool CSceneResource::LoadTextureArrayFullPath(const std::string& Name, const std::vector<TCHAR*>& vecFullPath)
{
	if (FindTexture(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadTextureArrayFullPath(Name, vecFullPath))
		return false;

	m_mapTexture.insert(std::make_pair(Name, CResourceManager::GetInst()->FindTexture(Name)));

	return true;
}

CTexture* CSceneResource::FindTexture(const std::string& Name)
{
	auto	iter = m_mapTexture.find(Name);

	if (iter == m_mapTexture.end())
	{
		CTexture* Texture = CResourceManager::GetInst()->FindTexture(Name);

		if (!Texture)
			return nullptr;

		m_mapTexture.insert(std::make_pair(Name, Texture));

		return Texture;
	}

	return iter->second;
}

bool CSceneResource::CreateAnimationSequence2D(const std::string& Name, 
	const std::string& TextureName, const TCHAR* FileName, const std::string& PathName)
{
	if (FindAnimationSequence2D(Name))
		return true;

	if (!CResourceManager::GetInst()->CreateAnimationSequence2D(Name, TextureName, FileName, PathName))
		return false;

	m_mapSequence2D.insert(std::make_pair(Name, CResourceManager::GetInst()->FindAnimationSequence2D(Name)));

	return true;
}

bool CSceneResource::CreateAnimationSequence2D(const std::string& Name, 
	CTexture* Texture)
{
	if (FindAnimationSequence2D(Name))
		return true;

	if (!CResourceManager::GetInst()->CreateAnimationSequence2D(Name, Texture))
		return false;

	m_mapSequence2D.insert(std::make_pair(Name, CResourceManager::GetInst()->FindAnimationSequence2D(Name)));

	return true;
}

void CSceneResource::AddAnimationSequence2DFrame(const std::string& Name, const Vector2& Start,
	const Vector2& Size)
{
	CAnimationSequence2D* Anim = FindAnimationSequence2D(Name);

	if (!Anim)
		return;

	Anim->AddFrame(Start, Size);
}

void CSceneResource::AddAnimationSequence2DFrame(const std::string& Name, float StartX,
	float StartY, float Width, float Height)
{
	CAnimationSequence2D* Anim = FindAnimationSequence2D(Name);

	if (!Anim)
		return;

	Anim->AddFrame(StartX, StartY, Width, Height);
}

bool CSceneResource::SaveSequence2DFullPath(const std::string& Name, const char* FullPath)
{
	return CResourceManager::GetInst()->SaveSequence2DFullPath(Name, FullPath);
}

bool CSceneResource::LoadSequence2DFullPath(const char* FullPath)
{
	std::string	SequenceName;

	if (!CResourceManager::GetInst()->LoadSequence2DFullPath(SequenceName, FullPath, m_Scene))
		return false;

	if (FindAnimationSequence2D(SequenceName))
		return true;

	CAnimationSequence2D* Sequence = CResourceManager::GetInst()->FindAnimationSequence2D(SequenceName);

	m_mapSequence2D.insert(std::make_pair(SequenceName, Sequence));

	return true;
}

bool CSceneResource::LoadSequence2DFullPath(std::string& resultName, const char* FullPath)
{
	if (!CResourceManager::GetInst()->LoadSequence2DFullPath(resultName, FullPath, m_Scene))
		return false;

	if (FindAnimationSequence2D(resultName))
		return true;

	CAnimationSequence2D* Sequence = CResourceManager::GetInst()->FindAnimationSequence2D(resultName);

	m_mapSequence2D.insert(std::make_pair(resultName, Sequence));

	return true;
}

bool CSceneResource::SaveSequence2D(const std::string& Name, const char* FileName, 
	const std::string& PathName)
{
	return CResourceManager::GetInst()->SaveSequence2D(Name, FileName, PathName);
}

bool CSceneResource::LoadSequence2D(const char* FileName, const std::string& PathName)
{
	std::string	SequenceName;

	if (!CResourceManager::GetInst()->LoadSequence2D(SequenceName, FileName, PathName, m_Scene))
		return false;

	if (FindAnimationSequence2D(SequenceName))
		return true;

	CAnimationSequence2D* Sequence = CResourceManager::GetInst()->FindAnimationSequence2D(SequenceName);

	m_mapSequence2D.insert(std::make_pair(SequenceName, Sequence));

	return true;
}

bool CSceneResource::LoadSequence2D(std::string& resultName, const char* FileName, 
	const std::string& PathName)
{
	if (!CResourceManager::GetInst()->LoadSequence2D(resultName, FileName, PathName, m_Scene))
		return false;

	if (FindAnimationSequence2D(resultName))
		return true;

	CAnimationSequence2D* Sequence = CResourceManager::GetInst()->FindAnimationSequence2D(resultName);

	m_mapSequence2D.insert(std::make_pair(resultName, Sequence));

	return true;
}

CAnimationSequence2D* CSceneResource::FindAnimationSequence2D(const std::string& Name)
{
	auto	iter = m_mapSequence2D.find(Name);

	if (iter == m_mapSequence2D.end())
	{
		CAnimationSequence2D* Anim = CResourceManager::GetInst()->FindAnimationSequence2D(Name);

		if (!Anim)
			return nullptr;

		m_mapSequence2D.insert(std::make_pair(Name, Anim));

		return Anim;
	}

	return iter->second;
}

CAnimation2DConstantBuffer* CSceneResource::GetAnimation2DCBuffer() const
{
	return CResourceManager::GetInst()->GetAnimation2DCBuffer();
}

bool CSceneResource::LoadSound(const std::string& ChannelGroupName, bool Loop, const std::string& Name, 
	const char* FileName, const std::string& PathName)
{
	if (FindSound(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadSound(ChannelGroupName, Loop, Name, FileName, PathName))
		return false;

	m_mapSound.insert(std::make_pair(Name, CResourceManager::GetInst()->FindSound(Name)));

	return true;
}

bool CSceneResource::CreateSoundChannelGroup(const std::string& Name)
{
	return CResourceManager::GetInst()->CreateSoundChannelGroup(Name);
}

bool CSceneResource::SetVolume(int Volume)
{
	return CResourceManager::GetInst()->SetVolume(Volume);
}

bool CSceneResource::SetVolume(const std::string& ChannelGroupName, int Volume)
{
	return CResourceManager::GetInst()->SetVolume(ChannelGroupName, Volume);
}

bool CSceneResource::SoundPlay(const std::string& Name)
{
	return CResourceManager::GetInst()->SoundPlay(Name);
}

bool CSceneResource::SoundStop(const std::string& Name)
{
	return CResourceManager::GetInst()->SoundStop(Name);
}

bool CSceneResource::SoundPause(const std::string& Name)
{
	return CResourceManager::GetInst()->SoundPause(Name);
}

bool CSceneResource::SoundResume(const std::string& Name)
{
	return CResourceManager::GetInst()->SoundResume(Name);
}

CSound* CSceneResource::FindSound(const std::string& Name)
{
	auto	iter = m_mapSound.find(Name);

	if (iter == m_mapSound.end())
	{
		CSound* Sound = CResourceManager::GetInst()->FindSound(Name);

		if (!Sound)
			return nullptr;

		m_mapSound.insert(std::make_pair(Name, Sound));

		return Sound;
	}

	return iter->second;
}

FMOD::ChannelGroup* CSceneResource::FindChannelGroup(const std::string& Name)
{
	return CResourceManager::GetInst()->FindChannelGroup(Name);
}

bool CSceneResource::CreateParticle(const std::string& Name)
{
	if (FindParticle(Name))
		return true;

	if (!CResourceManager::GetInst()->CreateParticle(Name))
		return false;

	m_mapParticle.insert(std::make_pair(Name, CResourceManager::GetInst()->FindParticle(Name)));

	return true;
}

CParticle* CSceneResource::FindParticle(const std::string& Name)
{
	auto	iter = m_mapParticle.find(Name);

	if (iter == m_mapParticle.end())
	{
		CParticle* Particle = CResourceManager::GetInst()->FindParticle(Name);

		if (!Particle)
			return nullptr;

		m_mapParticle.insert(std::make_pair(Name, Particle));

		return Particle;
	}

	return iter->second;
}

bool CSceneResource::LoadAnimationSequence(const std::string& Name, bool Loop, 
	_tagFbxAnimationClip* Clip)
{
	if (FindAnimationSequence(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadAnimationSequence(Name, Loop, Clip, m_Scene))
		return false;

	m_mapSequence.insert(std::make_pair(Name, CResourceManager::GetInst()->FindAnimationSequence(Name)));

	return true;
}

bool CSceneResource::LoadAnimationSequence(const std::string& Name, bool Loop,
	int StartFrame, int EndFrame, float PlayTime, const std::vector<BoneKeyFrame*>& vecFrame)
{
	if (FindAnimationSequence(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadAnimationSequence(Name, Loop, StartFrame, EndFrame,
		PlayTime, vecFrame, m_Scene))
		return false;

	m_mapSequence.insert(std::make_pair(Name, CResourceManager::GetInst()->FindAnimationSequence(Name)));

	return true;
}

bool CSceneResource::LoadAnimationSequence(bool Loop, const std::string& Name, const TCHAR* FileName,
	const std::string& PathName)
{
	if (FindAnimationSequence(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadAnimationSequence(Loop, Name, FileName, PathName, m_Scene))
		return false;

	m_mapSequence.insert(std::make_pair(Name, CResourceManager::GetInst()->FindAnimationSequence(Name)));

	return true;
}

bool CSceneResource::LoadAnimationSequenceFullPath(bool Loop, const std::string& Name, const TCHAR* FullPath)
{
	if (FindAnimationSequence(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadAnimationSequenceFullPath(Loop, Name, FullPath, m_Scene))
		return false;

	m_mapSequence.insert(std::make_pair(Name, CResourceManager::GetInst()->FindAnimationSequence(Name)));

	return true;
}

bool CSceneResource::LoadAnimationSequenceMultibyte(bool Loop, const std::string& Name, const char* FileName, const std::string& PathName)
{
	if (FindAnimationSequence(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadAnimationSequenceMultibyte(Loop, Name, FileName, PathName, m_Scene))
		return false;

	m_mapSequence.insert(std::make_pair(Name, CResourceManager::GetInst()->FindAnimationSequence(Name)));

	return true;
}

bool CSceneResource::LoadAnimationSequenceFullPathMultibyte(bool Loop, const std::string& Name,
	const char* FullPath)
{
	if (FindAnimationSequence(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadAnimationSequenceFullPathMultibyte(Loop, Name, FullPath, m_Scene))
		return false;

	m_mapSequence.insert(std::make_pair(Name, CResourceManager::GetInst()->FindAnimationSequence(Name)));

	return true;
}

CAnimationSequence* CSceneResource::FindAnimationSequence(const std::string& Name)
{
	auto	iter = m_mapSequence.find(Name);

	if (iter == m_mapSequence.end())
	{
		CAnimationSequence* Sequence = CResourceManager::GetInst()->FindAnimationSequence(Name);

		if (!Sequence)
			return nullptr;

		m_mapSequence.insert(std::make_pair(Name, Sequence));

		return Sequence;
	}

	return iter->second;
}

bool CSceneResource::LoadSkeleton(const std::string& Name,
	const TCHAR* FileName, const std::string& PathName)
{
	if (FindSkeleton(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadSkeleton(Name, FileName, PathName, m_Scene))
		return false;

	m_mapSkeleton.insert(std::make_pair(Name, CResourceManager::GetInst()->FindSkeleton(Name)));

	return true;
}

bool CSceneResource::LoadSkeletonFullPath(const std::string& Name, 
	const TCHAR* FullPath)
{
	if (FindSkeleton(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadSkeletonFullPath(Name, FullPath, m_Scene))
		return false;

	m_mapSkeleton.insert(std::make_pair(Name, CResourceManager::GetInst()->FindSkeleton(Name)));

	return true;
}

bool CSceneResource::LoadSkeletonMultibyte(const std::string& Name, const char* FileName, const std::string& PathName)
{
	if (FindSkeleton(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadSkeletonMultibyte(Name, FileName, PathName, m_Scene))
		return false;

	m_mapSkeleton.insert(std::make_pair(Name, CResourceManager::GetInst()->FindSkeleton(Name)));

	return true;
}

bool CSceneResource::LoadSkeletonFullPathMultibyte(const std::string& Name, const char* FullPath)
{
	if (FindSkeleton(Name))
		return true;

	if (!CResourceManager::GetInst()->LoadSkeletonFullPathMultibyte(Name, FullPath, m_Scene))
		return false;

	m_mapSkeleton.insert(std::make_pair(Name, CResourceManager::GetInst()->FindSkeleton(Name)));

	return true;
}

bool CSceneResource::AddSocket(const std::string& SkeletonName, 
	const std::string& BoneName, const std::string& SocketName,
	const Vector3& Offset, const Vector3& OffsetRot)
{
	return CResourceManager::GetInst()->AddSocket(SkeletonName, BoneName, SocketName,
		Offset, OffsetRot);
}

CSkeleton* CSceneResource::FindSkeleton(const std::string& Name)
{
	auto	iter = m_mapSkeleton.find(Name);

	if (iter == m_mapSkeleton.end())
	{
		CSkeleton* Skeleton = CResourceManager::GetInst()->FindSkeleton(Name);

		if (!Skeleton)
			return nullptr;

		m_mapSkeleton.insert(std::make_pair(Name, Skeleton));

		return Skeleton;
	}

	return iter->second;
}
