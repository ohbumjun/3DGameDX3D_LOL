#include "Input.h"
#include "Device.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/CameraManager.h"
#include "Component/CameraComponent.h"
#include "Engine.h"

DEFINITION_SINGLE(CInput)

CInput::CInput()	:
	m_hInst(0),
	m_hWnd(0),
	m_Input(nullptr),
	m_Keyboard(nullptr),
	m_Mouse(nullptr),
	m_KeyArray{},
	m_LButtonClick(false),
	m_RButtonClick(false),
	m_CollisionWidget(false)
{
	m_vecKeyState.resize(256);

	for (int i = 0; i < 256; ++i)
	{
		m_vecKeyState[i].Key = (unsigned char)i;
	}
}

CInput::~CInput()
{
	auto	iter = m_mapKeyInfo.begin();
	auto	iterEnd = m_mapKeyInfo.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}

	SAFE_RELEASE(m_Keyboard);
	SAFE_RELEASE(m_Mouse);
	SAFE_RELEASE(m_Input);
}

bool CInput::CreateKey(const std::string& Name, unsigned char Key)
{
	KeyInfo* Info = FindKeyInfo(Name);

	if (Info)
		return false;

	Info = new KeyInfo;

	Info->Name = Name;

	unsigned char	ConvertkeyValue = ConvertKey(Key);

	Info->State.Key = ConvertkeyValue;

	m_mapKeyInfo.insert(std::make_pair(Name, Info));

	bool	Add = false;

	size_t	Size = m_vecAddKey.size();

	for (size_t i = 0; i < Size; ++i)
	{
		if (m_vecAddKey[i] == ConvertkeyValue)
		{
			Add = true;
			break;
		}
	}

	if (!Add)
		m_vecAddKey.push_back(ConvertkeyValue);

	return true;
}

bool CInput::SetCtrlKey(const std::string& Name, bool State)
{
	KeyInfo* Info = FindKeyInfo(Name);

	if (!Info)
		return false;

	Info->Ctrl = State;

	return true;
}

bool CInput::SetAltKey(const std::string& Name, bool State)
{
	KeyInfo* Info = FindKeyInfo(Name);

	if (!Info)
		return false;

	Info->Alt = State;

	return true;
}

bool CInput::SetShiftKey(const std::string& Name, bool State)
{
	KeyInfo* Info = FindKeyInfo(Name);

	if (!Info)
		return false;

	Info->Shift = State;

	return true;
}

KeyInfo* CInput::FindKeyInfo(const std::string& Name)
{
	auto	iter = m_mapKeyInfo.find(Name);

	if (iter == m_mapKeyInfo.end())
		return nullptr;

	return iter->second;
}

bool CInput::InitWindow()
{
	return true;
}

bool CInput::InitDirectInput()
{
	if (FAILED(m_Input->CreateDevice(GUID_SysKeyboard, &m_Keyboard, nullptr)))
		return false;

	if (FAILED(m_Keyboard->SetDataFormat(&c_dfDIKeyboard)))
		return false;

	if (FAILED(m_Input->CreateDevice(GUID_SysMouse, &m_Mouse, nullptr)))
		return false;

	if (FAILED(m_Mouse->SetDataFormat(&c_dfDIMouse)))
		return false;

	return true;
}

bool CInput::Init(HINSTANCE hInst, HWND hWnd)
{
	m_hInst = hInst;
	m_hWnd = hWnd;

	m_InputType = Input_Type::Direct;

	

	HRESULT	result = DirectInput8Create(m_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&m_Input, nullptr);

	if (FAILED(result))
		m_InputType = Input_Type::Window;

	switch (m_InputType)
	{
	case Input_Type::Direct:
		if (!InitDirectInput())
			return false;
		break;
	case Input_Type::Window:
		if (!InitWindow())
			return false;
		break;
	}


	return true;
}

void CInput::Update(float DeltaTime)
{
	if (m_InputType == Input_Type::Direct)
	{
		ReadDirectInputKeyboard();
		ReadDirectInputMouse();
	}

	// 마우스 입력처리를 한다.
	UpdateMouse(DeltaTime);

	// UI : 마우스 충돌
	m_CollisionWidget = CSceneManager::GetInst()->GetScene()->GetCollision()->CollisionWidget(DeltaTime);

	// 키 상태를 업데이트 해준다.
	UpdateKeyState();

	// 키보드 키 입력처리를 한다.
	UpdateKeyInfo(DeltaTime);
}

