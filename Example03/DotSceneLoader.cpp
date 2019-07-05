#include "DotSceneLoader.hpp"
#include <OGRE/Ogre.h>
#include <OGRE/Terrain/OgreTerrain.h>
#include <OGRE/Terrain/OgreTerrainGroup.h>
#include <OGRE/Terrain/OgreTerrainMaterialGeneratorA.h>

#pragma warning(disable:4390)
#pragma warning(disable:4305)
#pragma warning(disable:4996)


DotSceneLoader::DotSceneLoader() : mSceneMgr(0), mTerrainGroup(0)
{
	mTerrainGlobalOptions = OGRE_NEW TerrainGlobalOptions();
}


DotSceneLoader::~DotSceneLoader()
{
	if (mTerrainGroup)
	{
		OGRE_DELETE mTerrainGroup;
	}

	OGRE_DELETE mTerrainGlobalOptions;
}

void DotSceneLoader::parseDotScene(const String &SceneName, const String &groupName, SceneManager *mySceneMgr, SceneNode *pAttachNode, const String &sPrependNode)
{
	// set up shared object values
	m_sGroupName = groupName;
	mSceneMgr = mySceneMgr;
	m_sPrependNode = sPrependNode;
	staticObjects.clear();
	dynamicObjects.clear();

	xml_document<> XMLDoc;    // character type defaults to char

	xml_node<>* XMLRoot;

	DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(SceneName, groupName);
	char* scene = strdup(stream->getAsString().c_str());
	XMLDoc.parse<0>(scene);

	// Grab the scene node
	XMLRoot = XMLDoc.first_node("scene");

	// Validate the File
	if (getAttrib(XMLRoot, "formatVersion", "") == "")
	{
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error: Invalid .scene File. Missing <scene>");
		delete scene;
		return;
	}

	// figure out where to attach any nodes we create
	mAttachNode = pAttachNode;
	if (!mAttachNode)
	{
		mAttachNode = mSceneMgr->getRootSceneNode();
	}

	// Process the scene
	processScene(XMLRoot);

	delete scene;
}

void DotSceneLoader::processScene(xml_node<>* XMLRoot)
{
	// Process the scene parameters
	String version = getAttrib(XMLRoot, "formatVersion", "unknown");

	String message = "[DotSceneLoader] Parsing dotScene file with version " + version;
	if (XMLRoot->first_attribute("ID"))
	{
		message += ", id " + String(XMLRoot->first_attribute("ID")->value());
	}
	if (XMLRoot->first_attribute("sceneManager"))
	{
		message += ", scene manager " + String(XMLRoot->first_attribute("sceneManager")->value());
	}
	if (XMLRoot->first_attribute("minOgreVersion"))
	{
		message += ", min. Ogre version " + String(XMLRoot->first_attribute("minOgreVersion")->value());
	}
	if (XMLRoot->first_attribute("author"))
	{
		message += ", author " + String(XMLRoot->first_attribute("author")->value());
	}

	LogManager::getSingleton().logMessage(message);

	xml_node<>* pElement;

	// Process environment (?)
	pElement = XMLRoot->first_node("environment");
	if (pElement)
	{
		processEnvironment(pElement);
	}

	// Process nodes (?)
	pElement = XMLRoot->first_node("nodes");
	if (pElement)
	{
		processNodes(pElement);
	}

	// Process externals (?)
	pElement = XMLRoot->first_node("externals");
	if (pElement)
	{
		processExternals(pElement);
	}

	// Process userDataReference (?)
	pElement = XMLRoot->first_node("userDataReference");
	if (pElement)
	{
		processUserDataReference(pElement);
	}

	// Process octree (?)
	pElement = XMLRoot->first_node("octree");
	if (pElement)
	{
		processOctree(pElement);
	}

	// Process light (?)
	//pElement = XMLRoot->first_node("light");
	//if(pElement)
	//    processLight(pElement);

	// Process camera (?)
	pElement = XMLRoot->first_node("camera");
	if (pElement)
	{
		processCamera(pElement);
	}

	// Process terrain (?)
	pElement = XMLRoot->first_node("terrain");
	if (pElement)
	{
		processTerrain(pElement);
	}
}

