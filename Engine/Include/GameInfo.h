#pragma once

#define	DIRECTINPUT_VERSION	0x0800

#include <Windows.h>
#include <list>
#include <vector>
#include <unordered_map>
#include <crtdbg.h>
#include <functional>
#include <string>
#include <math.h>
#include <algorithm>

#include <dinput.h>
#include <dwrite.h>
#include <dwrite_3.h>
#include <d2d1.h>
#include <stack>
#include <process.h>
#include <optional>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include "SharedPtr.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Resource/Texture/DirectXTex.h"
#include "fmod.hpp"

#ifdef _DEBUG

#pragma comment(lib, "DirectXTex_Debug.lib")

#else

#pragma comment(lib, "DirectXTex.lib")

#endif // _DEBUG

#pragma comment(lib, "fmod64_vc.lib")

#define	SHADOWMAP_WIDTH		2560.f
#define	SHADOWMAP_HEIGHT	1440.f

#define	ROOT_PATH		"Root"
#define	SHADER_PATH		"Shader"
#define	TEXTURE_PATH	"Texture"
#define	FONT_PATH		"Font"
#define	ANIMATION_PATH	"Animation"
#define	SCENE_PATH		"Scene"
#define	SOUND_PATH		"Sound"
#define	MESH_PATH		"Mesh"
#define	NAVIGATION_PATH	"Navigation"

#define	SAFE_DELETE(p)	if(p)	{ delete p; p = nullptr; }
#define	SAFE_DELETE_ARRAY(p)	if(p)	{ delete[] p; p = nullptr; }
#define	SAFE_RELEASE(p)	if(p)	{ p->Release(); p = nullptr; }

#define	SAFE_DELETE_ARRAY_VECLIST(p)	\
for(auto iter = p.begin(); iter != p.end(); ++iter)\
{\
	SAFE_DELETE_ARRAY(*iter);\
}

#define	DECLARE_SINGLE(Type)	\
private:\
	static Type*	m_Inst;\
public:\
	static Type* GetInst()\
	{\
		if(!m_Inst)\
			m_Inst = new Type;\
		return m_Inst;\
	}\
	static void DestroyInst()\
	{\
		SAFE_DELETE(m_Inst);\
	}\
private:\
	Type();\
	~Type();

#define	DEFINITION_SINGLE(Type)	Type* Type::m_Inst = nullptr;

struct Resolution
{
	unsigned int	Width;
	unsigned int	Height;
};

struct VertexColor
{
	Vector3	Pos;
	Vector4	Color;

	VertexColor()
	{
	}

	VertexColor(const Vector3& _Pos, const Vector4& _Color)	:
		Pos(_Pos),
		Color(_Color)
	{
	}
};


struct VertexBuffer
{
	ID3D11Buffer* Buffer;
	int		Size;
	int		Count;
	void* Data;

	VertexBuffer() :
		Buffer(nullptr),
		Size(0),
		Count(0),
		Data(nullptr)
	{
	}

	~VertexBuffer()
	{
		SAFE_DELETE_ARRAY(Data);
		SAFE_RELEASE(Buffer);
	}
};

struct IndexBuffer
{
	ID3D11Buffer* Buffer;
	int		Size;
	int		Count;
	DXGI_FORMAT	Fmt;
	void* Data;

	IndexBuffer() :
		Buffer(nullptr),
		Size(0),
		Count(0),
		Fmt(DXGI_FORMAT_UNKNOWN),
		Data(nullptr)
	{
	}

	~IndexBuffer()
	{
		SAFE_DELETE_ARRAY(Data);
		SAFE_RELEASE(Buffer);
	}
};



struct MeshContainer
{
	VertexBuffer	VB;
	std::vector<IndexBuffer>	vecIB;
	D3D11_PRIMITIVE_TOPOLOGY	Primitive;
};

struct MeshSlot
{
	VertexBuffer* VB;
	IndexBuffer* IB;
	D3D11_PRIMITIVE_TOPOLOGY	Primitive;

	MeshSlot() :
		VB(nullptr),
		IB(nullptr)
	{
	}
};


struct TransformCBuffer
{
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProj;
	Matrix	matInvProj;
	Matrix	matWV;
	Matrix	matWVP;
	Matrix	matVP;
	Matrix	matInvVP;
	Matrix	matInvWVP;
	Vector3	Pivot;
	float	Empty1;
	Vector3	MeshSize;
	float	Empty;
};

struct VertexUV
{
	Vector3	Pos;
	Vector2 UV;

	VertexUV()
	{
	}

	VertexUV(const Vector3& _Pos, const Vector2& _UV) :
		Pos(_Pos),
		UV(_UV)
	{
	}
};


struct VertexUV3D
{
	Vector3	Pos;
	Vector3 UV;
	VertexUV3D()
	{
	}

	VertexUV3D(const Vector3& _Pos, const Vector3& _UV) :
		Pos(_Pos),
		UV(_UV)
	{
	}
};