void CInput::ReadDirectInputKeyboard()
{
	HRESULT	result = m_Keyboard->GetDeviceState(256, m_KeyArray);

	if (FAILED(result))
	{
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
			m_Keyboard->Acquire();
	}
}

void CInput::ReadDirectInputMouse()
{
	HRESULT	result = m_Mouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);

	if (FAILED(result))
	{
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
			m_Mouse->Acquire();
	}
}

void CInput::UpdateMouse(float DeltaTime)
{
	POINT	MouseWindowPos;

	// 전체 화면 기준 마우스 좌표
	GetCursorPos(&MouseWindowPos);

	// 전체 화면 기준인 커서의 좌표를 해당 클라이언트(윈도우) 기준으로 좌표를 변환해주는 함수입니다.
	// 왼쪽 상단 지점 Start Point
	ScreenToClient(m_hWnd, &MouseWindowPos);

	// 2D일때는 월드공간에서의 마우스 좌표를 구한다.
	// 카메라를 얻어온다
	if (CEngine::GetInst()->GetEngineSpace() == Engine_Space::Space2D)
	{
		CCameraComponent* Camera = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();

		Vector2	Ratio = CDevice::GetInst()->GetViewportAspectRatio();

		Vector2	MousePos = Vector2(MouseWindowPos.x * Ratio.x, MouseWindowPos.y * Ratio.y);

		MousePos.y = CDevice::GetInst()->GetResolution().Height - MousePos.y;

		m_MouseMove = MousePos - m_MousePos;

		m_MousePos = MousePos;
		m_MouseWorldPos = m_MousePos;

		m_MouseWorldPos += Camera->GetLeftBottom();
	}

	else
	{
		// 3D 차원에서는 World 상에서 사용할 Mouse 위치와, UI 용 Mouse 위치를 따로 세팅할 것이다.
		// 아래의 과정은, 2D 공간 상의 x,y 마우스 좌표를 , z 가 전방으로 향하게 하는
		// Ray 벡터를 만들어내는 과정이다.
		// 정확하게는 뷰 공간 상의 Ray 벡터 정보를 만들어내는 과정이다.
		// (뷰 공간 상에서 Ray 의 시작점은 원점. 바로 카메라 위치가 된다.)

		// Ray를 구한다.
		CCameraComponent* Camera = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();

		// (해상도 / 현재 작업 영역 크기)
		// ex) 1280, 720 ~ 640, 360 => Vector2(2,2)가 나올 것이다.
		// 즉, 어떤 경로를 통해서든, 실제 작업 영역의 크기가 1280, 720과 달라질 수 있다.
		// ex) 내가 중간에 작게하던, 크게하던
		// 그럼에도 불구하고, 어떤 상황에서던 간에
		// 1280, 720 이라는 해상도를 기준으로 두고, 현재 마우스 위치를 구하고자 하는 것
		// (비율을 그대로 유지하면서)
		Vector2	Ratio = CDevice::GetInst()->GetViewportAspectRatio();

		// 이때 MousePos 의 y 는 위를 0으로 시작하여, 화면 기준 아래로 갈수록 값이 증가하는 형태
		Vector2	MousePos = Vector2(MouseWindowPos.x * Ratio.x, MouseWindowPos.y * Ratio.y);

		m_MouseUIPos = MousePos;

		m_MouseUIPos.y = CDevice::GetInst()->GetResolution().Height - m_MouseUIPos.y;

		m_MouseMove = MousePos - m_MousePos;

		m_MousePos = MousePos;
		m_MouseWorldPos = m_MousePos;

		// Screen Space 에 존재하던 Mouse Pos 를 View 공간으로 변화시켜야 한다.
		// 이를 위해 먼저 -1 ~ 1 사이의 (투영 공간 (NDC)적용 까지 반영해서) 위치로 변환을 해줘야 한다. 
		// 2D Screen Space 상의 Mouse x,y 를 이용하여 변환해야 한다.
		// ex) 1000 : 500 (2:1)
		
		// 먼저 0 에서 1 사이의 값으로 변환
		m_Ray.Dir.x = m_MousePos.x / CDevice::GetInst()->GetResolution().Width;
		m_Ray.Dir.y = m_MousePos.y / CDevice::GetInst()->GetResolution().Height;

		// -1 에서 1 사이의 값으로 변환
		m_Ray.Dir.x = m_Ray.Dir.x * 2.f - 1.f;

		// Y값의 경우, 아래가 -1, 위가 1 형태가 되어야 한다.
		// 그런데 현재 MousePos 를 구하는 과정에서, 위에서 아래로 가는 방향을 고려했고
		// 이로 인해 위가 -1, 아래가 1 인 상황이므로, 뒤집어줘야 한다.
		m_Ray.Dir.y = m_Ray.Dir.y * -2.f + 1.f;

		// 원근 투영 행렬은, 1*1 , 2*2 에만 들어있다.
		// 이를 나눠준다는 것은, 결과적으로 투영행렬의 역행렬을 곱해주는 것과 같다.
		Matrix	matProj = Camera->GetProjMatrix();

		// 결과적으로 뷰 공간 상에서의 x,y Ray 좌표값을 얻어낸 것이다.
		m_Ray.Dir.x /= matProj._11;
		m_Ray.Dir.y /= matProj._22;

		// Ray 의 Z 축 방향은, 전방을 향하고 있을 것이다.
		m_Ray.Dir.z = 1.f;

		// 방향 정보이므로 Normalize
		m_Ray.Dir.Normalize();

		// 원점에서 시작 
		m_Ray.Pos = Vector3::Zero;
	}
}