void DotSceneLoader::processNodes(xml_node<>* XMLNode)
{
	xml_node<>* pElement;

	// Process node (*)
	pElement = XMLNode->first_node("node");
	while (pElement)
	{
		processNode(pElement);
		pElement = pElement->next_sibling("node");
	}

	// Process position (?)
	pElement = XMLNode->first_node("position");
	if (pElement)
	{
		mAttachNode->setPosition(parseVector3(pElement));
		mAttachNode->setInitialState();
	}

	// Process rotation (?)
	pElement = XMLNode->first_node("rotation");
	if (pElement)
	{
		mAttachNode->setOrientation(parseQuaternion(pElement));
		mAttachNode->setInitialState();
	}

	// Process scale (?)
	pElement = XMLNode->first_node("scale");
	if (pElement)
	{
		mAttachNode->setScale(parseVector3(pElement));
		mAttachNode->setInitialState();
	}
}

void DotSceneLoader::processExternals(xml_node<>* XMLNode)
{
	//! @todo Implement this
}

void DotSceneLoader::processEnvironment(xml_node<>* XMLNode)
{
	xml_node<>* pElement;

	// Process camera (?)
	pElement = XMLNode->first_node("camera");
	if (pElement)
	{
		processCamera(pElement);
	}

	// Process fog (?)
	pElement = XMLNode->first_node("fog");
	if (pElement)
	{
		processFog(pElement);
	}

	// Process skyBox (?)
	pElement = XMLNode->first_node("skyBox");
	if (pElement)
	{
		processSkyBox(pElement);
	}

	// Process skyDome (?)
	pElement = XMLNode->first_node("skyDome");
	if (pElement)
	{
		processSkyDome(pElement);
	}

	// Process skyPlane (?)
	pElement = XMLNode->first_node("skyPlane");
	if (pElement)
	{
		processSkyPlane(pElement);
	}

	// Process clipping (?)
	pElement = XMLNode->first_node("clipping");
	if (pElement)
	{
		processClipping(pElement);
	}

	// Process colourAmbient (?)
	pElement = XMLNode->first_node("colourAmbient");
	if (pElement)
	{
		mSceneMgr->setAmbientLight(parseColour(pElement));
	}

	// Process colourBackground (?)
	//! @todo Set the background colour of all viewports (RenderWindow has to be provided then)
	pElement = XMLNode->first_node("colourBackground");
	if (pElement)
	{
		//mSceneMgr->set(parseColour(pElement));
	}

	// Process userDataReference (?)
	pElement = XMLNode->first_node("userDataReference");
	if (pElement)
	{
		processUserDataReference(pElement);
	}
}

void DotSceneLoader::processTerrain(xml_node<>* XMLNode)
{
	Real worldSize = getAttribReal(XMLNode, "worldSize");
	int mapSize = StringConverter::parseInt(XMLNode->first_attribute("mapSize")->value());
	bool colourmapEnabled = getAttribBool(XMLNode, "colourmapEnabled");
	int colourMapTextureSize = StringConverter::parseInt(XMLNode->first_attribute("colourMapTextureSize")->value());
	int compositeMapDistance = StringConverter::parseInt(XMLNode->first_attribute("tuningCompositeMapDistance")->value());
	int maxPixelError = StringConverter::parseInt(XMLNode->first_attribute("tuningMaxPixelError")->value());

	Vector3 lightdir(0, -0.3, 0.75);
	lightdir.normalise();
	Light* lt = mSceneMgr->createLight("tstLight");
	lt->setType(Light::LT_DIRECTIONAL);
	lt->setDirection(lightdir);
	lt->setDiffuseColour(ColourValue(1.0, 1.0, 1.0));
	lt->setSpecularColour(ColourValue(0.4, 0.4, 0.4));
	mSceneMgr->setAmbientLight(ColourValue(0.6, 0.6, 0.6));

	mTerrainGlobalOptions->setMaxPixelError((Real)maxPixelError);
	mTerrainGlobalOptions->setCompositeMapDistance((Real)compositeMapDistance);
	mTerrainGlobalOptions->setLightMapDirection(lightdir);
	mTerrainGlobalOptions->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
	mTerrainGlobalOptions->setCompositeMapDiffuse(lt->getDiffuseColour());

	mTerrainGroup = OGRE_NEW TerrainGroup(mSceneMgr, Terrain::ALIGN_X_Z, mapSize, worldSize);
	mTerrainGroup->setOrigin(Vector3::ZERO);

	mTerrainGroup->setResourceGroup("General");

	xml_node<>* pElement;
	xml_node<>* pPageElement;

	// Process terrain pages (*)
	pElement = XMLNode->first_node("terrainPages");
	if (pElement)
	{
		pPageElement = pElement->first_node("terrainPage");
		while (pPageElement)
		{
			processTerrainPage(pPageElement);
			pPageElement = pPageElement->next_sibling("terrainPage");
		}
	}
	mTerrainGroup->loadAllTerrains(true);

	mTerrainGroup->freeTemporaryResources();
	//mTerrain->setPosition(mTerrainPosition);
}

