#ifndef DOT_SCENELOADER_H
#define DOT_SCENELOADER_H

#include <OGRE/OgreString.h>
#include <OGRE/OgreVector3.h>
#include <OGRE/OgreQuaternion.h>
#include <OGRE/OgreResourceGroupManager.h>
#include <vector>

#include "rapidxml.hpp"
using namespace rapidxml;

namespace Ogre
{
	class SceneManager;
	class SceneNode;
	class TerrainGroup;
	class TerrainGlobalOptions;
}

using namespace Ogre;

class nodeProperty
{
public:
	String nodeName;
	String propertyNm;
	String valueName;
	String typeName;

	nodeProperty(const String &node, const String &propertyName, const String &value, const String &type)
		: nodeName(node), propertyNm(propertyName), valueName(value), typeName(type) 
	{
		//
	}
};

class DotSceneLoader
{
public:
	TerrainGlobalOptions *mTerrainGlobalOptions;

	DotSceneLoader();
	virtual ~DotSceneLoader();

	void parseDotScene(const String &SceneName, const String &groupName, SceneManager *mySceneMgr, SceneNode *pAttachNode = NULL, const String &sPrependNode = "");
	String getProperty(const String &ndNm, const String &prop);

	TerrainGroup* getTerrainGroup()
	{ 
		return mTerrainGroup; 
	}

	std::vector<nodeProperty> nodeProperties;
	std::vector<String> staticObjects;
	std::vector<String> dynamicObjects;

protected:
	void processScene(xml_node<>* XMLRoot);

	void processNodes(xml_node<>* XMLNode);
	void processExternals(xml_node<>* XMLNode);
	void processEnvironment(xml_node<>* XMLNode);
	void processTerrain(xml_node<>* XMLNode);
	void processTerrainPage(xml_node<>* XMLNode);
	void processBlendmaps(xml_node<>* XMLNode);
	void processUserDataReference(xml_node<>* XMLNode, SceneNode *pParent = 0);
	void processUserDataReference(xml_node<>* XMLNode, Entity *pEntity);
	void processOctree(xml_node<>* XMLNode);
	void processLight(xml_node<>* XMLNode, SceneNode *pParent = 0);
	void processCamera(xml_node<>* XMLNode, SceneNode *pParent = 0);

	void processNode(xml_node<>* XMLNode, SceneNode *pParent = 0);
	void processLookTarget(xml_node<>* XMLNode, SceneNode *pParent);
	void processTrackTarget(xml_node<>* XMLNode, SceneNode *pParent);
	void processEntity(xml_node<>* XMLNode, SceneNode *pParent);
	void processParticleSystem(xml_node<>* XMLNode, SceneNode *pParent);
	void processBillboardSet(xml_node<>* XMLNode, SceneNode *pParent);
	void processPlane(xml_node<>* XMLNode, SceneNode *pParent);

	void processFog(xml_node<>* XMLNode);
	void processSkyBox(xml_node<>* XMLNode);
	void processSkyDome(xml_node<>* XMLNode);
	void processSkyPlane(xml_node<>* XMLNode);
	void processClipping(xml_node<>* XMLNode);

	void processLightRange(xml_node<>* XMLNode, Light *pLight);
	void processLightAttenuation(xml_node<>* XMLNode, Light *pLight);

	String getAttrib(xml_node<>* XMLNode, const String &parameter, const String &defaultValue = "");
	Real getAttribReal(xml_node<>* XMLNode, const String &parameter, Real defaultValue = 0);
	bool getAttribBool(xml_node<>* XMLNode, const String &parameter, bool defaultValue = false);

	Ogre::Vector3 parseVector3(xml_node<>* XMLNode);
	Quaternion parseQuaternion(xml_node<>* XMLNode);
	ColourValue parseColour(xml_node<>* XMLNode);


	SceneManager *mSceneMgr;
	SceneNode *mAttachNode;
	String m_sGroupName;
	String m_sPrependNode;
	TerrainGroup* mTerrainGroup;
	Ogre::Vector3 mTerrainPosition;
	Ogre::Vector3 mLightDirection;
};

#endif // DOT_SCENELOADER_H