void CInput::UpdateKeyState()
{
	switch (m_InputType)
	{
	case Input_Type::Direct:
		if (m_KeyArray[DIK_LCONTROL] & 0x80)
			m_Ctrl = true;

		else
			m_Ctrl = false;

		if (m_KeyArray[DIK_LALT] & 0x80)
			m_Alt = true;

		else
			m_Alt = false;

		if (m_KeyArray[DIK_LSHIFT] & 0x80)
			m_Shift = true;

		else
			m_Shift = false;

		if (m_MouseState.rgbButtons[0] & 0x80)
			m_LButtonClick = true;

		else
			m_LButtonClick = false;

		if (m_MouseState.rgbButtons[1] & 0x80)
			m_RButtonClick = true;

		else
			m_RButtonClick = false;
		break;
	case Input_Type::Window:
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			m_Ctrl = true;

		else
			m_Ctrl = false;

		if (GetAsyncKeyState(VK_MENU) & 0x8000)
			m_Alt = true;

		else
			m_Alt = false;

		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			m_Shift = true;

		else
			m_Shift = false;
		break;
	}

	// 등록된 키를 반복하며 해당 키가 눌러졌는지를 판단한다.
	size_t	Size = m_vecAddKey.size();

	for (size_t i = 0; i < Size; ++i)
	{
		unsigned char Key = m_vecAddKey[i];

		bool	KeyPush = false;

		switch (m_InputType)
		{
		case Input_Type::Direct:
			switch (Key)
			{
			case DIK_MOUSELBUTTON:
				if (m_MouseState.rgbButtons[0] & 0x80 && !m_CollisionWidget)
				{
					m_LButtonClick = true;
					KeyPush = true;
				}
				break;
			case DIK_MOUSERBUTTON:
				if (m_MouseState.rgbButtons[1] & 0x80 && !m_CollisionWidget)
				{
					m_RButtonClick = true;
					KeyPush = true;
				}
				break;
			case DIK_MOUSEWHEEL:
				break;
			default:	// 키보드 키를 알아볼 경우
				if (m_KeyArray[Key] & 0x80)
				{
					KeyPush = true;
				}
				break;
			}
			break;
		case Input_Type::Window:
			if (GetAsyncKeyState(Key) & 0x8000)
			{
				KeyPush = true;
			}
			break;
		}

		if (KeyPush)
		{
			if (!m_vecKeyState[Key].State[KeyState_Down] &&
				!m_vecKeyState[Key].State[KeyState_Push])
			{
				m_vecKeyState[Key].State[KeyState_Down] = true;
				m_vecKeyState[Key].State[KeyState_Push] = true;
			}

			else
				m_vecKeyState[Key].State[KeyState_Down] = false;
		}

		else if (m_vecKeyState[Key].State[KeyState_Push])
		{
			m_vecKeyState[Key].State[KeyState_Up] = true;
			m_vecKeyState[Key].State[KeyState_Down] = false;
			m_vecKeyState[Key].State[KeyState_Push] = false;
		}

		else if (m_vecKeyState[Key].State[KeyState_Up])
		{
			m_vecKeyState[Key].State[KeyState_Up] = false;
		}
	}
}