void DotSceneLoader::processTerrainPage(xml_node<>* XMLNode)
{
	String name = getAttrib(XMLNode, "name");
	int pageX = StringConverter::parseInt(XMLNode->first_attribute("pageX")->value());
	int pageY = StringConverter::parseInt(XMLNode->first_attribute("pageY")->value());

	if (ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), name))
	{
		mTerrainGroup->defineTerrain(pageX, pageY, name);
	}
}

void DotSceneLoader::processUserDataReference(xml_node<>* XMLNode, SceneNode *pParent)
{
	//! @todo Implement this
}

void DotSceneLoader::processOctree(xml_node<>* XMLNode)
{
	//! @todo Implement this
}

void DotSceneLoader::processLight(xml_node<>* XMLNode, SceneNode *pParent)
{
	// Process attributes
	String name = getAttrib(XMLNode, "name");
	String id = getAttrib(XMLNode, "id");

	// Create the light
	Light *pLight = mSceneMgr->createLight(name);
	if (pParent)
	{
		pParent->attachObject(pLight);
	}

	String sValue = getAttrib(XMLNode, "type");
	if (sValue == "point")
	{
		pLight->setType(Light::LT_POINT);
	}
	else if (sValue == "directional")
	{
		pLight->setType(Light::LT_DIRECTIONAL);
	}
	else if (sValue == "spot")
	{
		pLight->setType(Light::LT_SPOTLIGHT);
	}
	else if (sValue == "radPoint")
	{
		pLight->setType(Light::LT_POINT);
	}

	pLight->setVisible(getAttribBool(XMLNode, "visible", true));
	pLight->setCastShadows(getAttribBool(XMLNode, "castShadows", true));

	xml_node<>* pElement;

	// Process position (?)
	pElement = XMLNode->first_node("position");
	if (pElement)
	{
		pLight->setPosition(parseVector3(pElement));
	}

	// Process normal (?)
	pElement = XMLNode->first_node("normal");
	if (pElement)
	{
		pLight->setDirection(parseVector3(pElement));
	}

	pElement = XMLNode->first_node("directionVector");
	if (pElement)
	{
		pLight->setDirection(parseVector3(pElement));
		mLightDirection = parseVector3(pElement);
	}

	// Process colourDiffuse (?)
	pElement = XMLNode->first_node("colourDiffuse");
	if (pElement)
	{
		pLight->setDiffuseColour(parseColour(pElement));
	}

	// Process colourSpecular (?)
	pElement = XMLNode->first_node("colourSpecular");
	if (pElement)
	{
		pLight->setSpecularColour(parseColour(pElement));
	}

	if (sValue != "directional")
	{
		// Process lightRange (?)
		pElement = XMLNode->first_node("lightRange");
		if (pElement)
		{
			processLightRange(pElement, pLight);
		}

		// Process lightAttenuation (?)
		pElement = XMLNode->first_node("lightAttenuation");
		if (pElement)
		{
			processLightAttenuation(pElement, pLight);
		}
	}
	// Process userDataReference (?)
	pElement = XMLNode->first_node("userDataReference");
	if (pElement)
	{
		//processUserDataReference(pElement, pLight);
	}
}

