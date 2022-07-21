#include "render_window.h"
//
#include "../tools/string_utility.h"
#include "../events/i_event_manager.h"
#include "os_message_data.h"
#include "../events/evt_data_os_message.h"
#include "engine.h"

bool RenderWindow::Initialize(const RenderWindowConfig& cfg) {
	m_window_config = cfg;

	static bool raw_input_initialized = false;
	if (!raw_input_initialized) {
		RAWINPUTDEVICE rid;
		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = 0;
		if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
			exit(-1);
		}
		raw_input_initialized = true;
	}

	this->RegisterWindowClass();

	Engine* pEngine = Engine::GetEngine();
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int posX = 0;
	int posY = 0;

	if (!cfg.is_windowed_mode) {
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));

		if (cfg.options.m_full_screen_max) {
			m_window_config.height = screenHeight;
			m_window_config.width = screenWidth;
		}
		else {
			screenWidth = cfg.width;
			screenHeight = cfg.height;
		}
		
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = 0;
		posY = 0;

		m_handle = CreateWindowEx(
			WS_EX_APPWINDOW,
			s2w(m_window_config.window_class).c_str(),
			s2w(m_window_config.window_title).c_str(),
			//WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, // окно без рамки
			WS_OVERLAPPEDWINDOW,
			posX,
			posY,
			screenWidth,
			screenHeight,
			NULL,
			NULL,
			m_window_config.hInstance,
			pEngine
		);
	}
	else {
		screenWidth = cfg.width;
		screenHeight = cfg.height;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

		DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
		RECT wr;
		wr.left = 0;
		wr.top = 0;
		wr.right = wr.left + cfg.width;
		wr.bottom = wr.top + cfg.height;
		AdjustWindowRect(&wr, style, FALSE);

		m_handle = CreateWindowEx(
			0,
			s2w(m_window_config.window_class).c_str(),
			s2w(m_window_config.window_title).c_str(),
			style,
			posX,
			posY,
			wr.right - wr.left,
			wr.bottom - wr.top,
			NULL,
			NULL,
			m_window_config.hInstance,
			pEngine
		);
	}

	if (m_handle == NULL) {
		return false;
	}

	ShowWindow(m_handle, SW_SHOW);
	SetForegroundWindow(m_handle);
	SetFocus(m_handle);

	return true;
}

bool RenderWindow::ProcessMessages() {
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));

	while (PeekMessage(&msg, m_handle, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL) {
		if (!IsWindow(m_handle)) {
			m_handle = NULL;
			UnregisterClass(s2w(m_window_config.window_class).c_str(), m_window_config.hInstance);
			return false;
		}
	}

	return true;
}

RenderWindow::~RenderWindow() {
	if (m_handle != NULL) {
		UnregisterClass(s2w(m_window_config.window_class).c_str(), m_window_config.hInstance);
		DestroyWindow(m_handle);
	}
}

HWND RenderWindow::GetHWND() const {
	return m_handle;
}

HINSTANCE RenderWindow::GetInstance() const {
	return m_window_config.hInstance;
}

int RenderWindow::GetWidth() const {
	return m_window_config.width;
}

int RenderWindow::GetHeight() const {
	return m_window_config.height;
}

const RenderWindowConfig& RenderWindow::GetConfig() const {
	return m_window_config;
}

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_CLOSE: {
			DestroyWindow(hwnd);
			return 0;
		}
		default: {
			Engine* const pWindow = reinterpret_cast<Engine*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_NCCREATE: {
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Engine* pWindow = reinterpret_cast<Engine*>(pCreate->lpCreateParams);
			if (!pWindow) {
				exit(-1);
			}
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));

			return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
		}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_CLOSE: {
			DestroyWindow(hwnd);
			return 0;
		}
		case WM_DISPLAYCHANGE:
		case WM_SYSCOMMAND:
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case MM_JOY1BUTTONDOWN:
		case MM_JOY1BUTTONUP:
		case MM_JOY1MOVE:
		case MM_JOY1ZMOVE:
		case MM_JOY2BUTTONDOWN:
		case MM_JOY2BUTTONUP:
		case MM_JOY2MOVE:
		case MM_JOY2ZMOVE: {
			std::shared_ptr<EvtData_OS_Message> pEvtData_OS_Message(new EvtData_OS_Message({ hwnd , uMsg, wParam, lParam }));
			IEventManager::Get()->VQueueEvent(pEvtData_OS_Message);
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		default: {
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
}

void RenderWindow::RegisterWindowClass() {
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	//wc.lpfnWndProc = HandleMessage;
	wc.lpfnWndProc = HandleMessageSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_window_config.hInstance;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_window_config.window_class_w.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);
}