void CInput::UpdateKeyInfo(float DeltaTime)
{
	auto	iter = m_mapKeyInfo.begin();
	auto	iterEnd = m_mapKeyInfo.end();

	for (; iter != iterEnd; ++iter)
	{
		unsigned char Key = iter->second->State.Key;

		if (CEngine::GetInst()->GetEditMode())
			bool b = ImGui::GetIO().WantCaptureMouse;

		if (m_vecKeyState[Key].State[KeyState_Down] &&
			iter->second->Ctrl == m_Ctrl &&
			iter->second->Alt == m_Alt &&
			iter->second->Shift == m_Shift)
		{
			if (iter->second->Callback[KeyState_Down])
			{
				if (CEngine::GetInst()->GetEditMode())
				{
					if (!ImGui::GetIO().WantCaptureMouse)
						iter->second->Callback[KeyState_Down](DeltaTime);
				}

				else
					iter->second->Callback[KeyState_Down](DeltaTime);
			}
		}


		if (m_vecKeyState[Key].State[KeyState_Push] &&
			iter->second->Ctrl == m_Ctrl &&
			iter->second->Alt == m_Alt &&
			iter->second->Shift == m_Shift)
		{
			if (iter->second->Callback[KeyState_Push])
			{
				if (CEngine::GetInst()->GetEditMode())
				{
					if (!ImGui::GetIO().WantCaptureMouse)
						iter->second->Callback[KeyState_Push](DeltaTime);
				}

				else
					iter->second->Callback[KeyState_Push](DeltaTime);
			}
		}


		if (m_vecKeyState[Key].State[KeyState_Up] &&
			iter->second->Ctrl == m_Ctrl &&
			iter->second->Alt == m_Alt &&
			iter->second->Shift == m_Shift)
		{
			if (iter->second->Callback[KeyState_Up])
			{
				if (CEngine::GetInst()->GetEditMode())
				{
					if (!ImGui::GetIO().WantCaptureMouse)
						iter->second->Callback[KeyState_Up](DeltaTime);
				}

				else
					iter->second->Callback[KeyState_Up](DeltaTime);
			}
		}
	}
}

void CInput::ClearCallback()
{
	auto	iter = m_mapKeyInfo.begin();
	auto	iterEnd = m_mapKeyInfo.end();

	for (; iter != iterEnd; ++iter)
	{
		for (int i = 0; i < KeyState_Max; ++i)
		{
			iter->second->Callback[i] = nullptr;
		}
	}
}