void DotSceneLoader::processCamera(xml_node<>* XMLNode, SceneNode *pParent)
{
	// Process attributes
	String name = getAttrib(XMLNode, "name");
	String id = getAttrib(XMLNode, "id");
	Real fov = getAttribReal(XMLNode, "fov", 45);
	Real aspectRatio = getAttribReal(XMLNode, "aspectRatio", 1.3333);
	String projectionType = getAttrib(XMLNode, "projectionType", "perspective");

	// Create the camera
	Camera *pCamera = mSceneMgr->createCamera(name);

	//TODO: make a flag or attribute indicating whether or not the camera should be attached to any parent node.
	//if(pParent)
	//    pParent->attachObject(pCamera);

	// Set the field-of-view
	//! @todo Is this always in degrees?
	//pCamera->setFOVy(Degree(fov));

	// Set the aspect ratio
	//pCamera->setAspectRatio(aspectRatio);

	// Set the projection type
	if (projectionType == "perspective")
	{
		pCamera->setProjectionType(PT_PERSPECTIVE);
	}
	else if (projectionType == "orthographic")
	{
		pCamera->setProjectionType(PT_ORTHOGRAPHIC);
	}

	xml_node<>* pElement;

	// Process clipping (?)
	pElement = XMLNode->first_node("clipping");
	if (pElement)
	{
		Real nearDist = getAttribReal(pElement, "near");
		pCamera->setNearClipDistance(nearDist);

		Real farDist = getAttribReal(pElement, "far");
		pCamera->setFarClipDistance(farDist);
	}

	// Process position (?)
	pElement = XMLNode->first_node("position");
	if (pElement)
	{
		pCamera->setPosition(parseVector3(pElement));
	}

	// Process rotation (?)
	pElement = XMLNode->first_node("rotation");
	if (pElement)
	{
		pCamera->setOrientation(parseQuaternion(pElement));
	}

	// Process normal (?)
	pElement = XMLNode->first_node("normal");
	if (pElement)
	{
		//!< @todo What to do with this element?
	}

	// Process lookTarget (?)
	pElement = XMLNode->first_node("lookTarget");
	if (pElement)
	{
		//!< @todo Implement the camera look target
	}

	// Process trackTarget (?)
	pElement = XMLNode->first_node("trackTarget");
	if (pElement)
	{
		//!< @todo Implement the camera track target
	}

	// Process userDataReference (?)
	pElement = XMLNode->first_node("userDataReference");
	if (pElement)
	{
		//!< @todo Implement the camera user data reference
	}

	// construct a scenenode is no parent
	if (!pParent)
	{
		SceneNode* pNode = mAttachNode->createChildSceneNode(name);
		pNode->setPosition(pCamera->getPosition());
		pNode->setOrientation(pCamera->getOrientation());
		pNode->scale(1, 1, 1);
	}
}

void DotSceneLoader::processNode(xml_node<>* XMLNode, SceneNode *pParent)
{
	// Construct the node's name
	String name = m_sPrependNode + getAttrib(XMLNode, "name");

	// Create the scene node
	SceneNode *pNode;
	if (name.empty())
	{
		// Let Ogre choose the name
		if (pParent)
		{
			pNode = pParent->createChildSceneNode();
		}
		else
		{
			pNode = mAttachNode->createChildSceneNode();
		}
	}
	else
	{
		// Provide the name
		if (pParent)
		{
			pNode = pParent->createChildSceneNode(name);
		}
		else
		{
			pNode = mAttachNode->createChildSceneNode(name);
		}
	}

	// Process other attributes
	String id = getAttrib(XMLNode, "id");
	bool isTarget = getAttribBool(XMLNode, "isTarget");

	xml_node<>* pElement;

	// Process position (?)
	pElement = XMLNode->first_node("position");
	if (pElement)
	{
		pNode->setPosition(parseVector3(pElement));
		pNode->setInitialState();
	}

	// Process rotation (?)
	pElement = XMLNode->first_node("rotation");
	if (pElement)
	{
		pNode->setOrientation(parseQuaternion(pElement));
		pNode->setInitialState();
	}

	// Process scale (?)
	pElement = XMLNode->first_node("scale");
	if (pElement)
	{
		pNode->setScale(parseVector3(pElement));
		pNode->setInitialState();
	}

	// Process lookTarget (?)
	pElement = XMLNode->first_node("lookTarget");
	if (pElement)
	{
		processLookTarget(pElement, pNode);
	}

	// Process trackTarget (?)
	pElement = XMLNode->first_node("trackTarget");
	if (pElement)
	{
		processTrackTarget(pElement, pNode);
	}

	// Process node (*)
	pElement = XMLNode->first_node("node");
	while (pElement)
	{
		processNode(pElement, pNode);
		pElement = pElement->next_sibling("node");
	}

	// Process entity (*)
	pElement = XMLNode->first_node("entity");
	while (pElement)
	{
		processEntity(pElement, pNode);
		pElement = pElement->next_sibling("entity");
	}

	// Process light (*)
	//pElement = XMLNode->first_node("light");
	//while(pElement)
	//{
	//    processLight(pElement, pNode);
	//    pElement = pElement->next_sibling("light");
	//}

	// Process camera (*)
	pElement = XMLNode->first_node("camera");
	while (pElement)
	{
		processCamera(pElement, pNode);
		pElement = pElement->next_sibling("camera");
	}

	// Process particleSystem (*)
	pElement = XMLNode->first_node("particleSystem");
	while (pElement)
	{
		processParticleSystem(pElement, pNode);
		pElement = pElement->next_sibling("particleSystem");
	}

	// Process billboardSet (*)
	pElement = XMLNode->first_node("billboardSet");
	while (pElement)
	{
		processBillboardSet(pElement, pNode);
		pElement = pElement->next_sibling("billboardSet");
	}

	// Process plane (*)
	pElement = XMLNode->first_node("plane");
	while (pElement)
	{
		processPlane(pElement, pNode);
		pElement = pElement->next_sibling("plane");
	}

	// Process userDataReference (?)
	pElement = XMLNode->first_node("userDataReference");
	if (pElement)
	{
		processUserDataReference(pElement, pNode);
	}
}

