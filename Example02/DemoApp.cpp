#include "DemoApp.hpp"

DemoApp::DemoApp()
{
	m_pOgreHeadNode = 0;
	m_pOgreHeadEntity = 0;
}

DemoApp::~DemoApp()
{
	delete m_pOgreFramework;
}

void DemoApp::startDemo()
{
	new OgreFramework();
	m_pOgreFramework = OgreFramework::getSingletonPtr();

	if (!m_pOgreFramework->initOgre("DemoApp v1.0", this, 0))
	{
		return;
	}

	m_bShutdown = false;

	m_pOgreFramework->m_pLog->logMessage("Demo initialized!");

	setupDemoScene();
	runDemo();
}

void DemoApp::setupDemoScene()
{
	m_pOgreFramework->m_pSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");

	m_pOgreFramework->m_pSceneMgr->createLight("Light")->setPosition(75, 75, 75);

	m_pOgreHeadEntity = m_pOgreFramework->m_pSceneMgr->createEntity("OgreHeadEntity", "ogrehead.mesh");
	m_pOgreHeadNode = m_pOgreFramework->m_pSceneMgr->getRootSceneNode()->createChildSceneNode("OgreHeadNode");
	m_pOgreHeadNode->attachObject(m_pOgreHeadEntity);
}

void DemoApp::runDemo()
{
	m_pOgreFramework->m_pLog->logMessage("Start main loop...");

	double timeSinceLastFrame = 0;
	double startTime = 0;

	m_pOgreFramework->m_pRenderWnd->resetStatistics();

	while (true)
	{
		if (m_bShutdown || m_pOgreFramework->isOgreToBeShutDown()) break;

		if (m_pOgreFramework->m_pRenderWnd->isClosed())
		{
			m_bShutdown = true;
		}

		WindowEventUtilities::messagePump();

		if (m_pOgreFramework->m_pRenderWnd->isActive())
		{
			startTime = m_pOgreFramework->m_pTimer->getMillisecondsCPU();

			m_pOgreFramework->m_pKeyboard->capture();
			m_pOgreFramework->m_pMouse->capture();
			m_pOgreFramework->updateOgre(timeSinceLastFrame);
			m_pOgreFramework->m_pRoot->renderOneFrame();

			timeSinceLastFrame = m_pOgreFramework->m_pTimer->getMillisecondsCPU() - startTime;
		}
		else
		{
			Sleep(1000);
		}
	}

	m_pOgreFramework->m_pLog->logMessage("Main loop quit");
	m_pOgreFramework->m_pLog->logMessage("Shutdown OGRE...");
}

bool DemoApp::keyPressed(const KeyEvent &keyEventRef)
{
	m_pOgreFramework->keyPressed(keyEventRef);

	if (m_pOgreFramework->m_pKeyboard->isKeyDown(KC_F))
	{
		//do something
	}

	return true;
}

bool DemoApp::keyReleased(const KeyEvent &keyEventRef)
{
	m_pOgreFramework->keyReleased(keyEventRef);

	return true;
}