unsigned char CInput::ConvertKey(unsigned char Key)
{
	if (m_InputType == Input_Type::Direct)
	{
		switch (Key)
		{
		case VK_LBUTTON:
			return DIK_MOUSELBUTTON;
		case VK_RBUTTON:
			return DIK_MOUSERBUTTON;
		case VK_MBUTTON:
			return DIK_MOUSEWHEEL;
		case VK_BACK:
			return DIK_BACK;
		case VK_TAB:
			return DIK_TAB;
		case VK_RETURN:
			return DIK_RETURN;
		case VK_LCONTROL:
			return DIK_LCONTROL;
		case VK_RCONTROL:
			return DIK_RCONTROL;
		case VK_LMENU:
			return DIK_LALT;
		case VK_RMENU:
			return DIK_RALT;
		case VK_LSHIFT:
			return DIK_LSHIFT;
		case VK_RSHIFT:
			return DIK_RSHIFT;
		case VK_PAUSE:
			return DIK_PAUSE;
		case VK_CAPITAL:
			return DIK_CAPSLOCK;
		case VK_ESCAPE:
			return DIK_ESCAPE;
		case VK_SPACE:
			return DIK_SPACE;
		case VK_NEXT:
			return DIK_PGDN;
		case VK_PRIOR:
			return DIK_PGUP;
		case VK_END:
			return DIK_END;
		case VK_HOME:
			return DIK_HOME;
		case VK_LEFT:
			return DIK_LEFT;
		case VK_UP:
			return DIK_UP;
		case VK_RIGHT:
			return DIK_RIGHT;
		case VK_DOWN:
			return DIK_DOWN;
		case VK_PRINT:
			return DIK_SYSRQ;
		case VK_INSERT:
			return DIK_INSERT;
		case VK_DELETE:
			return DIK_DELETE;
		case VK_HELP:
			return 0;
		case '0':
			return DIK_0;
		case '1':
			return DIK_1;
		case '2':
			return DIK_2;
		case '3':
			return DIK_3;
		case '4':
			return DIK_4;
		case '5':
			return DIK_5;
		case '6':
			return DIK_6;
		case '7':
			return DIK_7;
		case '8':
			return DIK_8;
		case '9':
			return DIK_9;
		case 'A':
			return DIK_A;
		case 'B':
			return DIK_B;
		case 'C':
			return DIK_C;
		case 'D':
			return DIK_D;
		case 'E':
			return DIK_E;
		case 'F':
			return DIK_F;
		case 'G':
			return DIK_G;
		case 'H':
			return DIK_H;
		case 'I':
			return DIK_I;
		case 'J':
			return DIK_J;
		case 'K':
			return DIK_K;
		case 'L':
			return DIK_L;
		case 'M':
			return DIK_M;
		case 'N':
			return DIK_N;
		case 'O':
			return DIK_O;
		case 'P':
			return DIK_P;
		case 'Q':
			return DIK_Q;
		case 'R':
			return DIK_R;
		case 'S':
			return DIK_S;
		case 'T':
			return DIK_T;
		case 'U':
			return DIK_U;
		case 'V':
			return DIK_V;
		case 'W':
			return DIK_W;
		case 'X':
			return DIK_X;
		case 'Y':
			return DIK_Y;
		case 'Z':
			return DIK_Z;
		case VK_OEM_3:
			return DIK_GRAVE;
		case VK_OEM_MINUS:
			return DIK_MINUS;
		case VK_OEM_NEC_EQUAL:
			return DIK_EQUALS;
		case VK_OEM_4:
			return DIK_LBRACKET;
		case VK_OEM_6:
			return DIK_RBRACKET;
		case VK_OEM_5:
			return DIK_BACKSLASH;
		case VK_OEM_1:
			return DIK_SEMICOLON;
		case VK_OEM_7:
			return DIK_APOSTROPHE;
		case VK_OEM_COMMA:
			return DIK_COMMA;
		case VK_OEM_PERIOD:
			return DIK_PERIOD;
		case VK_OEM_2:
			return DIK_SLASH;
		case VK_NUMPAD0:
			return DIK_NUMPAD0;
		case VK_NUMPAD1:
			return DIK_NUMPAD1;
		case VK_NUMPAD2:
			return DIK_NUMPAD2;
		case VK_NUMPAD3:
			return DIK_NUMPAD3;
		case VK_NUMPAD4:
			return DIK_NUMPAD4;
		case VK_NUMPAD5:
			return DIK_NUMPAD5;
		case VK_NUMPAD6:
			return DIK_NUMPAD6;
		case VK_NUMPAD7:
			return DIK_NUMPAD7;
		case VK_NUMPAD8:
			return DIK_NUMPAD8;
		case VK_NUMPAD9:
			return DIK_NUMPAD9;
		case VK_MULTIPLY:
			return DIK_MULTIPLY;
		case VK_ADD:
			return DIK_ADD;
		case VK_SEPARATOR:
			return DIK_NUMPADCOMMA;
		case VK_SUBTRACT:
			return DIK_SUBTRACT;
		case VK_DECIMAL:
			return DIK_DECIMAL;
		case VK_DIVIDE:
			return DIK_DIVIDE;
			//case VK_RETURN:		
			return DIK_NUMPADENTER;
		case VK_F1:
			return DIK_F1;
		case VK_F2:
			return DIK_F2;
		case VK_F3:
			return DIK_F3;
		case VK_F4:
			return DIK_F4;
		case VK_F5:
			return DIK_F5;
		case VK_F6:
			return DIK_F6;
		case VK_F7:
			return DIK_F7;
		case VK_F8:
			return DIK_F8;
		case VK_F9:
			return DIK_F9;
		case VK_F10:
			return DIK_F10;
		case VK_F11:
			return DIK_F11;
		case VK_F12:
			return DIK_F12;
		case VK_F13:
			return DIK_F13;
		case VK_F14:
			return DIK_F14;
		case VK_F15:
			return DIK_F15;
		case VK_F16:
		case VK_F17:
		case VK_F18:
		case VK_F19:
		case VK_F20:
		case VK_F21:
		case VK_F22:
		case VK_F23:
		case VK_F24:
			return 0;
		case VK_NUMLOCK:
			return DIK_NUMLOCK;
		case VK_SCROLL:
			return DIK_SCROLL;
		case VK_LWIN:
			return DIK_LWIN;
		case VK_RWIN:
			return DIK_RWIN;
		case VK_APPS:
			return DIK_APPS;
		case VK_OEM_102:
			return DIK_OEM_102;
		}

		return 0xff;
	}

	return Key;
}