void DotSceneLoader::processLookTarget(xml_node<>* XMLNode, SceneNode *pParent)
{
	//! @todo Is this correct? Cause I don't have a clue actually

	// Process attributes
	String nodeName = getAttrib(XMLNode, "nodeName");

	Node::TransformSpace relativeTo = Node::TS_PARENT;
	String sValue = getAttrib(XMLNode, "relativeTo");
	if (sValue == "local")
	{
		relativeTo = Node::TS_LOCAL;
	}
	else if (sValue == "parent")
	{
		relativeTo = Node::TS_PARENT;
	}
	else if (sValue == "world")
	{
		relativeTo = Node::TS_WORLD;
	}

	xml_node<>* pElement;

	// Process position (?)
	Vector3 position;
	pElement = XMLNode->first_node("position");
	if (pElement)
	{
		position = parseVector3(pElement);
	}

	// Process localDirection (?)
	Vector3 localDirection = Vector3::NEGATIVE_UNIT_Z;
	pElement = XMLNode->first_node("localDirection");
	if (pElement)
	{
		localDirection = parseVector3(pElement);
	}

	// Setup the look target
	try
	{
		if (!nodeName.empty())
		{
			SceneNode *pLookNode = mSceneMgr->getSceneNode(nodeName);
			position = pLookNode->_getDerivedPosition();
		}

		pParent->lookAt(position, relativeTo, localDirection);
	}
	catch (Exception &/*e*/)
	{
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a look target!");
	}
}

void DotSceneLoader::processTrackTarget(xml_node<>* XMLNode, SceneNode *pParent)
{
	// Process attributes
	String nodeName = getAttrib(XMLNode, "nodeName");

	xml_node<>* pElement;

	// Process localDirection (?)
	Vector3 localDirection = Vector3::NEGATIVE_UNIT_Z;
	pElement = XMLNode->first_node("localDirection");
	if (pElement)
	{
		localDirection = parseVector3(pElement);
	}

	// Process offset (?)
	Vector3 offset = Vector3::ZERO;
	pElement = XMLNode->first_node("offset");
	if (pElement)
	{
		offset = parseVector3(pElement);
	}

	// Setup the track target
	try
	{
		SceneNode *pTrackNode = mSceneMgr->getSceneNode(nodeName);
		pParent->setAutoTracking(true, pTrackNode, localDirection, offset);
	}
	catch (Exception &/*e*/)
	{
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a track target!");
	}
}

void DotSceneLoader::processEntity(xml_node<>* XMLNode, SceneNode *pParent)
{
	// Process attributes
	String name = getAttrib(XMLNode, "name");
	String id = getAttrib(XMLNode, "id");
	String meshFile = getAttrib(XMLNode, "meshFile");
	String materialFile = getAttrib(XMLNode, "materialFile");
	bool isStatic = getAttribBool(XMLNode, "static", false);;
	bool castShadows = getAttribBool(XMLNode, "castShadows", true);

	// TEMP: Maintain a list of static and dynamic objects
	if (isStatic)
	{
		staticObjects.push_back(name);
	}
	else
	{
		dynamicObjects.push_back(name);
	}

	xml_node<>* pElement;

	// Process vertexBuffer (?)
	pElement = XMLNode->first_node("vertexBuffer");
	if (pElement)
	{
		//processVertexBuffer(pElement);
	}

	// Process indexBuffer (?)
	pElement = XMLNode->first_node("indexBuffer");
	if (pElement)
	{
		//processIndexBuffer(pElement);
	}

	// Create the entity
	Entity *pEntity = 0;
	try
	{
		MeshManager::getSingleton().load(meshFile, m_sGroupName);
		pEntity = mSceneMgr->createEntity(name, meshFile);
		pEntity->setCastShadows(castShadows);
		pParent->attachObject(pEntity);

		if (!materialFile.empty())
		{
			pEntity->setMaterialName(materialFile);
		}
	}
	catch (Exception &/*e*/)
	{
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
	}

	// Process userDataReference (?)
	pElement = XMLNode->first_node("userDataReference");
	if (pElement)
	{
		processUserDataReference(pElement, pEntity);
	}
}