struct MaterialCBuffer
{
	Vector4	BaseColor;
	Vector4	AmbientColor;
	Vector4	SpecularColor;
	Vector4	EmissiveColor;
	float	Opacity;
	int		PaperBurnEnable;
	int		BumpEnable;
	int		Animation3DEnable;
	int		SpecularTex;
	int		EmissiveTex;
	int		ReceiveDecal;
	float	Empty;
};

struct AnimationFrameData
{
	Vector2	Start;
	Vector2	Size;
};

struct Animation2DCBuffer
{
	Vector2	Animation2DStartUV;
	Vector2	Animation2DEndUV;
	int		Animation2DType;
	Vector3	Animation2DEmpty;
};

struct Standard2DCBuffer
{
	int		AnimationEnable;
	Vector3	Empty;
};

struct FindComponentName
{
	std::string	Name;
	std::string	ParentName;
};

struct CollisionProfile
{
	std::string			Name;
	Collision_Channel	Channel;
	bool				CollisionEnable;

	std::vector<Collision_Interaction>	vecInteraction;
};

struct CollisionResult
{
	class CColliderComponent* Src;
	class CColliderComponent* Dest;
	Vector3	HitPoint;

	CollisionResult()	:
		Src(nullptr),
		Dest(nullptr)
	{
	}
};

struct Box2DInfo
{
	Vector2	Center;
	Vector2	Axis[2];
	Vector2	Length;
	Vector2	Min;
	Vector2	Max;
};

struct Box3DInfo
{
	Vector3	Center;
	Vector3	Axis[3];
	Vector3	Length;
	Vector3	Min;
	Vector3	Max;
};

struct CircleInfo
{
	Vector2	Center;
	float	Radius;
	Vector2	Min;
	Vector2	Max;
};

struct PixelInfo
{
	unsigned char* Pixel;
	unsigned int	Width;
	unsigned int	Height;
	PixelCollision_Type	Type;
	unsigned char	Color[4];
	Box2DInfo		Box;
	Vector2	Min;
	Vector2	Max;
	ID3D11ShaderResourceView* SRV;
	int		RefCount;

	PixelInfo()	:
		RefCount(1),
		Pixel(nullptr),
		SRV(nullptr),
		Width(0),
		Height(0),
		Box{},
		Color{},
		Type(PixelCollision_Type::Color_Confirm)
	{
	}
};

struct ColliderCBuffer
{
	Vector4	Color;
	Matrix	matWVP;
};

struct WidgetCBuffer
{
	Vector4	Tint;
	Matrix	matWP;
	int		UseTexture;
	int		WidgetAnimType;
	Vector2	WidgetAnimStartUV;
	Vector2	WidgetAnimEndUV;
	int		WidgetAnimEnable;
	float	Opacity;
};

struct ProgressBarCBuffer
{
	float	Percent;
	int		Dir;
	Vector2	Empty;
};

struct WaterCBuffer
{
	float WaveHeight;
	float WaveSpeed;
	float WaveFrequencey;
	float Empty;
};

struct FirstHDRDownScaleCBuffer
{
	Vector2 FRes; // 백버퍼의 높이와 너비를 4로 나눈 값
	unsigned int FDomain; // 백퍼퍼와 높이와 너비를 곱한 후 16으로 나눈 값
	unsigned int FGroupSize; // 백버퍼의 높이와 너비를 곱한 후, 16으로 나눈 다음 1024를 곱한 값
	
	float FAdaptation; // 적응 값
	float Empty;
	Vector2 Empty2;
};

struct ToneMappingCBuffer
{
	unsigned int MiddleGrey; 
	unsigned int LumWhiteSqr;
	Vector2 Empty;
};

struct	ParticleCBuffer
{
	unsigned int	SpawnEnable;	// 현재 파티클 생성 여부
	Vector3	StartMin;		// 파티클이 생성될 영역의 Min
	Vector3	StartMax;		// 파티클이 생성될 영역의 Max
	unsigned int	SpawnCountMax;	// 생성될 파티클의 최대
	Vector3	ScaleMin;		// 생성될 파티클 크기의 Min
	float	LifeTimeMin;	// 생성될 파티클이 살아있을 최소시간
	Vector3	ScaleMax;		// 새성될 파티클 크기의 Max
	float	LifeTimeMax;	// 생성될 파티클이 살아있을 최대시간
	Vector4	ColorMin;		// 생성될 파티클의 색상 Min
	Vector4	ColorMax;		// 생성될 파티클의 색상 Max
	float	SpeedMin;		// 파티클의 최소 이동속도
	float	SpeedMax;		// 파티클의 최대 이동속도
	int		Move;			// 이동을 하는지 안하는지
	int		Gravity;		// 중력 적용을 받는지 안받는지
	Vector3	MoveDir;		// 이동을 한다면 기준이 될 이동 방향
	int		Is2D;			// 2D용 파티클인지
	Vector3	MoveAngle;	// 이동을 한다면 기준이 될 방향으로부터 x, y, z 에 저장된 각도만큼 틀어진 랜덤한 방향을 구한다.
	float	Empty;
};

