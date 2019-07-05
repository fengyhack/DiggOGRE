#ifndef PAUSE_STATE_HPP
#define PAUSE_STATE_HPP

#include "AppState.hpp"

class PauseState : public AppState
{
public:
	PauseState();

	DECLARE_APPSTATE_CLASS(PauseState)

	void enter();
	void createScene();
	void exit();

	bool keyPressed(const KeyEvent &keyEventRef);
	bool keyReleased(const KeyEvent &keyEventRef);

	bool mouseMoved(const MouseEvent &evt);
	bool mousePressed(const MouseEvent &evt, MouseButtonID id);
	bool mouseReleased(const MouseEvent &evt, MouseButtonID id);

	void buttonHit(OgreBites::Button* button);
	void yesNoDialogClosed(const DisplayString& question, bool yesHit);

	void update(double timeSinceLastFrame);

private:
	OgreFramework* m_pOgreFramework;
	bool m_bQuit;
	bool m_bQuestionActive;
};

#endif