void DotSceneLoader::processParticleSystem(xml_node<>* XMLNode, SceneNode *pParent)
{
	// Process attributes
	String name = getAttrib(XMLNode, "name");
	String id = getAttrib(XMLNode, "id");
	String file = getAttrib(XMLNode, "file");

	// Create the particle system
	try
	{
		ParticleSystem *pParticles = mSceneMgr->createParticleSystem(name, file);
		pParent->attachObject(pParticles);
	}
	catch (Exception &/*e*/)
	{
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error creating a particle system!");
	}
}

void DotSceneLoader::processBillboardSet(xml_node<>* XMLNode, SceneNode *pParent)
{
	//! @todo Implement this
}

void DotSceneLoader::processPlane(xml_node<>* XMLNode, SceneNode *pParent)
{
	String name = getAttrib(XMLNode, "name");
	Real distance = getAttribReal(XMLNode, "distance");
	Real width = getAttribReal(XMLNode, "width");
	Real height = getAttribReal(XMLNode, "height");
	int xSegments = StringConverter::parseInt(getAttrib(XMLNode, "xSegments"));
	int ySegments = StringConverter::parseInt(getAttrib(XMLNode, "ySegments"));
	int numTexCoordSets = StringConverter::parseInt(getAttrib(XMLNode, "numTexCoordSets"));
	Real uTile = getAttribReal(XMLNode, "uTile");
	Real vTile = getAttribReal(XMLNode, "vTile");
	String material = getAttrib(XMLNode, "material");
	bool hasNormals = getAttribBool(XMLNode, "hasNormals");
	Vector3 normal = parseVector3(XMLNode->first_node("normal"));
	Vector3 up = parseVector3(XMLNode->first_node("upVector"));

	Plane plane(normal, distance);
	MeshPtr res = MeshManager::getSingletonPtr()->createPlane( name + "mesh", "General", plane, width, height, xSegments, ySegments, hasNormals, numTexCoordSets, uTile, vTile, up);
	Entity* ent = mSceneMgr->createEntity(name, name + "mesh");

	ent->setMaterialName(material);

	pParent->attachObject(ent);
}

void DotSceneLoader::processFog(xml_node<>* XMLNode)
{
	// Process attributes
	Real expDensity = getAttribReal(XMLNode, "density", 0.001);
	Real linearStart = getAttribReal(XMLNode, "start", 0.0);
	Real linearEnd = getAttribReal(XMLNode, "end", 1.0);

	FogMode mode = FOG_NONE;
	String sMode = getAttrib(XMLNode, "mode");
	if (sMode == "none")
	{
		mode = FOG_NONE;
	}
	else if (sMode == "exp")
	{
		mode = FOG_EXP;
	}
	else if (sMode == "exp2")
	{
		mode = FOG_EXP2;
	}
	else if (sMode == "linear")
	{
		mode = FOG_LINEAR;
	}
	else
	{
		mode = (FogMode)StringConverter::parseInt(sMode);
	}

	xml_node<>* pElement;

	// Process colourDiffuse (?)
	ColourValue colourDiffuse = ColourValue::White;
	pElement = XMLNode->first_node("colour");
	if (pElement)
	{
		colourDiffuse = parseColour(pElement);
	}

	// Setup the fog
	mSceneMgr->setFog(mode, colourDiffuse, expDensity, linearStart, linearEnd);
}