struct ParticleInfo
{
	Vector3	WorldPos;
	Vector3	Dir;
	float	Speed;
	float	LifeTime;
	float	LifeTimeMax;
	int		Alive;
	float	FallTime;
	float	FallStartY;
};

struct ParticleInfoShared
{
	unsigned int	SpawnEnable;
	Vector3	ScaleMin;
	Vector3	ScaleMax;
	Vector4	ColorMin;
	Vector4	ColorMax;
	int		GravityEnable;
};

struct GlobalCBuffer
{
	float	DeltaTime;
	float	AccTime;
	Vector2	Resolution;
	Vector2	NoiseResolution;
	Vector2	Empty;
};

struct PaperBurnCBuffer
{
	Vector4	InLineColor;
	Vector4	OutLineColor;
	Vector4	CenterLineColor;
	float	Filter;
	int		Inverse;
	float	InFilter;
	float	OutFilter;
	float	CenterFilter;
	Vector3	Empty;
};

struct TileMapCBuffer
{
	Vector2 ImageSize;
	Vector2 Size;
	Vector2 Start;
	Vector2 End;
	Matrix  matWVP;
};

struct TileInfo
{
	Matrix matWVP;
	Vector2 TileStart;
	Vector2 TileEnd;
	Vector4 TileColor;
	float Opacity;
	Vector3 Empty;
};

// Paper Burn을 이용하는 물체들은 Instancing에서 제외시켜준다.
// 별도로 출력을 해주는 것이다.
struct InstancingInfo
{
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProj;
	Matrix	matWV;
	Matrix	matWVP;
	Matrix	matVP;
	Vector3	Pivot;
	float	Opacity;
	Vector3	MeshSize;
	int		PaperBurnEnable;
	Vector4	BaseColor;
	Vector2	Animation2DStartUV;
	Vector2	Animation2DEndUV;
	int		AnimEnable;
	Vector3	Empty;
};

struct NavWorkData
{
	std::function<void(const std::list<Vector3>&)>	Callback;
	CSharedPtr<class CSceneComponent>	OwnerComponent;
	//Vector3		Start;
	Vector3		End;
};

struct NavResultData
{
	std::function<void(const std::list<Vector3>&)>	Callback;
	std::list<Vector3>	vecPath;
};

struct Vertex3D
{
	Vector3	Pos;
	Vector3	Normal;
	Vector2	UV;
	Vector3	Tangent;
	Vector3	Binormal;
	Vector4	BlendWeight;
	Vector4	BlendIndex;
};


struct AnimationCBuffer
{
	int	BoneCount;
	int	CurrentFrame;
	int	NextFrame;
	float	Ratio;
	int	FrameCount;
	int	RowIndex;
	int	ChangeAnimation;
	float	ChangeRatio;
	int		ChangeFrameCount;
	Vector3	Empty;
};


struct LightCBuffer
{
	Vector4	Color;
	int	LightType;
	Vector3	Pos;
	Vector3	Dir;	
	float	Distance;
	float	AngleIn;
	float	AngleOut;
	float	Att1;
	float	Att2;
	float	Att3;
	Vector3	Empty;
};

struct LandScapeCBuffer
{
	float	DetailLevel;
	int		SplatCount;
	Vector2	Empty;
};

struct Ray
{
	Vector3	Pos;
	Vector3	Dir;
	float	Dist;
	Vector3	ImpactPoint;
};

struct SphereInfo
{
	Vector3	Center;
	float	Radius;
	Vector3	Min;
	Vector3	Max;
};


struct Instancing3DInfo
{
	Matrix matWV;
	Matrix matWVP;
	Vector4 MtrlBaseColor;
	Vector4 MtrlAmbientColor;
	Vector4 MtrlSpecularColor;
	Vector4 MtrlEmissiveColor;
	float MtrlOpacity;
	int MtrlPaperBurnEnable;
	int MtrlBumpEnable;
	int MtrlAnimation3DEnable;
	int MtrlSpecularTex;
	int MtrlEmissiveTex;
	int MtrlReceiveDecal;
	float PaperBurnFilter;
	int PaperBurnInverse;
	float PaperBurnInFilter;
	float PaperBurnOutFilter;
	float PaperBurnCenterFilter;
	Vector4 PaperBurnInLineColor;
	Vector4 PaperBurnOutLineColor;
	Vector4 PaperBurnCenterLineColor;
};

struct InstancingCBuffer
{
	int InstancingBoneCount;
	Vector3 InstancingEmpty;
};

struct LightFowardCBuffer
{
	int LightCount;
	Vector3 Empty;
};