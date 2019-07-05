#ifndef MENU_STATE_HPP
#define MENU_STATE_HPP

#include "AppState.hpp"

class MenuState : public AppState
{
public:
	MenuState();

	DECLARE_APPSTATE_CLASS(MenuState)

	void enter();
	void createScene();
	void exit();

	bool keyPressed(const KeyEvent &keyEventRef);
	bool keyReleased(const KeyEvent &keyEventRef);

	bool mouseMoved(const MouseEvent &evt);
	bool mousePressed(const MouseEvent &evt, MouseButtonID id);
	bool mouseReleased(const MouseEvent &evt, MouseButtonID id);

	void buttonHit(OgreBites::Button* button);

	void update(double timeSinceLastFrame);

private:
	OgreFramework* m_pOgreFramework;
	bool m_bQuit;
};

#endif