void DotSceneLoader::processSkyBox(xml_node<>* XMLNode)
{
	// Process attributes
	String material = getAttrib(XMLNode, "material", "BaseWhite");
	Real distance = getAttribReal(XMLNode, "distance", 5000);
	bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);
	bool active = getAttribBool(XMLNode, "active", false);
	if (!active)
	{
		return;
	}

	xml_node<>* pElement;

	// Process rotation (?)
	Quaternion rotation = Quaternion::IDENTITY;
	pElement = XMLNode->first_node("rotation");
	if (pElement)
	{
		rotation = parseQuaternion(pElement);
	}

	// Setup the sky box
	mSceneMgr->setSkyBox(true, material, distance, drawFirst, rotation, m_sGroupName);
}

void DotSceneLoader::processSkyDome(xml_node<>* XMLNode)
{
	// Process attributes
	String material = XMLNode->first_attribute("material")->value();
	Real curvature = getAttribReal(XMLNode, "curvature", 10);
	Real tiling = getAttribReal(XMLNode, "tiling", 8);
	Real distance = getAttribReal(XMLNode, "distance", 4000);
	bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);
	bool active = getAttribBool(XMLNode, "active", false);
	if (!active)
	{
		return;
	}

	xml_node<>* pElement;

	// Process rotation (?)
	Quaternion rotation = Quaternion::IDENTITY;
	pElement = XMLNode->first_node("rotation");
	if (pElement)
	{
		rotation = parseQuaternion(pElement);
	}

	// Setup the sky dome
	mSceneMgr->setSkyDome(true, material, curvature, tiling, distance, drawFirst, rotation, 16, 16, -1, m_sGroupName);
}

void DotSceneLoader::processSkyPlane(xml_node<>* XMLNode)
{
	// Process attributes
	String material = getAttrib(XMLNode, "material");
	Real planeX = getAttribReal(XMLNode, "planeX", 0);
	Real planeY = getAttribReal(XMLNode, "planeY", -1);
	Real planeZ = getAttribReal(XMLNode, "planeX", 0);
	Real planeD = getAttribReal(XMLNode, "planeD", 5000);
	Real scale = getAttribReal(XMLNode, "scale", 1000);
	Real bow = getAttribReal(XMLNode, "bow", 0);
	Real tiling = getAttribReal(XMLNode, "tiling", 10);
	bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);

	// Setup the sky plane
	Plane plane;
	plane.normal = Vector3(planeX, planeY, planeZ);
	plane.d = planeD;
	mSceneMgr->setSkyPlane(true, plane, material, scale, tiling, drawFirst, bow, 1, 1, m_sGroupName);
}

void DotSceneLoader::processClipping(xml_node<>* XMLNode)
{
	//! @todo Implement this

	// Process attributes
	Real fNear = getAttribReal(XMLNode, "near", 0);
	Real fFar = getAttribReal(XMLNode, "far", 1);
}

void DotSceneLoader::processLightRange(xml_node<>* XMLNode, Light *pLight)
{
	// Process attributes
	Real inner = getAttribReal(XMLNode, "inner");
	Real outer = getAttribReal(XMLNode, "outer");
	Real falloff = getAttribReal(XMLNode, "falloff", 1.0);

	// Setup the light range
	pLight->setSpotlightRange(Angle(inner), Angle(outer), falloff);
}

void DotSceneLoader::processLightAttenuation(xml_node<>* XMLNode, Light *pLight)
{
	// Process attributes
	Real range = getAttribReal(XMLNode, "range");
	Real constant = getAttribReal(XMLNode, "constant");
	Real linear = getAttribReal(XMLNode, "linear");
	Real quadratic = getAttribReal(XMLNode, "quadratic");

	// Setup the light attenuation
	pLight->setAttenuation(range, constant, linear, quadratic);
}


String DotSceneLoader::getAttrib(xml_node<>* XMLNode, const String &attrib, const String &defaultValue)
{
	if (XMLNode->first_attribute(attrib.c_str()))
	{
		return XMLNode->first_attribute(attrib.c_str())->value();
	}
	else
	{
		return defaultValue;
	}
}

Real DotSceneLoader::getAttribReal(xml_node<>* XMLNode, const String &attrib, Real defaultValue)
{
	if (XMLNode->first_attribute(attrib.c_str()))
	{
		return StringConverter::parseReal(XMLNode->first_attribute(attrib.c_str())->value());
	}
	else
	{
		return defaultValue;
	}
}

bool DotSceneLoader::getAttribBool(xml_node<>* XMLNode, const String &attrib, bool defaultValue)
{
	if (!XMLNode->first_attribute(attrib.c_str()))
	{
		return defaultValue;
	}

	if (String(XMLNode->first_attribute(attrib.c_str())->value()) == "true")
	{
		return true;
	}

	return false;
}

