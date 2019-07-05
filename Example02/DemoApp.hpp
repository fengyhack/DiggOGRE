#ifndef OGRE_DEMO_APP_HPP
#define OGRE_DEMO_APP_HPP

#include "OgreFramework.hpp"

class DemoApp : public KeyListener
{
public:
	DemoApp();
	~DemoApp();

	void startDemo();

	bool keyPressed(const KeyEvent &keyEventRef);
	bool keyReleased(const KeyEvent &keyEventRef);

private:
	void setupDemoScene();
	void runDemo();

	OgreFramework* m_pOgreFramework;
	SceneNode*    m_pOgreHeadNode;
	Entity*    m_pOgreHeadEntity;

	bool    m_bShutdown;
};

#endif // OGRE_DEMO_APP_HPP