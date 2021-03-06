#include <Windows.h>
#include <OgreRoot.h>
#include <OgreRenderSystem.h>
#include <OgreWindowEventUtilities.h>
#include <irrKlang.h>
#include <string.h>
#pragma comment(lib,"OgreMain.lib")
#pragma comment(lib,"irrKlang.lib")
#pragma warning(disable:4996)

using namespace Ogre;
using namespace irrklang;
using namespace std;

#define STR_RENDER_SYSTEM_DirectX9  "Direct3D9 Rendering Subsystem"
#define STR_RENDER_SYSTEM_OpenGL  "OpenGL Rendering Subsystem"

#define WINDOW_WIDTH	800	
#define WINDOW_HEIGHT	600		

LRESULT CALLBACK	WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR strCmdLine, INT nCmdShow)
{
	string rsName = STR_RENDER_SYSTEM_DirectX9;
	string windowTitle = string("OGRE Example Window - ") + rsName;
	int width=800, height=600;
	//=============================================================================
	LogManager* logMgr = new LogManager;
	Log* m_pLog = LogManager::getSingleton().createLog("LoggerFile.log", true, true, false);
	Root* m_pRoot = new Root("plugins.cfg", "config.cfg");
	RenderSystemList rsList = m_pRoot->getAvailableRenderers();
	RenderSystemList::iterator it = rsList.begin();
	RenderSystem* pRenderSubystem = NULL;
	while (it != rsList.end())
	{
		pRenderSubystem = *it;
		if (pRenderSubystem->getName() == rsName)
		{
			m_pRoot->setRenderSystem(pRenderSubystem);
			break;
		}
		++it;
	}
	pRenderSubystem = m_pRoot->getRenderSystem();
	if (pRenderSubystem == NULL)
	{
		string msg = "FAILED to set Rendering Subsystem: " + rsName;
		MessageBox(NULL, msg.c_str(), "ERROR", MB_OK);
	}
	if (!m_pRoot->restoreConfig())
	{
		m_pRoot->showConfigDialog();
	}
	//===============================================================================
	width = WINDOW_WIDTH;
	height = WINDOW_HEIGHT;
	//===============================================================================
	LPCSTR wcexName = "MainWindow";
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInst;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); //LoadIcon(hInst, IDI_ICON1);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = wcexName;
	RegisterClassEx(&wndClass);
	DWORD dwStyle = WS_SYSMENU | WS_SIZEBOX;
	HWND hWnd = CreateWindow(wcexName, windowTitle.c_str(), dwStyle, 100, 100, width, height, NULL, NULL, hInst, NULL);
	//===============================================================================
	m_pRoot->initialise(false);
	NameValuePairList miscParams;
	miscParams["parentWindowHandle"] = StringConverter::toString((ULONG)hWnd);
	m_pRoot->createRenderWindow(windowTitle, width, height, false, &miscParams);
	//===============================================================================
	string strSoundFile = "./Media/1.flac";
	ISoundEngine* m_pSoundEngine = createIrrKlangDevice(ESOD_DIRECT_SOUND, ESEO_DEFAULT_OPTIONS);
	ISound* m_pSound2D = m_pSoundEngine->play2D(strSoundFile.c_str(), true, true, false, ESM_AUTO_DETECT, true);
	ISoundEffectControl* fx = m_pSound2D->getSoundEffectControl();
	if(fx!=NULL) fx->enableDistortionSoundEffect();
	m_pSound2D->setIsPaused(false);
	//===============================================================================
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Render Loop
		}
	}
	//===============================================================================
	m_pSound2D->drop();
	m_pSoundEngine->drop();
	m_pRoot->shutdown();
	delete m_pRoot;
	delete m_pLog;
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char key = 0;
	char msg[32] = "You pressed key: ";
	int pos = strlen(msg);
	switch (message)
	{
	case WM_PAINT:
	{
		ValidateRect(hwnd, NULL);
		break;
	}
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)    // ESC
		{
			DestroyWindow(hwnd);
		}
		break;
	}
	case WM_CHAR:
	{
		key = LOWORD(wParam);		
		if (IsCharAlpha(key))
		{
			msg[pos] = key;
			MessageBox(hwnd, msg, "Info", MB_OK | MB_ICONINFORMATION);
		}
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	default:
	{
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	}

	return 0;
}