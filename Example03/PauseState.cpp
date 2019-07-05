#include "PauseState.hpp"

PauseState::PauseState()
{
	m_pOgreFramework = OgreFramework::getSingletonPtr();
	m_bQuit = false;
	m_bQuestionActive = false;
	m_FrameEvent = FrameEvent();
}

void PauseState::enter()
{
	m_pOgreFramework->m_pLog->logMessage("Entering PauseState...");

	m_pSceneMgr = m_pOgreFramework->m_pRoot->createSceneManager(ST_GENERIC, "PauseSceneMgr");
	m_pSceneMgr->setAmbientLight(ColourValue(0.7f, 0.7f, 0.7f));

	m_pCamera = m_pSceneMgr->createCamera("PauseCam");
	m_pCamera->setPosition(Ogre::Vector3(0, 25, -50));
	m_pCamera->lookAt(Ogre::Vector3(0, 0, 0));
	m_pCamera->setNearClipDistance(1);

	m_pCamera->setAspectRatio(Real(m_pOgreFramework->m_pViewport->getActualWidth()) / Real(m_pOgreFramework->m_pViewport->getActualHeight()));

	m_pOgreFramework->m_pViewport->setCamera(m_pCamera);

	m_pOgreFramework->m_pTrayMgr->destroyAllWidgets();
	m_pOgreFramework->m_pTrayMgr->showCursor();
	m_pOgreFramework->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "BackToGameBtn", "Return to GameState", 250);
	m_pOgreFramework->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "BackToMenuBtn", "Return to Menu", 250);
	m_pOgreFramework->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "ExitBtn", "Exit AdvancedOgreFramework", 250);
	m_pOgreFramework->m_pTrayMgr->createLabel(OgreBites::TL_TOP, "PauseLbl", "Pause mode", 250);

	m_bQuit = false;

	createScene();
}

void PauseState::createScene()
{
}

void PauseState::exit()
{
	m_pOgreFramework->m_pLog->logMessage("Leaving PauseState...");

	m_pSceneMgr->destroyCamera(m_pCamera);
	if (m_pSceneMgr)
	{
		m_pOgreFramework->m_pRoot->destroySceneManager(m_pSceneMgr);
	}

	m_pOgreFramework->m_pTrayMgr->clearAllTrays();
	m_pOgreFramework->m_pTrayMgr->destroyAllWidgets();
	m_pOgreFramework->m_pTrayMgr->setListener(0);
}

bool PauseState::keyPressed(const KeyEvent &keyEventRef)
{
	if (m_pOgreFramework->m_pKeyboard->isKeyDown(KC_ESCAPE) && !m_bQuestionActive)
	{
		m_bQuit = true;
		return true;
	}

	m_pOgreFramework->keyPressed(keyEventRef);

	return true;
}

bool PauseState::keyReleased(const KeyEvent &keyEventRef)
{
	m_pOgreFramework->keyReleased(keyEventRef);

	return true;
}

bool PauseState::mouseMoved(const MouseEvent &evt)
{
	if (m_pOgreFramework->m_pTrayMgr->injectMouseMove(evt))
	{
		return true;
	}
	return true;
}

bool PauseState::mousePressed(const MouseEvent &evt, MouseButtonID id)
{
	if (m_pOgreFramework->m_pTrayMgr->injectMouseDown(evt, id))
	{
		return true;
	}
	return true;
}

bool PauseState::mouseReleased(const MouseEvent &evt, MouseButtonID id)
{
	if (m_pOgreFramework->m_pTrayMgr->injectMouseUp(evt, id))
	{
		return true;
	}
	return true;
}

void PauseState::update(double timeSinceLastFrame)
{
	m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
	m_pOgreFramework->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);

	if (m_bQuit == true)
	{
		popAppState();
		return;
	}
}

void PauseState::buttonHit(OgreBites::Button *button)
{
	if (button->getName() == "ExitBtn")
	{
		m_pOgreFramework->m_pTrayMgr->showYesNoDialog("Sure?", "Really leave?");
		m_bQuestionActive = true;
	}
	else if (button->getName() == "BackToGameBtn")
	{
		popAllAndPushAppState(findByName("GameState"));
		m_bQuit = true;
	}
	else if (button->getName() == "BackToMenuBtn")
	{
		popAllAndPushAppState(findByName("MenuState"));
	}
}

void PauseState::yesNoDialogClosed(const DisplayString& question, bool yesHit)
{
	if (yesHit == true)
	{
		shutdown();
	}
	else
	{
		m_pOgreFramework->m_pTrayMgr->closeDialog();
	}

	m_bQuestionActive = false;
}