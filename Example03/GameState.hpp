#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "AppState.hpp"
#include "DotSceneLoader.hpp"
#include <OGRE/OgreSubEntity.h>
#include <OGRE/OgreMaterialManager.h>

enum QueryFlags
{
	OGRE_HEAD_MASK = 1 << 0,
	CUBE_MASK = 1 << 1
};

class GameState : public AppState
{
public:
	GameState();

	DECLARE_APPSTATE_CLASS(GameState)

	void enter();
	void createScene();
	void exit();
	bool pause();
	void resume();

	void moveCamera();
	void getInput();
	void buildGUI();

	bool keyPressed(const KeyEvent &keyEventRef);
	bool keyReleased(const KeyEvent &keyEventRef);

	bool mouseMoved(const MouseEvent &evt);
	bool mousePressed(const MouseEvent &evt, MouseButtonID id);
	bool mouseReleased(const MouseEvent &evt, MouseButtonID id);

	void onLeftPressed(const MouseEvent &evt);
	void itemSelected(OgreBites::SelectMenu* menu);

	void update(double timeSinceLastFrame);

private:
	OgreFramework* m_pOgreFramework;
	SceneNode*		m_pOgreHeadNode;
	Entity*			m_pOgreHeadEntity;
	MaterialPtr		m_pOgreHeadMat;
	MaterialPtr		m_pOgreHeadMatHigh;

	ParamsPanel*		m_pDetailsPanel;
	bool				m_bQuit;

	Ogre::Vector3			m_TranslateVector;
	Real			m_MoveSpeed;
	Degree			m_RotateSpeed;
	float				m_MoveScale;
	Degree			m_RotScale;

	RaySceneQuery*		m_pRSQ;
	SceneNode*		m_pCurrentObject;
	Entity*			m_pCurrentEntity;
	bool				m_bLMouseDown, m_bRMouseDown;
	bool				m_bSettingsMode;
};

#endif