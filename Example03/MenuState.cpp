#include "MenuState.hpp"

MenuState::MenuState()
{
	m_pOgreFramework = OgreFramework::getSingletonPtr();
	m_bQuit = false;
	m_FrameEvent = FrameEvent();
}

void MenuState::enter()
{
	m_pOgreFramework->m_pLog->logMessage("Entering MenuState...");

	m_pSceneMgr = m_pOgreFramework->m_pRoot->createSceneManager(ST_GENERIC, "MenuSceneMgr");
	m_pSceneMgr->setAmbientLight(ColourValue(0.7f, 0.7f, 0.7f));

	m_pCamera = m_pSceneMgr->createCamera("MenuCam");
	m_pCamera->setPosition(Ogre::Vector3(0, 25, -50));
	m_pCamera->lookAt(Ogre::Vector3(0, 0, 0));
	m_pCamera->setNearClipDistance(1);

	m_pCamera->setAspectRatio(Real(m_pOgreFramework->m_pViewport->getActualWidth()) / Real(m_pOgreFramework->m_pViewport->getActualHeight()));

	m_pOgreFramework->m_pViewport->setCamera(m_pCamera);

	m_pOgreFramework->m_pTrayMgr->destroyAllWidgets();
	m_pOgreFramework->m_pTrayMgr->showFrameStats(TL_BOTTOMLEFT);
	m_pOgreFramework->m_pTrayMgr->showLogo(TL_BOTTOMRIGHT);
	m_pOgreFramework->m_pTrayMgr->showCursor();
	m_pOgreFramework->m_pTrayMgr->createButton(TL_CENTER, "EnterBtn", "Enter GameState", 250);
	m_pOgreFramework->m_pTrayMgr->createButton(TL_CENTER, "ExitBtn", "Exit AdvancedOgreFramework", 250);
	m_pOgreFramework->m_pTrayMgr->createLabel(TL_TOP, "MenuLbl", "Menu mode", 250);

	createScene();
}

void MenuState::createScene()
{
}

void MenuState::exit()
{
	m_pOgreFramework->m_pLog->logMessage("Leaving MenuState...");

	m_pSceneMgr->destroyCamera(m_pCamera);
	if (m_pSceneMgr)
	{
		m_pOgreFramework->m_pRoot->destroySceneManager(m_pSceneMgr);
	}

	m_pOgreFramework->m_pTrayMgr->clearAllTrays();
	m_pOgreFramework->m_pTrayMgr->destroyAllWidgets();
	m_pOgreFramework->m_pTrayMgr->setListener(0);
}

bool MenuState::keyPressed(const KeyEvent &keyEventRef)
{
	if (m_pOgreFramework->m_pKeyboard->isKeyDown(KC_ESCAPE))
	{
		m_bQuit = true;
		return true;
	}

	m_pOgreFramework->keyPressed(keyEventRef);
	return true;
}

bool MenuState::keyReleased(const KeyEvent &keyEventRef)
{
	m_pOgreFramework->keyReleased(keyEventRef);
	return true;
}

bool MenuState::mouseMoved(const MouseEvent &evt)
{
	if (m_pOgreFramework->m_pTrayMgr->injectMouseMove(evt))
	{
		return true;
	}
	return true;
}

bool MenuState::mousePressed(const MouseEvent &evt, MouseButtonID id)
{
	if (m_pOgreFramework->m_pTrayMgr->injectMouseDown(evt, id))
	{
		return true;
	}
	return true;
}

bool MenuState::mouseReleased(const MouseEvent &evt, MouseButtonID id)
{
	if (m_pOgreFramework->m_pTrayMgr->injectMouseUp(evt, id))
	{
		return true;
	}
	return true;
}

void MenuState::update(double timeSinceLastFrame)
{
	m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
	m_pOgreFramework->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);

	if (m_bQuit == true)
	{
		shutdown();
		return;
	}
}

void MenuState::buttonHit(OgreBites::Button *button)
{
	if (button->getName() == "ExitBtn")
	{
		m_bQuit = true;
	}
	else if (button->getName() == "EnterBtn")
	{
		changeAppState(findByName("GameState"));
	}
}

