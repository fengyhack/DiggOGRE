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
#include <OGRE/OgreFrameListener.h>

#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include <OGRE/SdkTrays.h>

using namespace Ogre;
using namespace OgreBites;
using namespace OIS;

class OgreFramework : public Singleton<OgreFramework>, KeyListener, MouseListener, SdkTrayListener
{
public:
	OgreFramework();
	~OgreFramework();

	bool initOgre(String wndTitle, KeyListener *pKeyListener = 0, MouseListener *pMouseListener = 0);
	void updateOgre(double timeSinceLastFrame);
	void moveCamera();
	void getInput();

	bool isOgreToBeShutDown()const
	{ 
		return m_bShutDownOgre; 
	}

	bool keyPressed(const KeyEvent &keyEventRef);
	bool keyReleased(const KeyEvent &keyEventRef);

	bool mouseMoved(const MouseEvent &evt);
	bool mousePressed(const MouseEvent &evt, MouseButtonID id);
	bool mouseReleased(const MouseEvent &evt, MouseButtonID id);

	Root*    m_pRoot;
	SceneManager*    m_pSceneMgr;
	RenderWindow*    m_pRenderWnd;
	Camera*    m_pCamera;
	Viewport*    m_pViewport;
	Log*    m_pLog;
	Timer*    m_pTimer;

	InputManager*    m_pInputMgr;
	Keyboard*    m_pKeyboard;
	Mouse*    m_pMouse;

private:
	OgreFramework(const OgreFramework&);
	OgreFramework& operator= (const OgreFramework&);

	SdkTrayManager*    m_pTrayMgr;
	FrameEvent    m_FrameEvent;
	int    m_iNumScreenShots;
	
	bool    m_bShutDownOgre;

	Ogre::Vector3    m_TranslateVector;
	float   m_MoveSpeed;
	Degree    m_RotateSpeed;
	float    m_MoveScale;
	Degree    m_RotScale;
};

#endif // OGRE_FRAMEWORK_HPP