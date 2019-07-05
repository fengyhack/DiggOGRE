#include "DemoApp.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include "PauseState.hpp"

DemoApp::DemoApp()
{
	m_pOgreFramework = NULL;
	m_pAppStateManager = NULL;
}

DemoApp::~DemoApp()
{
	delete m_pAppStateManager;
	delete m_pOgreFramework;
}

void DemoApp::startDemo()
{
	new OgreFramework();
	m_pOgreFramework = OgreFramework::getSingletonPtr();
	if (!m_pOgreFramework->initOgre("AdvancedOgreFramework", 0, 0))
	{
		return;
	}

	m_pOgreFramework->m_pLog->logMessage("Demo initialized!");

	m_pAppStateManager = new AppStateManager();

	MenuState::create(m_pAppStateManager, "MenuState");
	GameState::create(m_pAppStateManager, "GameState");
	PauseState::create(m_pAppStateManager, "PauseState");

	m_pAppStateManager->start(m_pAppStateManager->findByName("MenuState"));
}