Vector3 DotSceneLoader::parseVector3(xml_node<>* XMLNode)
{
	return Vector3(
		StringConverter::parseReal(XMLNode->first_attribute("x")->value()),
		StringConverter::parseReal(XMLNode->first_attribute("y")->value()),
		StringConverter::parseReal(XMLNode->first_attribute("z")->value())
		);
}

Quaternion DotSceneLoader::parseQuaternion(xml_node<>* XMLNode)
{
	//! @todo Fix this crap!

	Quaternion orientation;

	if (XMLNode->first_attribute("qx"))
	{
		orientation.x = StringConverter::parseReal(XMLNode->first_attribute("qx")->value());
		orientation.y = StringConverter::parseReal(XMLNode->first_attribute("qy")->value());
		orientation.z = StringConverter::parseReal(XMLNode->first_attribute("qz")->value());
		orientation.w = StringConverter::parseReal(XMLNode->first_attribute("qw")->value());
	}
	if (XMLNode->first_attribute("qw"))
	{
		orientation.w = StringConverter::parseReal(XMLNode->first_attribute("qw")->value());
		orientation.x = StringConverter::parseReal(XMLNode->first_attribute("qx")->value());
		orientation.y = StringConverter::parseReal(XMLNode->first_attribute("qy")->value());
		orientation.z = StringConverter::parseReal(XMLNode->first_attribute("qz")->value());
	}
	else if (XMLNode->first_attribute("axisX"))
	{
		Vector3 axis;
		axis.x = StringConverter::parseReal(XMLNode->first_attribute("axisX")->value());
		axis.y = StringConverter::parseReal(XMLNode->first_attribute("axisY")->value());
		axis.z = StringConverter::parseReal(XMLNode->first_attribute("axisZ")->value());
		Real angle = StringConverter::parseReal(XMLNode->first_attribute("angle")->value());;
		orientation.FromAngleAxis(Angle(angle), axis);
	}
	else if (XMLNode->first_attribute("angleX"))
	{
		Vector3 axis;
		axis.x = StringConverter::parseReal(XMLNode->first_attribute("angleX")->value());
		axis.y = StringConverter::parseReal(XMLNode->first_attribute("angleY")->value());
		axis.z = StringConverter::parseReal(XMLNode->first_attribute("angleZ")->value());
		//orientation.FromAxes(&axis);
		//orientation.F
	}
	else if (XMLNode->first_attribute("x"))
	{
		orientation.x = StringConverter::parseReal(XMLNode->first_attribute("x")->value());
		orientation.y = StringConverter::parseReal(XMLNode->first_attribute("y")->value());
		orientation.z = StringConverter::parseReal(XMLNode->first_attribute("z")->value());
		orientation.w = StringConverter::parseReal(XMLNode->first_attribute("w")->value());
	}
	else if (XMLNode->first_attribute("w"))
	{
		orientation.w = StringConverter::parseReal(XMLNode->first_attribute("w")->value());
		orientation.x = StringConverter::parseReal(XMLNode->first_attribute("x")->value());
		orientation.y = StringConverter::parseReal(XMLNode->first_attribute("y")->value());
		orientation.z = StringConverter::parseReal(XMLNode->first_attribute("z")->value());
	}

	return orientation;
}

ColourValue DotSceneLoader::parseColour(xml_node<>* XMLNode)
{
	return ColourValue(
		StringConverter::parseReal(XMLNode->first_attribute("r")->value()),
		StringConverter::parseReal(XMLNode->first_attribute("g")->value()),
		StringConverter::parseReal(XMLNode->first_attribute("b")->value()),
		XMLNode->first_attribute("a") != NULL ? StringConverter::parseReal(XMLNode->first_attribute("a")->value()) : 1
		);
}

String DotSceneLoader::getProperty(const String &ndNm, const String &prop)
{
	for (unsigned int i = 0; i < nodeProperties.size(); i++)
	{
		if (nodeProperties[i].nodeName == ndNm && nodeProperties[i].propertyNm == prop)
		{
			return nodeProperties[i].valueName;
		}
	}

	return "";
}

void DotSceneLoader::processUserDataReference(xml_node<>* XMLNode, Entity *pEntity)
{
	String str = XMLNode->first_attribute("id")->value();
	pEntity->setUserAny(Any(str));
}