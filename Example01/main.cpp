#include <OGRE/OgreRoot.h>
#include <OGRE/OgreRenderSystem.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreWindowEventUtilities.h>
#include <memory>  // std::auto_ptr
#include <exception> // std::exception

#pragma comment(lib,"OgreMain.lib")
using namespace Ogre;
using namespace std;

int WinMain(HINSTANCE h1, HINSTANCE h2, LPTSTR cmdLine, int cmdShow)
{
	auto_ptr<Root> pRoot(new Root("", "", "Example01.LOG")); // auto delete

	std::vector<string>  sPlugins;
	sPlugins.push_back("RenderSystem_Direct3D9");
	sPlugins.push_back("Plugin_ParticleFX");
	sPlugins.push_back("Plugin_CgProgramManager");
	//sPlugins.push_back("Plugin_PCZSceneManager");
	//sPlugins.push_back("Plugin_OctreeZone");
	sPlugins.push_back("Plugin_OctreeSceneManager");
	//sPlugins.push_back("Plugin_BSPSceneManager");
	for (uint i = 0; i < sPlugins.size(); ++i)
	{
		pRoot->loadPlugin(sPlugins[i]);
	}

	const RenderSystemList& rsList = pRoot->getAvailableRenderers();
	if (rsList.size() == 0)
	{
		LogManager::getSingleton().logMessage("Sorry, no rendersystem was found.");
		return 1;
	}

	pRoot->setRenderSystem(rsList[0]);

	pRoot->initialise(false, "", "");

	string title = "OGRE MadMarx Tutorial -- Example 01";
	NameValuePairList nvParams;
	nvParams["FSAA"] = "0";
	nvParams["vsync"] = "true";
	RenderWindow* pRenderWindow = pRoot->createRenderWindow(title, 800, 600, false, &nvParams);

	while (true)
	{
		if (pRenderWindow->isClosed()) break;
		WindowEventUtilities::messagePump();
	}

	LogManager::getSingleton().logMessage("End of the program");

	return 0;
}