#include "OgreFramework.hpp"
#include <OGRE/OgreTextureManager.h>

#pragma comment(lib,"OgreMain.lib")
#pragma comment(lib,"OIS.lib")

using namespace Ogre;

template<> OgreFramework* Singleton<OgreFramework>::msSingleton = 0;

OgreFramework::OgreFramework()
{
	m_MoveSpeed = 0.1f;
	m_RotateSpeed = 0.3f;

	m_bShutDownOgre = false;
	m_iNumScreenShots = 0;

	m_pRoot = 0;
	m_pSceneMgr = 0;
	m_pRenderWnd = 0;
	m_pCamera = 0;
	m_pViewport = 0;
	m_pLog = 0;
	m_pTimer = 0;

	m_pInputMgr = 0;
	m_pKeyboard = 0;
	m_pMouse = 0;

	m_pTrayMgr = 0;
	m_FrameEvent = FrameEvent();
}

bool OgreFramework::initOgre(String wndTitle,KeyListener *pKeyListener, MouseListener *pMouseListener)
{
	LogManager* logMgr = new LogManager();

	m_pLog = LogManager::getSingleton().createLog("LoggerFile.log", true, true, false);
	m_pLog->setDebugOutputEnabled(true);

	m_pRoot = new Root("plugins.cfg","settings.cfg","");

	if (!m_pRoot->showConfigDialog())
	{
		return false;
	}
	m_pRenderWnd = m_pRoot->initialise(true, wndTitle);

	m_pSceneMgr = m_pRoot->createSceneManager(ST_GENERIC, "SceneManager");
	m_pSceneMgr->setAmbientLight(ColourValue(0.7f, 0.7f, 0.7f));

	m_pCamera = m_pSceneMgr->createCamera("Camera");
	m_pCamera->setPosition(Ogre::Vector3(0, 60, 60));
	m_pCamera->lookAt(Ogre::Vector3(0, 0, 0));
	m_pCamera->setNearClipDistance(1);

	m_pViewport = m_pRenderWnd->addViewport(m_pCamera);
	m_pViewport->setBackgroundColour(ColourValue(0.8f, 0.7f, 0.6f, 1.0f));

	m_pCamera->setAspectRatio(float(m_pViewport->getActualWidth()) / float(m_pViewport->getActualHeight()));

	m_pViewport->setCamera(m_pCamera);

	size_t hWnd = 0;
	ParamList paramList;
	m_pRenderWnd->getCustomAttribute("WINDOW", &hWnd);

	paramList.insert(ParamList::value_type("WINDOW", StringConverter::toString(hWnd)));

	m_pInputMgr = InputManager::createInputSystem(paramList);

	m_pKeyboard = static_cast<Keyboard*>(m_pInputMgr->createInputObject(OISKeyboard, true));
	m_pMouse = static_cast<Mouse*>(m_pInputMgr->createInputObject(OISMouse, true));

	m_pMouse->getMouseState().height = m_pRenderWnd->getHeight();
	m_pMouse->getMouseState().width = m_pRenderWnd->getWidth();

	if (pKeyListener == 0)
	{
		m_pKeyboard->setEventCallback(this);
	}
	else
	{
		m_pKeyboard->setEventCallback(pKeyListener);
	}

	if (pMouseListener == 0)
	{
		m_pMouse->setEventCallback(this);
	}
	else
	{
		m_pMouse->setEventCallback(pMouseListener);
	}

	String secName, typeName, archName;
	ConfigFile cf;
	cf.load("resources.cfg");

	ConfigFile::SectionIterator seci = cf.getSectionIterator();
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}
	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	m_pTimer = new Timer();
	m_pTimer->reset();

	m_pTrayMgr = new SdkTrayManager("TrayMgr", m_pRenderWnd, m_pMouse, this);
	m_pTrayMgr->showFrameStats(TL_BOTTOMLEFT);
	m_pTrayMgr->showLogo(TL_BOTTOMRIGHT);
	m_pTrayMgr->hideCursor();

	m_pRenderWnd->setActive(true);

	return true;
}

OgreFramework::~OgreFramework()
{
	if (m_pInputMgr)
	{
		InputManager::destroyInputSystem(m_pInputMgr);
	}
	if (m_pTrayMgr)
	{
		delete m_pTrayMgr;
	}
	if (m_pRoot)
	{
		delete m_pRoot;
	}
}

bool OgreFramework::keyPressed(const KeyEvent &keyEventRef)
{
	if (m_pKeyboard->isKeyDown(KC_ESCAPE))
	{
		m_bShutDownOgre = true;
		return true;
	}

	if (m_pKeyboard->isKeyDown(KC_SYSRQ))
	{
		m_pRenderWnd->writeContentsToTimestampedFile("BOF_Screenshot_", ".png");
		return true;
	}

	if (m_pKeyboard->isKeyDown(KC_M))
	{
		static int mode = 0;

		if (mode == 2)
		{
			m_pCamera->setPolygonMode(PM_SOLID);
			mode = 0;
		}
		else if (mode == 0)
		{
			m_pCamera->setPolygonMode(PM_WIREFRAME);
			mode = 1;
		}
		else if (mode == 1)
		{
			m_pCamera->setPolygonMode(PM_POINTS);
			mode = 2;
		}
	}

	if (m_pKeyboard->isKeyDown(KC_O))
	{
		if (m_pTrayMgr->isLogoVisible())
		{
			m_pTrayMgr->hideLogo();
			m_pTrayMgr->hideFrameStats();
		}
		else
		{
			m_pTrayMgr->showLogo(TL_BOTTOMRIGHT);
			m_pTrayMgr->showFrameStats(TL_BOTTOMLEFT);
		}
	}

	return true;
}

bool OgreFramework::keyReleased(const KeyEvent &keyEventRef)
{
	return true;
}

bool OgreFramework::mouseMoved(const MouseEvent &evt)
{
	m_pCamera->yaw(Degree(evt.state.X.rel * -0.1f));
	m_pCamera->pitch(Degree(evt.state.Y.rel * -0.1f));

	return true;
}

bool OgreFramework::mousePressed(const MouseEvent &evt, MouseButtonID id)
{
	return true;
}

bool OgreFramework::mouseReleased(const MouseEvent &evt, MouseButtonID id)
{
	return true;
}

void OgreFramework::updateOgre(double timeSinceLastFrame)
{
	m_MoveScale = m_MoveSpeed   * (float)timeSinceLastFrame;
	m_RotScale = m_RotateSpeed * (float)timeSinceLastFrame;

	m_TranslateVector = Ogre::Vector3::ZERO;

	getInput();
	moveCamera();

	m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
	m_pTrayMgr->frameRenderingQueued(m_FrameEvent);
}

void OgreFramework::moveCamera()
{
	if (m_pKeyboard->isKeyDown(KC_LSHIFT))
	{
		m_pCamera->moveRelative(m_TranslateVector);
	}
	else
	{
		m_pCamera->moveRelative(m_TranslateVector / 10);
	}
}

void OgreFramework::getInput()
{
	if (m_pKeyboard->isKeyDown(KC_A))
	{
		m_TranslateVector.x = -m_MoveScale;
	}

	if (m_pKeyboard->isKeyDown(KC_D))
	{
		m_TranslateVector.x = m_MoveScale;
	}

	if (m_pKeyboard->isKeyDown(KC_W))
	{
		m_TranslateVector.z = -m_MoveScale;
	}

	if (m_pKeyboard->isKeyDown(KC_S))
	{
		m_TranslateVector.z = m_MoveScale;
	}
}
