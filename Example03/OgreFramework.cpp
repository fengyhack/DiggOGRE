#include "OgreFramework.hpp"

template<> OgreFramework* Singleton<OgreFramework>::msSingleton = 0;

OgreFramework::OgreFramework()
{
	m_pRoot = 0;
	m_pRenderWnd = 0;
	m_pViewport = 0;
	m_pLog = 0;
	m_pTimer = 0;

	m_pInputMgr = 0;
	m_pKeyboard = 0;
	m_pMouse = 0;
	m_pTrayMgr = 0;
}

OgreFramework::~OgreFramework()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Shutdown OGRE...");
	if (m_pTrayMgr)
	{
		delete m_pTrayMgr;
	}
	if (m_pInputMgr)
	{
		InputManager::destroyInputSystem(m_pInputMgr);
	}
	if (m_pRoot)
	{
		delete m_pRoot;
	}
}

bool OgreFramework::initOgre(String wndTitle, KeyListener *pKeyListener, MouseListener *pMouseListener)
{
	LogManager* logMgr = new LogManager();

	m_pLog = LogManager::getSingleton().createLog("LoggerFile.log", true, true, false);
	m_pLog->setDebugOutputEnabled(true);

	m_pRoot = new Ogre::Root("plugins.cfg","settings.cfg","");

	if (!m_pRoot->showConfigDialog())
	{
		return false;
	}
	m_pRenderWnd = m_pRoot->initialise(true, wndTitle);

	m_pViewport = m_pRenderWnd->addViewport(0);
	m_pViewport->setBackgroundColour(ColourValue(0.5f, 0.5f, 0.5f, 1.0f));

	m_pViewport->setCamera(0);

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

	m_pTrayMgr = new SdkTrayManager("AOFTrayMgr", m_pRenderWnd, m_pMouse, 0);

	m_pTimer = new Timer();
	m_pTimer->reset();

	m_pRenderWnd->setActive(true);

	return true;
}

bool OgreFramework::keyPressed(const KeyEvent &keyEventRef)
{
	if (m_pKeyboard->isKeyDown(KC_SYSRQ))
	{
		m_pRenderWnd->writeContentsToTimestampedFile("AOF_Screenshot_", ".jpg");
		return true;
	}

	if (m_pKeyboard->isKeyDown(KC_O))
	{
		if (m_pTrayMgr->isLogoVisible())
		{
			m_pTrayMgr->hideFrameStats();
			m_pTrayMgr->hideLogo();
		}
		else
		{
			m_pTrayMgr->showFrameStats(TL_BOTTOMLEFT);
			m_pTrayMgr->showLogo(TL_BOTTOMRIGHT);
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
}

