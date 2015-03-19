// OGRE Example Hello
#include <OGRE\ExampleApplication.h>
#include <irrKlang.h>
#pragma comment(lib,"OgreMain.lib")
#pragma comment(lib,"OIS.lib")
#pragma comment(lib,"irrKlang.lib")

using namespace Ogre;
using namespace irrklang;

class ExampleHello : public ExampleApplication
{
public:
	void createScene()
	{
		Entity* ent1 = mSceneMgr->createEntity("MyEntity1", "Sinbad.mesh");
		SceneNode* node1 = mSceneMgr->createSceneNode("Node1");
		node1->attachObject(ent1);
		mSceneMgr->getRootSceneNode()->addChild(node1);
		soundEngine = createIrrKlangDevice(ESOD_AUTO_DETECT, ESEO_DEFAULT_OPTIONS);
		sound2D = soundEngine->play2D("1.mp3", true, true, false, ESM_AUTO_DETECT, true);
		ISoundEffectControl* fx = sound2D->getSoundEffectControl();
		fx->enableDistortionSoundEffect();
		sound2D->setIsPaused(false);
	}

	void soundDrop()
	{
		sound2D->drop();
		soundEngine->drop();
	}
private:
	ISoundEngine* soundEngine;
	ISound* sound2D;
};

int WinMain(HINSTANCE h1, HINSTANCE h2, LPTSTR cmdline, int cmdshow)
{
	ExampleHello app;
	app.go();
	app.soundDrop();

	return 0;
}