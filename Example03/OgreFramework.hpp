#ifndef OGRE_FRAMEWORK_HPP
#define OGRE_FRAMEWORK_HPP

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreOverlay.h>
#include <OGRE/OgreOverlayElement.h>
#include <OGRE/OgreOverlayManager.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreConfigFile.h>

#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include <OGRE/SdkTrays.h>

#pragma comment(lib,"OgreMain.lib")
#pragma comment(lib,"OgreTerrain.lib")
#pragma comment(lib,"OIS.lib")

using namespace Ogre;
using namespace OIS;
using namespace OgreBites;


class OgreFramework : public Singleton<OgreFramework>, KeyListener, MouseListener
{
public:
	OgreFramework();
	~OgreFramework();

	bool initOgre(String wndTitle, KeyListener *pKeyListener = 0, MouseListener *pMouseListener = 0);
	void updateOgre(double timeSinceLastFrame);

	bool keyPressed(const KeyEvent &keyEventRef);
	bool keyReleased(const KeyEvent &keyEventRef);

	bool mouseMoved(const MouseEvent &evt);
	bool mousePressed(const MouseEvent &evt, MouseButtonID id);
	bool mouseReleased(const MouseEvent &evt, MouseButtonID id);

	Root*    m_pRoot;
	RenderWindow*    m_pRenderWnd;
	Viewport*    m_pViewport;
	Log*    m_pLog;
	Timer*	    m_pTimer;

	InputManager*    m_pInputMgr;
	Keyboard*    m_pKeyboard;
	Mouse*    m_pMouse;

	SdkTrayManager*    m_pTrayMgr;

private:
	OgreFramework(const OgreFramework&);
	OgreFramework& operator= (const OgreFramework&);
};

#endif