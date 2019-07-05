#ifndef OGRE_DEMO_HPP
#define OGRE_DEMO_HPP

#include "OgreFramework.hpp"
#include "AppStateManager.hpp"

class DemoApp
{
public:
	DemoApp();
	~DemoApp();

	void startDemo();

private:
	OgreFramework*  m_pOgreFramework;
	AppStateManager*	m_pAppStateManager;
};

#endif