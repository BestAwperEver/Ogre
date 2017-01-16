#include "myapp.h"
//#include "RobotFactory.h"
//#include "SelectionBox.h"
//#include "SelectedGroup.h"
#include "PlayerCamera.h"
#include "AbstractUnit.h"
//#include "OgreConsoleM.h"
#include "MyOverlayManager.h"
#include "GameMap.h"
#include "MapBinManager.h"
#include "MapObject.h"
//#include "tcp_connection.h"
#include "HeroFactory.h"
#include "DynamicLine3D.h"
#include "LightMap.h"
#include "Logger.h"
#include <boost/lexical_cast.hpp>
//#include "EntityMaterialInstance.h"
//class AbstractUnit;
//#include <OgreRectangle2D.h>
//void myapp::initMConsole() {
//	m_pMConsole = new OgreConsoleM;
//	m_pMConsole->init(m_pRoot);
//	m_pMConsole->setVisible(true);
//}

//void myapp::initCEGUI(void) {
//	m_pRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

//	CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
//	//CEGUI::Font::setDefaultResourceGroup("Fonts");
//	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
//	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
//	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
//
//	CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
//	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
//	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setImage("TaharezLook/MouseArrow");
//	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().show();
//}

// не нужны
//inline Ogre::Real fmax(Ogre::Real z) {
//	return (z - 991)*(-150.0/1977)+755;
//}
//inline Ogre::Real fmin(Ogre::Real z) {
//	return (z - 991)*(150.0/1977)-755;
//}

#define INT(x) *( (int*)(&x) )
#define SHORT(x) *( (short*)(&x) )
#define UD(x) *( (UnitDef*)(&x) )

namespace Ogre {

	// опасная функция, выделяет память в vertices и indices
	void getMeshInformation(const Ogre::MeshPtr const mesh,
		size_t &vertex_count,
		Ogre::Vector3* &vertices,
		size_t &index_count,
		unsigned long* &indices,
		const Ogre::Vector3 &position = Ogre::Vector3::ZERO,
		const Ogre::Quaternion &orient = Ogre::Quaternion::IDENTITY,
		const Ogre::Vector3 &scale = Ogre::Vector3::UNIT_SCALE)
	{
		bool added_shared = false;
		size_t current_offset = 0;
		size_t shared_offset = 0;
		size_t next_offset = 0;
		size_t index_offset = 0;

		vertex_count = index_count = 0;

		// Calculate how many vertices and indices we're going to need
		for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
		{
			Ogre::SubMesh* submesh = mesh->getSubMesh(i);
			// We only need to add the shared vertices once
			if(submesh->useSharedVertices)
			{
				if( !added_shared )
				{
					vertex_count += mesh->sharedVertexData->vertexCount;
					added_shared = true;
				}
			}
			else
			{
				vertex_count += submesh->vertexData->vertexCount;
			}
			// Add the indices
			index_count += submesh->indexData->indexCount;
		}

		// Allocate space for the vertices and indices
		vertices = new Ogre::Vector3[vertex_count];
		indices = new unsigned long[index_count];

		added_shared = false;

		// Run through the submeshes again, adding the data into the arrays
		for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
		{
			Ogre::SubMesh* submesh = mesh->getSubMesh(i);

			Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

			if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
			{
				if(submesh->useSharedVertices)
				{
					added_shared = true;
					shared_offset = current_offset;
				}

				const Ogre::VertexElement* posElem =
					vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

				Ogre::HardwareVertexBufferSharedPtr vbuf =
					vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

				unsigned char* vertex =
					static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

				// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
				//  as second argument. So make it float, to avoid trouble when Ogre::Real will
				//  be comiled/typedefed as double:
				//Ogre::Real* pReal;
				float* pReal;

				for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
				{
					posElem->baseVertexPointerToElement(vertex, &pReal);
					Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);
					vertices[current_offset + j] = (orient * (pt * scale)) + position;
				}

				vbuf->unlock();
				next_offset += vertex_data->vertexCount;
			}

			Ogre::IndexData* index_data = submesh->indexData;
			size_t numTris = index_data->indexCount / 3;
			Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

			bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

			unsigned long* pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
			unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

			size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

			if ( use32bitindexes )
			{
				for ( size_t k = 0; k < numTris*3; ++k)
				{
					indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
				}
			}
			else
			{
				for ( size_t k = 0; k < numTris*3; ++k)
				{
					indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
						static_cast<unsigned long>(offset);
				}
			}

			ibuf->unlock();
			current_offset = next_offset;
		}
	}

	bool rayIntersectsEntity(const Ogre::Ray& ray, const Ogre::Entity* const ent) {

		size_t vertex_count;
		size_t index_count;
		Ogre::Vector3* vertices;
		unsigned long* indices;

		Ogre::getMeshInformation(ent->getMesh(), vertex_count, vertices, index_count, indices,
			ent->getParentNode()->_getDerivedPosition(),
			ent->getParentNode()->_getDerivedOrientation(),
			ent->getParentNode()->_getDerivedScale());

		for (int i = 0; i < static_cast<int>(index_count); i += 3)
		{
			// check for a hit against this triangle
			std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]],
				vertices[indices[i+1]], vertices[indices[i+2]], true, false);

			if (hit.first)
			{
				delete[] vertices;
				delete[] indices;

				return true;
			}
		}

		delete[] vertices;
		delete[] indices;

		return false;
	}
}

void assert_me(bool condition, Ogre::String message) {
	if (!condition) {
		_LOG("Assertion fault: " + message);
	}
#ifdef _USE_ASSERTS_
	assert(condition && message.c_str());
#endif
}

myapp::myapp(): m_pRoot(0),
	m_sPluginsCfg(Ogre::StringUtil::BLANK),
	m_sResourcesCfg(Ogre::StringUtil::BLANK),
	m_pViewport(0),
	m_pCamera(0),
	m_pSceneManager(0),
	m_pRenderWindow(0),
	m_pTrayManager(0),
	m_pCameraMan(0),
	m_pParamsPanels(0),
	//m_bCursorWasVisible(false),
	m_bShutDown(false),
	m_pInputManager(0),
	m_pOverlaySystem(0),
	m_pMouse(0),
	m_pKeyboard(0),
	//	m_pRenderer(0),
	m_bLMouseDown(false),
	m_bRMouseDown(false),
	//	m_pSelectionBox(0),
	m_Plane(),
	//	m_SelectedGroups(),
	//	m_pActiveSelectedGroup(0),
	m_pActiveUnit(0),
	//	m_pVolQuery(0),
	//rect(0),
	m_pGrid(0),
	m_bCalculateDeviations(false),
	m_pConsole(0),
	m_pGorilla(0),
	//m_pMConsole(0),
	m_pScreen(0),
	m_pMyOverlayManager(0),
	m_pGameMap(0),
	m_bDontMoveSelectedUnit(false),
	m_bDontSelectUnit(false),
	m_bActivityBlocked(false),
	m_mouseCellCoordinates(-1,-1),
	m_pPathLine(0),
	m_pMapBinManager(0),
	m_fScale(0.8f),
	m_fMinScale(0.5f),
	m_fMaxScale(2.0f),
	m_cursorType(Arrow),
	m_bFindPath(true),
	m_DefaultPort(27015),
	m_pConnection(nullptr),
	m_fDefaultRobotScale(1.3f),
	m_nActivePanelIndex(1),
	m_pUnitUnderMouseRay(0),
	m_CurrentActivePlayer(-1),
	m_bUnitInfoNeedsToBeUpdated(false),
	//m_bLogFileCreated(false),
	m_bNextDoing(false),
	m_pLightMap(0),
	m_pStaticGeometry(0),
	m_pEntGround(0),
	m_pTransparentGroundEntity(0),
	//m_pLightMapTexture(0),
	m_sCurMap("nomap"),
	m_CurMapID(0),
	m_pCurPlayer(nullptr),
	m_pRenderedFOV(0),
	m_DarkLine(0),
	m_LightLine(0),
	m_LightMapPixelPerTile(3)
	//m_bForceFindPath(false)
	//m_pObjectUnderMouseRay(nullptr)
{}
myapp::~myapp() {
	//if (m_pStaticGeometry) {
	//	delete m_pStaticGeometry;
	//	m_pStaticGeometry = 0;
	//} ибо сценманагер приберется
	m_GroundTexturePtr.setNull();
	m_DecalTexturePtr.setNull();
	m_MapMaterialPtr.setNull();
	m_LightMapTexturePtr.setNull();
	m_DecalMaterialPtr.setNull();

	m_Maps[0] = "testmap";

	m_Objects.clear();
	if (m_DarkLine) {
		delete[] m_DarkLine;
		m_DarkLine = 0;
	}
	if (m_LightLine) {
		delete[] m_LightLine;
		m_LightLine = 0;
	}
	if (m_pRenderedFOV) {
		delete m_pRenderedFOV;
		m_pRenderedFOV = 0;
	}
	if (m_pLightMap) {
		delete m_pLightMap;
		m_pLightMap = 0;
	}
	if (m_pGrid) {
		delete m_pGrid;
		m_pGrid = 0;
	}
	if (m_pGameMap) {
		delete m_pGameMap;
		m_pGameMap = 0;
	}
	if (m_pPathLine) {
		delete m_pPathLine;
		m_pPathLine = 0;
	}
	if (m_pTrayManager) {
		delete m_pTrayManager;
		m_pTrayManager = 0;
	}
	if (m_pParamsPanels) {
		delete[] m_pParamsPanels;
		m_pParamsPanels = 0;
	}
	if (m_pCameraMan) {
		delete m_pCameraMan;
		m_pCameraMan = 0;
	}
	if (m_pOverlaySystem) {
		delete m_pOverlaySystem;
		m_pOverlaySystem = 0;
	}
	//if (m_pSelectionBox) {
	//	delete m_pSelectionBox;
	//	m_pSelectionBox = 0;
	//}
	////////////////////// ТОЛЬКО ДЛЯ ТЕСТИРОВАНИЯ
	//if (m_pActiveSelectedGroup) {
	//	delete m_pActiveSelectedGroup;
	//	m_pActiveSelectedGroup = 0;
	//}
	//////////////////////
	//if (rect) {
	//	delete rect;
	//	rect = 0;
	//}
	if (m_pConsole) {
		delete m_pConsole;
		m_pConsole = 0;
	}
	if (m_pGorilla) {
		delete m_pGorilla;
		m_pGorilla = 0;
	}
	if (m_pMyOverlayManager) {
		delete m_pMyOverlayManager;
		m_pMyOverlayManager = 0;
	}
	if (m_pMapBinManager) {
		delete m_pMapBinManager;
		m_pMapBinManager = 0;
	}
	//#ifdef ENABLE_GAME_LOG
	//	if (log_file.is_open()) {
	//		log_file.close();
	//	}
	//#endif

	Ogre::WindowEventUtilities::removeWindowEventListener(m_pRenderWindow, this);
	//m_pSceneManager->destroyQuery(m_pVolQuery);
	windowClosed(m_pRenderWindow);

	delete m_pRoot;
	m_pRoot = 0;
}

bool myapp::setup() {

	m_pRoot = new Ogre::Root(m_sPluginsCfg);

	setupResources();

	if (!configure()) {
		return false;
	}

	createRenderSystemListener();

	chooseSceneManager();
	initFactories();
	createCamera();
	createViewports();

	initGorilla();

	initConsole();

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	createResourceListener();
	loadResources();

	//initCEGUI();

	//initMConsole();

	createFrameListener();

	//start_connection();

	createOverlay();

	loadStandartObjects();

	createScene();

	//// костыль мод он
	//mRayScnQuery = mSceneMgr->createRayQuery(Ogre::Ray())
	//// костыль мод офф

	return true;

}
bool myapp::start() {

#ifdef _DEBUG
	m_sPluginsCfg = "plugins_d.cfg";
	m_sResourcesCfg = "resources_d.cfg";
#else
	m_sPluginsCfg = "plugins.cfg";
	m_sResourcesCfg = "resources.cfg";
#endif

	if (!setup()) {
		return false;
	}

	//while(true)
	//{
	//	// Pump window messages for nice behaviour
	//	Ogre::WindowEventUtilities::messagePump();
	//
	//	if(m_pRenderWindow->isClosed())
	//	{
	//		break;
	//	}
	//
	//	// Render a frame
	//	if(!m_pRoot->renderOneFrame()) 
	//	{
	//		return false;
	//	}
	//}

	m_pRoot->startRendering();

	return true;
}
void myapp::setupResources() {

	m_pMapBinManager = new MapBinManager();
	//Ogre::ResourceGroupManager::getSingleton().declareResource("testmap.bin", "MapBin");

	Ogre::ConfigFile cf;
	cf.load(m_sResourcesCfg);

	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, TypeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			TypeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, TypeName, secName);
		}
	}
}
bool myapp::configure() {
	if (m_pRoot->restoreConfig() || m_pRoot->showConfigDialog()) {

		/*
		You can use Root::getAvailableRenderers() to find out which RenderSystems are available for your application to use. 
		Once you have retrieved a RenderSystem, you can use the RenderSystem::getConfigOptions to see what options are available for the user. 
		By combining these two function calls, you can create your own config dialog for your application.
		*/

		m_pRenderWindow = m_pRoot->initialise(true, "Ololo");

		/*
		Alternatively, you can create a render window yourself using the win32 API, wxWidgets, or one of the many other Windows or Linux GUI systems. 
		A quick example of how to do this under Windows would look something like this: 
		// Do not add this to the application
		mRoot->initialise(false);
		HWND hWnd = 0;  // Get the hWnd of the application!
		NameValuePairList misc;
		misc["externalWindowHandle"] = StringConverter::toString((int)hWnd);
		RenderWindow *win = mRoot->createRenderWindow("Main RenderWindow", 800, 600, false, &misc);
		*/

		return true;
	}
	else {
		return false;
	}

	//// Do not add this to the application
	//Ogre::RenderSystem *rs = m_pRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem");
	//// or use "OpenGL Rendering Subsystem"
	//m_pRoot->setRenderSystem(rs);
	//rs->setConfigOption("Full Screen", "No");
	//rs->setConfigOption("Video Mode", "800 x 600 @ 32-bit colour");
}
void myapp::createRenderSystemListener() {
	m_pRoot->getRenderSystem()->addListener(this);
}
void myapp::chooseSceneManager() {
	m_pSceneManager = m_pRoot->createSceneManager("DefaultSceneManager"); //Ogre::ST_GENERIC
	m_pOverlaySystem = new Ogre::OverlaySystem();
	m_pSceneManager->addRenderQueueListener(m_pOverlaySystem);
}
void myapp::initFactories() {
	AbstractFactory::getInstance().configure(m_pSceneManager);//, "undefined", UNDEFINED_TYPE);
	//	RobotFactory::getInstance().configure(m_pSceneManager);
	HeroFactory::getInstance().configure(m_pSceneManager);
}
void myapp::setCameraScale(Ogre::Real _Scale) {

	if (_Scale < m_fMinScale) _Scale = m_fScale;
	else m_fScale = _Scale;

	int W = m_pRenderWindow->getWidth();
	int H = m_pRenderWindow->getHeight();

	Ogre::Matrix4 p = BuildScaledOrthoMatrix(W  / _Scale / -2.0f,
		W  / _Scale /  2.0f,
		H / _Scale / -2.0f,
		H / _Scale /  2.0f, 0, 3000);

	m_pCamera->setCustomProjectionMatrix(true, p);
}
void myapp::createCamera() {
	m_pCamera = m_pSceneManager->createCamera("PlayerCamera");

	//m_pCamera->setPosition( PLANE_CENTER_X - 450, 780, PLANE_CENTER_Z + 1110);
	//m_pCamera->lookAt(PLANE_CENTER_X-450,0,PLANE_CENTER_Z+810);

	m_pCamera->setProjectionType(Ogre::ProjectionType::PT_ORTHOGRAPHIC);
	//m_pCamera->setPosition( PLANE_CENTER_X - 450, 780, PLANE_CENTER_Z + 1110);
	//m_pCamera->lookAt(PLANE_CENTER_X-450,0,PLANE_CENTER_Z+450);
	setCameraScale(m_fScale);

	m_pCamera->setOrientation(Ogre::Quaternion(.88831, -.253778, .367963, .105117));
	//(0.886801, -0.259108, 0.367325, 0.107326));


	//m_pCamera->setDirection(-0.43, 0.32, 0.17);
	//m_pCamera->setOrientation(Ogre::Quaternion(0.04, 0, 0, 0));

	//m_pCamera->yaw(Ogre::Radian(3.1415926/4));
	//m_pCamera->pitch(Ogre::Radian(0.3));

	m_pCamera->setPosition( 1194, 780, 2489);

	m_pCamera->setNearClipDistance(1);
	//m_pCamera->setFarClipDistance(10000.0f);
	//	m_pCamera->setProjectionType(Ogre::PT_PERSPECTIVE);

	//	m_pCameraMan = new PlayerCamera(m_pCamera, 450, -450, 1110, -90);
	m_pCameraMan = new PlayerCamera(m_pCamera, 
		PLANE_CENTER_X + 1500,
		PLANE_CENTER_X - 800, 
		PLANE_CENTER_Z + 1500,//, 
		PLANE_CENTER_Z - 800);
	//												&myapp::updateMouseCellPosition);

	//m_pCameraMan = new PlayerCamera(m_pCamera, PLANE_CENTER_X + 1357,
	//											PLANE_CENTER_X - 800, 
	//											PLANE_CENTER_Z + 1357,//, 
	//											PLANE_CENTER_Z - 800);

}
void myapp::createViewports() {
	m_pViewport = m_pRenderWindow->addViewport(m_pCamera);
	m_pViewport->setBackgroundColour(Ogre::ColourValue(0,0,0));

	m_pCamera->setAspectRatio(Ogre::Real(m_pViewport->getActualWidth()) / Ogre::Real(m_pViewport->getActualHeight()));
}
void myapp::createFrameListener() {

	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	m_pRenderWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	m_pInputManager = OIS::InputManager::createInputSystem( pl );

	m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputManager->createInputObject( OIS::OISKeyboard, true ));
	m_pMouse = static_cast<OIS::Mouse*>(m_pInputManager->createInputObject( OIS::OISMouse, true ));

	//Fix for 1.9
	m_InputContext.mKeyboard = m_pKeyboard;
	m_InputContext.mMouse = m_pMouse;

	m_pMouse->setEventCallback(this);
	m_pKeyboard->setEventCallback(this);

	//Set initial mouse clipping size
	windowResized(m_pRenderWindow);

	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(m_pRenderWindow, this);

	//m_pTrayManager = new OgreBites::SdkTrayManager("InterfaceName", m_pRenderWindow, m_InputContext, this);

	m_pRoot->addFrameListener(this);
}
void myapp::createOverlay() {

	//Fix for 1.9
	m_pTrayManager = new OgreBites::SdkTrayManager("InterfaceName", m_pRenderWindow, m_InputContext, this);
	//m_pTrayManager = new OgreBites::SdkTrayManager("InterfaceName", m_pRenderWindow, m_pMouse, this);
	m_pTrayManager->showFrameStats(OgreBites::TL_BOTTOMLEFT);
	//m_pTrayManager->showLogo(OgreBites::TL_BOTTOMRIGHT);

	m_pMyOverlayManager = new MyOverlayManager(m_pTrayManager);

	m_pParamsPanels = new OgreBites::ParamsPanel* [PANELS_COUNT];

	Ogre::StringVector items;
	items.push_back("cam.pX");
	items.push_back("cam.pY");
	items.push_back("cam.pZ");
	items.push_back("------");
	items.push_back("cam.oW");
	items.push_back("cam.oX");
	items.push_back("cam.oY");
	items.push_back("cam.oZ");
	items.push_back("------");
	items.push_back("Filtering");
	items.push_back("Poly Mode");

	Ogre::StringVector items1;
	items1.push_back("intersection.x");
	items1.push_back("intersection.y");
	items1.push_back("intersection.z");
	items1.push_back("------");
	items1.push_back("cell");
	items1.push_back("cell visability");
	//	items1.push_back("robot cell");

	Ogre::StringVector items2;
	items2.push_back("Sended");
	items2.push_back("Recieved");
	items2.push_back("Total");
	items2.push_back("------");
	items2.push_back("Ping");

	Ogre::StringVector items3;
	items3.push_back("Player");
	items3.push_back("Name");
	items3.push_back("Race");
	//items3.push_back("Class");
	items3.push_back("Hit points");
	items3.push_back("Armor class");
	items3.push_back("Active points");
	items3.push_back("Weapon");
	items3.push_back("Armor");

	m_pParamsPanels[0] = m_pTrayManager->createParamsPanel(OgreBites::TL_TOPLEFT, "DetailsPanel", 220, items);
	m_pParamsPanels[1] = m_pTrayManager->createParamsPanel(OgreBites::TL_TOPLEFT, "Intersection", 220, items1);
	m_pParamsPanels[2] = m_pTrayManager->createParamsPanel(OgreBites::TL_TOPLEFT, "Traffic", 220, items2);
	m_pParamsPanels[3] = m_pTrayManager->createParamsPanel(OgreBites::TL_TOPLEFT, "Unit info", 220, items3);

	m_pParamsPanels[0]->setParamValue(9, "Bilinear");
	m_pParamsPanels[0]->setParamValue(10, "Solid");

	m_pParamsPanels[2]->setParamValue(0, "0 bytes");
	m_pParamsPanels[2]->setParamValue(1, "0 bytes");
	m_pParamsPanels[2]->setParamValue(2, "0 bytes");

	for (int i = 0; i < PANELS_COUNT; ++i) {
		if (i == m_nActivePanelIndex) {
			m_pTrayManager->moveWidgetToTray(m_pParamsPanels[i], OgreBites::TL_TOPLEFT, 0);
			m_pParamsPanels[i]->show();
		} else {
			m_pTrayManager->removeWidgetFromTray(m_pParamsPanels[i]);
			m_pParamsPanels[i]->hide();
		}
	}

	//m_pTextBox = m_pTrayManager->createTextBox(OgreBites::TrayLocation::TL_TOP, "MyConsole", "OloloConsole", 800, 100);
}
void myapp::createResourceListener(void) {

}
//bool myapp::createGameLogFile() {
//	log_file.open("Game.log");
//	if (!log_file.is_open()) {
//		_OGRELOG("Can't create Game.log file");
//		return false;
//	}
//	std::time_t t = time(nullptr);
//	std::tm* aTm = localtime(&t);
//	log_file << aTm->tm_mday << "." << std::setw(2) << std::setfill('0') << aTm->tm_mon+1
//		<< "." << aTm->tm_year+1900 << std::endl;
////	delete aTm;
//	return true;
//}

void myapp::initGorilla() {
	m_pGorilla = new Gorilla::Silverback();
	m_pGorilla->loadAtlas("console","Console");
	//m_pGorilla->loadAtlas("arial","Fonts");
	m_pScreen = m_pGorilla->createScreen(m_pViewport, "console");
}
void myapp::initConsole() {
	m_pConsole = new OgreConsole<myapp>(this, m_pScreen->getWidth() / 6 - 1);
	m_pConsole->init(m_pScreen);
	m_pConsole->setVisible(true);

	m_pConsole->addCommand("hello", &myapp::sayHello);
	m_pConsole->addCommand("why", &myapp::consoleWhy);
	m_pConsole->addCommand("quit", &myapp::shutDown);
	m_pConsole->addCommand("fullscreen", &myapp::fullScreen);
	m_pConsole->addCommand("printmap", &myapp::printMap);
	m_pConsole->addCommand("set_line_count", &myapp::setConsoleLineCount);
	m_pConsole->addCommand("slc", &myapp::setConsoleLineCount);
	m_pConsole->addCommand("set_resolution", &myapp::setResolution);
	m_pConsole->addCommand("sr", &myapp::setResolution);
	m_pConsole->addCommand("set_ortho_scale", &myapp::setOrthoScale);
	m_pConsole->addCommand("sos", &myapp::setOrthoScale);
	m_pConsole->addCommand("help", &myapp::consoleHelp);
	m_pConsole->addCommand("print_camera_orientation", &myapp::printCameraOrientation);
	m_pConsole->addCommand("pco", &myapp::printCameraOrientation);
	m_pConsole->addCommand("print_camera_direction", &myapp::printCameraDirection);
	m_pConsole->addCommand("pcd", &myapp::printCameraDirection);
	//m_pConsole->addCommand("set_robot_scale", &myapp::setRobotScale);
	//m_pConsole->addCommand("srs", &myapp::setRobotScale);
	m_pConsole->addCommand("connect", &myapp::consoleConnect);
	m_pConsole->addCommand("disconnect", &myapp::consoleDisconnect);
	m_pConsole->addCommand("run_service", &myapp::consoleRunService);
	m_pConsole->addCommand("unimportant_messages", &myapp::enableUnimportantMsgs);
	m_pConsole->addCommand("emulate_ping", &myapp::enableEmulatingPing);

	m_pConsole->addCommand("create_unit", &myapp::consoleCreateUnit);
	m_pConsole->addCommand("create_hero", &myapp::consoleCreateHero);
	m_pConsole->addCommand("turn_end", &myapp::consoleTurnEnd);
	m_pConsole->addCommand("start_game", &myapp::consoleGameStart);
	m_pConsole->addCommand("print_light_map", &myapp::printLightMap);
	m_pConsole->addCommand("create_lobby", &myapp::consoleCreateLobby);
	m_pConsole->addCommand("exit_lobby", &myapp::consoleExitLobby);
	m_pConsole->addCommand("show_players_in_lobby", &myapp::consoleShowPlayersInLobby);
	m_pConsole->addCommand("spil", &myapp::consoleShowPlayersInLobby);
	m_pConsole->addCommand("join_lobby", &myapp::consoleJoinLobby);
	m_pConsole->addCommand("players_in_lobby", &myapp::consolePlayersInLobby);
	m_pConsole->addCommand("refresh_player_list", &myapp::consolePlayerList);
	m_pConsole->addCommand("refresh_lobby_list", &myapp::consoleLobbyList);
	m_pConsole->addCommand("player_list", &myapp::consoleShowPlayerList);
	m_pConsole->addCommand("lobby_list", &myapp::consoleShowLobbyList);
	m_pConsole->addCommand("change_map", &myapp::consoleChangeMap);
	m_pConsole->addCommand("create_merk", &myapp::consoleCreateMerk);
	m_pConsole->addCommand("delete_merk", &myapp::consoleDeleteMerk);
	m_pConsole->addCommand("choose_merk", &myapp::consoleChooseMerk);
	m_pConsole->addCommand("change_weapon", &myapp::consoleChangeWeapon);
	m_pConsole->addCommand("merk_list", &myapp::consoleMerkList);
	m_pConsole->addCommand("refresh_merk_list", &myapp::consoleRefreshMerkList);
	m_pConsole->addCommand("exit_combat", &myapp::consoleExitGame);
	m_pConsole->addCommand("change_view_range", &myapp::consoleChangeViewRange);
	m_pConsole->addCommand("cvr", &myapp::consoleChangeViewRange);

	m_pConsole->addAlias("cnct", "connect 127.0.0.1 Radagast");
	m_pConsole->addAlias("crt", "create_lobby testmap3");
	m_pConsole->addAlias("strt", "start_game");
}

void myapp::eventOccurred(const Ogre::String& eventName, const Ogre::NameValuePairList* parameters) {
	if (!m_LightMapTexturePtr.isNull()) {
		if (eventName == "DeviceRestored") {
			redrawFOV(true);
		}
	}
}

void myapp::loadResources() {
	// initialise all resource groups
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	//m_DecalTextureHandle = Ogre::TextureManager::getSingleton().load(SELECT_DECAL_STRING, "General")->getHandle();
	m_DecalTexturePtr = Ogre::TextureManager::getSingleton().load(SELECT_DECAL_STRING, "General");
}
void myapp::loadStandartObjects() {

	MapObject::StandartObjects[MAP_OBJECT_INDEX::OgreHead] = MapObject(MAP_OBJECT_INDEX::OgreHead, OgreHeadMask,
		"ogrehead.mesh", "OgreHead",
		Ogre::Vector3(2.0f), Ogre::Vector3(FIELD_CELL_LENGTH / 2, 54, FIELD_CELL_LENGTH / 2));

	MapObject::StandartObjects[MAP_OBJECT_INDEX::Wall_X] = MapObject(MAP_OBJECT_INDEX::Wall_X, WallMask,
		"wall.mesh", "Wall_X", Ogre::Vector3(1.2f, 0.9f, 2.0f/*1.12f, 0.8*1.12f, 1.12f*/));

	Ogre::Quaternion Wall_Z_orient;
	Wall_Z_orient.FromAngleAxis(Ogre::Radian(Ogre::Math::DegreesToRadians(90)), Ogre::Vector3::UNIT_Y);

	MapObject::StandartObjects[MAP_OBJECT_INDEX::Wall_Z] = MapObject(MAP_OBJECT_INDEX::Wall_Z, WallMask,
		"wall.mesh", "Wall_Z", Ogre::Vector3(1.175f, 0.9f, 2.0f/*0.5f, 0.8*0.5f, 0.5f*/), Ogre::Vector3::ZERO,
		Wall_Z_orient);

	// тут нужно грузить стандартные объекты
	// ну или прописывать все вручную -__-

}
void myapp::constructDynamicMapObject(CellCoordinates Coordinates, MapObject& Obj) {
	Ogre::MeshPtr mMesh = Ogre::MeshManager::getSingleton().load(Obj.getMesh(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	if (m_pSceneManager->getShadowTechnique() & 16) mMesh->buildEdgeList(); // stencil
	//mMesh->buildEdgeList();

	Ogre::Entity* newEnt = m_pSceneManager->createEntity(
		Obj.getName() + boost::to_string(m_Objects.size()),
		mMesh);
	//newEnt->setLightMask(0xFFFFFFFF);
	Obj.setEntity(newEnt);
	Obj.setScnMgr(m_pSceneManager);
	//Obj.setTransparency(0.1);

	//static EntityMaterialInstance emi(newEnt);
	//emi.setTransparency(0.5);

	Ogre::SceneNode* newNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
	newNode->attachObject(newEnt);
	newNode->setPosition(getCellCenter(Coordinates) + Obj.getShift());
	newNode->setScale(Obj.getScale());
	newNode->setOrientation(Obj.getOrientation());
	//if (Obj.getIndex() == MAP_OBJECT_INDEX::Wall_Z) {
	//	newNode->yaw(Ogre::Radian(Ogre::Math::PI/2));
	//}
}
void myapp::constructStaticMapObject(CellCoordinates Coordinates, MapObject& Obj) {
	// я думал, что тут будет отдельный метод для статических объектов, юзающие статическую геометрию,
	// но походу пока что лучше юзать отдельные ноды, чтобы можно было просвечивать
	// мб что-то еще тут будет
	//Ogre::Entity* newEnt = m_pSceneManager->createEntity(Obj.getName(), Obj.getMesh());
	//Ogre::SceneNode* newNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
	//newNode->attachObject(newEnt);
	//newNode->setPosition(getCellCenter(Coordinates) + Obj.getShift());
	//newNode->setScale(Obj.getScale());
	constructDynamicMapObject(Coordinates, Obj);
	return;

	Ogre::MeshPtr mMesh = Ogre::MeshManager::getSingleton().load(Obj.getMesh(),
						  Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	//mMesh->buildEdgeList();
	//const Ogre::LodStrategy* lodstr;
	//if (Obj.getMesh() == "ogrehead.mesh") lodstr = mMesh->getLodStrategy();
	//else mMesh->setLodStrategy(lodstr);

	Ogre::Entity* newEnt = m_pSceneManager->createEntity(
		Obj.getName() + boost::to_string(m_Objects.size()),
		mMesh);

	Obj.setEntity(newEnt);
	Obj.setScnMgr(m_pSceneManager);

	m_pStaticGeometry->addEntity(newEnt,
		getCellCenter(Coordinates) + Obj.getShift(),
		Obj.getOrientation(),
		Obj.getScale());
}
void myapp::loadMap(char map_id) {
	if (map_id == m_CurMapID) {
		return;
	}
	if (map_id == -1 || map_id == 0) {
		unloadMap();
		return;
	}
	loadMap(_DATA.getMapName(map_id));
}
void myapp::loadMap(Ogre::String MapName) {

	_GAMELOG("loadMap " + MapName);

	//if (m_sCurMap == MapName) {
	//	return;
	//}

	if (m_sCurMap != "nomap") {
		unloadMap();
	}

	if (MapName == "nomap") {
		return;
	}

	_LOG("Loading map " + MapName);

	m_sCurMap = MapName;

	Ogre::String MapPath = MapName + ".mapdef";

	m_pGameMap = new GameMap(m_pMapBinManager->load(MapPath,"Maps").dynamicCast<MapBin>()->getArray());

	// при динамическом определении просто ничего не произойдёт
	m_pGameMap->recalculateLightMaps(UNIT_MIN_VISIBILITY_RADIUS, UNIT_MAX_VISIBILITY_RADIUS);

	m_pGameMap->setTeam(m_pCurPlayer->get_team());
	//m_LightMaps.initialize(m_pGameMap->getHeight(), m_pGameMap->getWidth(), 0);
	//	AbstractUnit::m_pGameMap = m_pGameMap;

	if (!m_pCurPlayer || m_pConnection->connected()) {
		m_pGameMap->setPlayerID(m_pCurPlayer->get_id());
		// на самом деле тут надо команду выбрать будет
	} else {
		_LOG("Warning: can't set team: not connected");
	}

	createLights();

	createPlane();

	createGrid();

	_LOG("Loading geometry...");

	if (m_pSceneManager->hasStaticGeometry(MapName)) {
		m_pStaticGeometry = m_pSceneManager->getStaticGeometry(MapName);
	} else {

		m_pStaticGeometry = m_pSceneManager->createStaticGeometry(MapName);
		//m_pStaticGeometry->setRegionDimensions(Ogre::Vector3(2*PLANE_RADIUS, SG_HEIGHT, 2*PLANE_RADIUS));
		m_pStaticGeometry->setRegionDimensions(Ogre::Vector3(m_pGameMap->getHeight()*FIELD_CELL_LENGTH,
			SG_HEIGHT,
			m_pGameMap->getWidth()*FIELD_CELL_LENGTH));
		m_pStaticGeometry->setOrigin(Ogre::Vector3(0,0,0));
		m_pStaticGeometry->setCastShadows(true);

		for (auto it = m_pGameMap->getMapContainer().getObjects().begin(); 
			it != m_pGameMap->getMapContainer().getObjects().end(); ++it) {

				m_Objects.push_back(pairCoordObj(it->objectCoordinates, MapObject::StandartObjects[it->objectIndex]));

				if (m_Objects.back().second.isDynamic()) {
					constructDynamicMapObject(it->objectCoordinates, m_Objects.back().second);
				}
				else {
					constructStaticMapObject(it->objectCoordinates, m_Objects.back().second);
				}
		}
	}

	m_pStaticGeometry->build();

	m_pMapBinManager->unload(MapPath);
	m_pMapBinManager->remove(MapPath);

	m_Path.reserve(31);
	m_pPathLine->setVisible(true);

	if (m_pRenderedFOV) {
		delete m_pRenderedFOV;
		m_pRenderedFOV = 0;
	}

	m_pRenderedFOV = new FieldOfView(m_pGameMap);

	_LOG("Map " + MapName + " loaded");
}
void myapp::unloadMap() {

	_GAMELOG("unloadMap");

	if (m_LightLine) {
		delete m_LightLine;
		m_LightLine = 0;
	}
	if (m_DarkLine) {
		delete m_DarkLine;
		m_DarkLine = 0;
	}

	if (m_pRenderedFOV) {
		delete m_pRenderedFOV;
		m_pRenderedFOV = 0;
	}

	if (m_pEntGround) {
		m_pSceneManager->destroyEntity(m_pEntGround);
		m_pEntGround = 0;
	}
	if (m_pTransparentGroundEntity) {
		m_pSceneManager->destroyEntity(m_pTransparentGroundEntity);
		m_pTransparentGroundEntity = 0;
	}
	if (m_pSceneManager->hasLight("directionalLight")) {
		m_pSceneManager->destroyLight("directionalLight");
	}
	if (m_pGrid) {
		m_pGrid->setVisible(false);
		delete m_pGrid;
		m_pGrid = 0;
	}
	if (m_pSceneManager->hasStaticGeometry(m_sCurMap)) {
		m_pSceneManager->destroyStaticGeometry(m_sCurMap);
		m_pStaticGeometry = nullptr;
	}
	//m_pStaticGeometry->destroy();
	m_Objects.clear();
	if (m_pGameMap) {
		delete m_pGameMap;
		m_pGameMap = 0;
	}
	if (m_pPathLine) {
		m_pPathLine->setVisible(false);
	}
	m_sCurMap = "nomap";

	//m_LightMapTexturePtr = 0;

	//m_DecalTextureHandle = 0;
	//m_GroundMaterialHandle = 0;

	Ogre::TextureManager::getSingleton().removeUnreferencedResources();
}

Ogre::TexturePtr myapp::createLightMapTexture() {
	auto lightmapTexture = Ogre::TextureManager::getSingleton().createManual(
		"LightMapTexture",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,
		//sourceWidth*N, sourceHeight*N,
		//sourceWidth, sourceHeight,
		m_pGameMap->getWidth() * m_LightMapPixelPerTile,
		m_pGameMap->getHeight() * m_LightMapPixelPerTile,
		0,					// number of mipmaps
		Ogre::PF_BYTE_BGRA,
		Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);	// usage; should be TU_DYNAMIC_WRITE_ONLY_DISCARDABLE for
							// textures updated very often (e.g. each frame)

	if (lightmapTexture->getFormat() != lightmapTexture->getDesiredFormat()) {
		assert_me(false, "Creating lightmap texture: format isn't desired");
	}

	auto pixelBuffer = lightmapTexture->getBuffer();
	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD); // for best performance use HBL_DISCARD!

	const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
	Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);

	const int lightmapWidth = lightmapTexture->getWidth(),
			  lightmapHeight = lightmapTexture->getHeight();

	const int bytePerPixel = Ogre::PixelUtil::getNumElemBytes(pixelBox.format);

	// изначально целиком затемнённая
	for (int j = 0; j < lightmapHeight; ++j) {
		for (int i = 0; i < lightmapWidth; ++i) {
			*pDest++ = COLOUR_SHADOW_VALUE; //blue
			*pDest++ = COLOUR_SHADOW_VALUE; //green
			*pDest++ = COLOUR_SHADOW_VALUE; //red
			*pDest++ = 0;					//alpha
		}
		pDest += pixelBox.getRowSkip() * bytePerPixel;
	}

	pixelBuffer->unlock();

	if (m_LightLine) {
		delete m_LightLine;
		m_LightLine = 0;
	}
	if (m_DarkLine) {
		delete m_DarkLine;
		m_DarkLine = 0;
	}

	m_DarkLine = new Ogre::uint8[(m_LightMapPixelPerTile + 2) * bytePerPixel];
	for (int i = 0; i < (m_LightMapPixelPerTile + 2) * bytePerPixel; ++i) m_DarkLine[i] = COLOUR_SHADOW_VALUE;
	m_LightLine = new Ogre::uint8[(m_LightMapPixelPerTile + 2) * bytePerPixel];
	for (int i = 0; i < (m_LightMapPixelPerTile + 2) * bytePerPixel; ++i) m_LightLine[i] = COLOUR_LIGHT_VALUE;

	return lightmapTexture;
}
Ogre::TexturePtr myapp::createGroundTexture() {
	/* creating manual texture & material */

	Ogre::TexturePtr sourceTexture = Ogre::TextureManager::getSingleton().load("rockwall.tga", "General");

	// Create the texture

	const int sourceWidth = sourceTexture->getWidth(),
			  sourceHeight = sourceTexture->getHeight();

	auto groundTexture = Ogre::TextureManager::getSingleton().createManual(
		"GroundTexture",				// name
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,				// type
		//sourceWidth*N,					// width
		//sourceHeight*N,					// height
		sourceWidth,					// width
		sourceHeight,					// height
		0,								// number of mipmaps
		sourceTexture->getFormat(),		// pixel format
		Ogre::TU_DEFAULT);				// usage; should be TU_DYNAMIC_WRITE_ONLY_DISCARDABLE for
										// textures updated very often (e.g. each frame)

	if (groundTexture->getFormat() != groundTexture->getDesiredFormat()) {
		int a = 5;
		assert_me(false, "Copying ground texture: format isn't desired");
	}

	if (groundTexture->getFormat() != sourceTexture->getFormat()) {
		int a = 5;
		assert_me(false, "Copying ground texture: formats don't match");
	}

	// Get the pixel buffer
	Ogre::HardwarePixelBufferSharedPtr	pixelBuffer = groundTexture->getBuffer(),
										sourcePixelBuffer = sourceTexture->getBuffer();

	// Lock the pixel buffer and get a pixel box
	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL); // for best performance use HBL_DISCARD!
	sourcePixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
	const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
	const Ogre::PixelBox& rockBox = sourcePixelBuffer->getCurrentLock();

	Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);
	Ogre::uint8* pSource = static_cast<Ogre::uint8*>(rockBox.data);

	// а это я сделал после того, как узнал, что можно делать scale в texture_unit

	for (int j = 0; j < sourceHeight; ++j) {

		memcpy(pDest, pSource, sourceWidth * Ogre::PixelUtil::getNumElemBytes(pixelBox.format));

		pDest += sourceWidth * Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
		pSource += sourceWidth * Ogre::PixelUtil::getNumElemBytes(rockBox.format);

		pDest += pixelBox.getRowSkip() * Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
		pSource += rockBox.getRowSkip() * Ogre::PixelUtil::getNumElemBytes(rockBox.format);

	}

	// это я сделал потом, чтобы сделать текстуру N x N

	//for (int j = 0; j < sourceHeight; ++j) {

	//	for (int k = 0; k < N; ++k) {
	//		memcpy(pDest, pSource, sourceWidth*4);
	//		pDest += sourceWidth*4;
	//	}

	//	pSource += sourceWidth*4;
	//}

	//for (int k = 1; k < N; ++k) {
	//	memcpy(pDest, pixelBox.data, sourceWidth*sourceHeight*4*N);
	//	pDest += sourceWidth*sourceHeight*4*N;
	//}

	//// это было тут изначально

	////for (size_t j = 0; j < sourceHeight; j++)
	////{
	////	for(size_t i = 0; i < sourceWidth; i++)
	////	{
	////		//*pDest++ = 255; // B
	////		//*pDest++ =   0; // G
	////		//*pDest++ =   0; // R
	////		//*pDest++ = 127; // A
	////		memcpy(pDest, pRockDest, 4);
	////		pDest += 4;
	////		pRockDest += 4;
	////		//for (int l = 0; l < 4; ++l)
	////		//	*pDest++ = *pRockDest++;
	////	}

	////	pDest += pixelBox.getRowSkip() * Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
	////	pRockDest += rockBox.getRowSkip() * Ogre::PixelUtil::getNumElemBytes(rockBox.format);
	////}

	// Unlock the pixel buffer
	pixelBuffer->unlock();
	sourcePixelBuffer->unlock();
	sourceTexture->unload();
	//Ogre::TextureManager::getSingleton().unload("rock.jpg");
	return groundTexture;
}
Ogre::MaterialPtr myapp::createMapMaterial() {
	const int N = 5;

	auto mapMaterial = Ogre::MaterialManager::getSingleton().create(
		"GroundMaterial", // name
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	mapMaterial->getTechnique(0)->getPass(0)->createTextureUnitState();
	mapMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureScale(1.0f/N, 1.0f/N);
	mapMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(m_GroundTexturePtr);
	//mapMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(sourceTexture);
	//mapMaterial->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	//mapMaterial->getTechnique(0)->getPass(0)->setAmbient(Ogre::ColourValue::White);
	//mapMaterial->getTechnique(0)->getPass(0)->setDiffuse(Ogre::ColourValue::White);
	//mapMaterial->getTechnique(0)->getPass(0)->setSpecular(1.0f, 1.0f, 1.0f, 9800.0f);
	//mapMaterial->getTechnique(0)->getPass(0)->setEmissive(0.0f, 0.0f, 0.0f);
	//mapMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureCoordSet(0);
	mapMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setColourOperation(Ogre::LBO_MODULATE);
	
	mapMaterial->getTechnique(0)->createPass();

	mapMaterial->getTechnique(0)->getPass(1)->setSceneBlending(Ogre::SBT_MODULATE);
	mapMaterial->getTechnique(0)->getPass(1)->createTextureUnitState();
	mapMaterial->getTechnique(0)->getPass(1)->getTextureUnitState(0)->setTexture(m_LightMapTexturePtr);
	//mapMaterial->getTechnique(0)->getPass(1)->getTextureUnitState(0)->setTextureCoordSet(0);
	mapMaterial->getTechnique(0)->getPass(1)->getTextureUnitState(0)->setColourOperation(Ogre::LBO_MODULATE);

	return mapMaterial;
}
void myapp::setRenderedLightMapDark() {
	auto pixelBuffer = m_LightMapTexturePtr->getBuffer();
	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD); // for best performance use HBL_DISCARD!

	const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
	Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);

	const int lightmapWidth = m_LightMapTexturePtr->getWidth(),
			  lightmapHeight = m_LightMapTexturePtr->getHeight();

	// изначально целиком затемнённая
	for (int j = 0; j < lightmapHeight; ++j) {
		for (int i = 0; i < lightmapWidth; ++i) {
			*pDest++ = COLOUR_SHADOW_VALUE; //blue
			*pDest++ = COLOUR_SHADOW_VALUE; //green
			*pDest++ = COLOUR_SHADOW_VALUE; //red
			*pDest++ = 0;					//alpha
		}
		pDest += pixelBox.getRowSkip() * Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
	}

	pixelBuffer->unlock();
}
void myapp::createPlane() {

	if (!m_pGameMap) {
		_LOG("Game map is not loaded");
		return;
	}

	_LOG("Creating plane...");

	m_Plane = Ogre::Plane(Ogre::Vector3::UNIT_Y, 0);
	//Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	//	m_Plane, PLANE_RADIUS * 2, PLANE_RADIUS * 2, 20, 20, true, 1, 10, 10, Ogre::Vector3::UNIT_Z);

	auto groundMesh = 
		Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			m_Plane, m_pGameMap->getHeight() * FIELD_CELL_LENGTH, m_pGameMap->getWidth() * FIELD_CELL_LENGTH,
			//1, 1, true, 1, m_pGameMap->getHeight()/2, m_pGameMap->getWidth()/2, Ogre::Vector3::UNIT_Z);
			1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

	//m_pEntGround = m_pSceneManager->createEntity("GroundEntity", "ground");
	m_pEntGround = m_pSceneManager->createEntity("GroundEntity", groundMesh);
	m_pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(m_pEntGround);

	m_pTransparentGroundEntity = m_pSceneManager->createEntity("TransparentGroundEntity", groundMesh);
	m_pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(m_pTransparentGroundEntity);

	if (m_GroundTexturePtr.isNull()) {
		m_GroundTexturePtr = createGroundTexture();
	}

	if (m_LightMapTexturePtr.isNull()) {
		m_LightMapTexturePtr = createLightMapTexture();
	} else {
		setRenderedLightMapDark();
	}

	if (m_MapMaterialPtr.isNull()) {
		m_MapMaterialPtr = createMapMaterial();
		auto c = m_MapMaterialPtr->getTechnique(0)->getPass(0)->getDiffuse();
		c.a = 0.5f;
		m_MapMaterialPtr->getTechnique(0)->getPass(0)->setDiffuse(c);
	}
	if (m_DecalMaterialPtr.isNull()) {
		m_DecalMaterialPtr = Ogre::MaterialManager::getSingleton().getByName("transparent", "General");
	}

	//m_pEntGround->setMaterialName("Rockwall");
	m_pEntGround->setMaterial(m_MapMaterialPtr);
	m_pEntGround->setCastShadows(false);

	m_pTransparentGroundEntity->setMaterial(m_DecalMaterialPtr);
	m_pTransparentGroundEntity->setCastShadows(false);
	//m_pEntGround->getParentNode()->setPosition(PLANE_CENTER_X, 0, PLANE_CENTER_Z);
	m_pEntGround->getParentNode()->setPosition(FIELD_CELL_LENGTH * m_pGameMap->getHeight() / 2,
		0,
		FIELD_CELL_LENGTH * m_pGameMap->getWidth() / 2);
	m_pTransparentGroundEntity->getParentNode()->setPosition(FIELD_CELL_LENGTH * m_pGameMap->getHeight() / 2,
		1.0f,
		FIELD_CELL_LENGTH * m_pGameMap->getWidth() / 2);
	////////////////////////////////////////////
	//auto groundMesh1 = 
	//	Ogre::MeshManager::getSingleton().createPlane("grount", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	//		m_Plane, m_pGameMap->getHeight() * FIELD_CELL_LENGTH, m_pGameMap->getWidth() * FIELD_CELL_LENGTH,
	//		//1, 1, true, 1, m_pGameMap->getHeight()/2, m_pGameMap->getWidth()/2, Ogre::Vector3::UNIT_Z);
	//		1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

	////m_pEntGround = m_pSceneManager->createEntity("GroundEntity", "ground");
	//auto EntGround = m_pSceneManager->createEntity("GroundEntity1", groundMesh1);
	//m_pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(EntGround);

	//mapMaterial = Ogre::MaterialManager::getSingleton().getByName("rock");

	//EntGround->setMaterial(mapMaterial);
	//EntGround->setCastShadows(false);
	////m_pEntGround->getParentNode()->setPosition(PLANE_CENTER_X, 0, PLANE_CENTER_Z);
	//EntGround->getParentNode()->setPosition(FIELD_CELL_LENGTH * m_pGameMap->getHeight() / 2,
	//	1,
	//	FIELD_CELL_LENGTH * m_pGameMap->getWidth() / 2);

	//m_GroundMaterialHandle = m_MapMaterialPtr->getHandle();
	//m_MapMaterialPtr = mapMaterial;
	
}

void myapp::createLights() {

	_LOG("Creating lighs...");

	m_pSceneManager->setAmbientLight(Ogre::ColourValue(0.6f, 0.6f, 0.6f));

	m_pSceneManager->setShadowDirectionalLightExtrusionDistance( 1500.0f );
	m_pSceneManager->setShadowFarDistance( 1500.0f );

	//Ogre::Light* l = m_pSceneManager->createLight("pointLight");
	//l->setDiffuseColour(Ogre::ColourValue(0.9f,0.9f,0.9f));
	//l->setSpecularColour(Ogre::ColourValue::Blue);

	// подходит для фонаря
	//	l->setPosition(PLANE_CENTER_X - 100,300,PLANE_CENTER_Z + 100);
	//	l->setAttenuation(10000,1,0,0.00001);	

	//l->setPosition(PLANE_CENTER_X - 100,600,PLANE_CENTER_Z + 100);
	//l->setAttenuation(10000,1,0,0.0000025);

	//Ogre::Light* l = m_pSceneManager->createLight("pointLight2");
	//l->setPosition(20,200,50);
	//l->setPowerScale(0.1);
	//l->setDiffuseColour(Ogre::ColourValue(0.1,0.1,0.1));
	//l->setSpecularColour(Ogre::ColourValue::Blue);

	Ogre::Light* directionalLight = m_pSceneManager->createLight("directionalLight");
	directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
	//directionalLight->setDiffuseColour(Ogre::ColourValue(0.6f, 0.6f, 0.6f));
	//directionalLight->setSpecularColour(Ogre::ColourValue(0.4f, 0.4f, 0.4f));
	directionalLight->setDiffuseColour(Ogre::ColourValue::White);
	directionalLight->setSpecularColour(Ogre::ColourValue(.9, .9, .9));
	directionalLight->setDirection(Ogre::Vector3( 0.6, -1.5, -1 ));

	m_pSceneManager->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_TEXTURE_MODULATIVE);
	m_pSceneManager->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_TEXTURE_ADDITIVE);

	//directionalLight->setDiffuseColour(Ogre::ColourValue::White);
	//directionalLight->setSpecularColour(Ogre::ColourValue::Blue);

	//m_pSceneManager->setShadowIndexBufferSize(200000);

	//Ogre::Light* spotLight = m_pSceneManager->createLight("spotLight");
	//   spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
	//   spotLight->setDiffuseColour(0, 0, 1.0);
	//   spotLight->setSpecularColour(0, 0, 1.0);
	//
	//   spotLight->setDirection(0, -1, 0);
	//   spotLight->setPosition(Ogre::Vector3(-900, 100, 900));
	//
	//   spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(40));
}
void myapp::createFog() {
	Ogre::ColourValue fadeColour(0.9, 0.9, 0.9);
	m_pSceneManager->setFog(Ogre::FOG_EXP2, fadeColour, 0.001f, 600, 1500);
}
void myapp::createGrid() {

	////	Ogre::MaterialPtr matptr = Ogre::MaterialManager::getSingleton().create("BaseColoured1","General");
	//Ogre::MaterialPtr matptr = Ogre::MaterialManager::getSingleton().load("BaseColoured1","General");
	//matptr->load();
	//matptr->getBestTechnique()->getPass(0)->setVertexColourTracking(Ogre::TVC_DIFFUSE);
	//matptr->getBestTechnique()->getPass(0)->setLightingEnabled(false); 
	//matptr->setDiffuse(1,1,1,1);//(0,1,0,0.8);
	////	Ogre::MaterialSerializer ser;
	////	ser.exportMaterial(matptr,"ololo.material");
	////	matptr->setDepthCheckEnabled(false);

	if (!m_pGameMap) {
		_LOG("Can't create grid: game map is not loaded");
		return;
	}

	m_pGrid = new Ogre::ManualObject("Grid1");
	m_pGrid->setCastShadows(false);
	m_pGrid->estimateVertexCount(m_pGameMap->getHeight() * 2 + m_pGameMap->getWidth() * 2 + 4);

	//	for (int i = 1; i < FIELD_SIZE; i++) {
	//		m_pGrid->begin("BaseColoured1", Ogre::RenderOperation::OT_TRIANGLE_STRIP);//OT_LINE_STRIP);
	//			m_pGrid->colour(COLOUR_GRID);
	//			m_pGrid->position(PLANE_MIN_X, 1, PLANE_MIN_Z + i * FIELD_CELL_LENGTH - LINE_WIDTH);
	//			m_pGrid->position(PLANE_MAX_X, 1, PLANE_MIN_Z + i * FIELD_CELL_LENGTH - LINE_WIDTH);
	//			m_pGrid->position(PLANE_MAX_X, 1, PLANE_MIN_Z + i * FIELD_CELL_LENGTH + LINE_WIDTH);
	//			m_pGrid->position(PLANE_MIN_X, 1, PLANE_MIN_Z + i * FIELD_CELL_LENGTH + LINE_WIDTH);
	///////////////////////////////////////////////// не знаю, почему просто так не работает, приходится так
	//			m_pGrid->position(PLANE_MIN_X, 1, PLANE_MIN_Z + i * FIELD_CELL_LENGTH - LINE_WIDTH);
	//			m_pGrid->position(PLANE_MAX_X, 1, PLANE_MIN_Z + i * FIELD_CELL_LENGTH - LINE_WIDTH);
	//			//m_pGrid->position(PLANE_MAX_X, 1, PLANE_MIN_Z + i * FIELD_CELL_LENGTH + LINE_WIDTH);
	//			//m_pGrid->position(PLANE_MIN_X, 1, PLANE_MIN_Z + i * FIELD_CELL_LENGTH + LINE_WIDTH);
	//
	//		m_pGrid->end();
	//	}
	//	for (int i = 1; i < FIELD_SIZE; i++) {
	//		m_pGrid->begin("BaseColoured1", Ogre::RenderOperation::OT_TRIANGLE_FAN);//OT_LINE_STRIP);
	//			m_pGrid->colour(COLOUR_GRID);
	//			m_pGrid->position(PLANE_MIN_X + i * FIELD_CELL_LENGTH - LINE_WIDTH, 1, PLANE_MIN_Z);
	//			m_pGrid->position(PLANE_MIN_X + i * FIELD_CELL_LENGTH - LINE_WIDTH, 1, PLANE_MAX_Z);
	//			m_pGrid->position(PLANE_MIN_X + i * FIELD_CELL_LENGTH + LINE_WIDTH, 1, PLANE_MAX_Z);
	//			m_pGrid->position(PLANE_MIN_X + i * FIELD_CELL_LENGTH + LINE_WIDTH, 1, PLANE_MIN_Z);
	//		m_pGrid->end();
	//	}

	for (int i = 1; i < m_pGameMap->getWidth(); i++) {
		m_pGrid->begin("BaseColoured1", Ogre::RenderOperation::OT_LINE_STRIP);
		m_pGrid->colour(COLOUR_GRID);
		//m_pGrid->position(PLANE_MIN_X, 0.5, PLANE_MIN_Z + i * FIELD_CELL_LENGTH);
		//m_pGrid->position(PLANE_MAX_X, 0.5, PLANE_MIN_Z + i * FIELD_CELL_LENGTH);
		m_pGrid->position(0, 0.5, 0 + i * FIELD_CELL_LENGTH);
		m_pGrid->position(m_pGameMap->getHeight() * FIELD_CELL_LENGTH, 0.5, i * FIELD_CELL_LENGTH);
		m_pGrid->end();
	}
	for (int i = 1; i < m_pGameMap->getHeight(); i++) {
		m_pGrid->begin("BaseColoured1", Ogre::RenderOperation::OT_LINE_STRIP);
		m_pGrid->colour(COLOUR_GRID);
		m_pGrid->position(i * FIELD_CELL_LENGTH, 0.5, 0);
		m_pGrid->position(i * FIELD_CELL_LENGTH, 0.5, m_pGameMap->getWidth() * FIELD_CELL_LENGTH);
		m_pGrid->end();
	}

	m_pGrid->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);

	m_pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(m_pGrid);

	//matptr->unload();
	//	Ogre::MaterialManager::getSingleton().remove("BaseColoured1");
}
void myapp::createScene() {

	//Ogre::Entity* ogreHead = m_pSceneManager->createEntity("OgreHead1515", "ogrehead.mesh");
	//Ogre::SceneNode* headNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
	//headNode->attachObject(ogreHead);
	//headNode->setPosition(PLANE_CENTER_X+100,30,PLANE_CENTER_Z+100);
	//headNode->setScale(Ogre::Vector3(2.0f));

	//Ogre::Entity* ogreHead = m_pSceneManager->createEntity("OgreHead", "ogrehead.mesh");
	//Ogre::SceneNode* headNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
	//headNode->attachObject(ogreHead);
	//headNode->setPosition(getCellCenter(CellCoordinates(12, 19)).x + 10,
	//						30,
	//						getCellCenter(CellCoordinates(12, 19)).z + 10);

	//Ogre::Entity* wallEnt = m_pSceneManager->createEntity("Wall", "wall.mesh");
	//Ogre::SceneNode* wallNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
	//wallNode->attachObject(wallEnt);
	//wallNode->setPosition(getCellCenter(CellCoordinates(11, 19)).x + 10,
	//						30,
	//						getCellCenter(CellCoordinates(11, 19)).z + 10);

	//////////////
	//Ogre::Real alphaLevel = 0.5f;

	//Ogre::MaterialPtr alphamat = Ogre::MaterialManager::getSingleton().create("testy", 
	//   Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	//Ogre::Pass* pass = alphamat->getTechnique(0)->getPass(0);
	//pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	//pass->setDepthWriteEnabled(false);
	//Ogre::TextureUnitState* t = pass->createTextureUnitState();
	//t->setAlphaOperation(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, alphaLevel);

	//auto ent = m_pSceneManager->createEntity("asd", "ogrehead.mesh");
	//m_pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
	//ent->getParentNode()->translate(40, 40, 40);

	//static EntityMaterialInstance emi(ent);

	//emi.setTransparency(alphaLevel);

	//////////////

	//	rect = new Ogre::Rectangle2D("MyRect");
	////	Ogre::Rectangle2D* rec1t = m_pSceneManager->createMovableObject("MyRect");
	//	rect->setCorners(0.1,0.1,0.9,0.9);
	//	rect->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
	//	rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY); // when using this, ensure Depth Check is Off in the material
	//	rect->setUseIdentityProjection(true);
	//	rect->setUseIdentityView(true);
	//	rect->setQueryFlags(0);
	//	m_pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(rect);

	//	m_pSelectionBox = new SelectionBox("SelectionBox");
	//	m_pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(m_pSelectionBox);
	//	m_pVolQuery = m_pSceneManager->createPlaneBoundedVolumeQuery(Ogre::PlaneBoundedVolumeList());

	////////////// ТОЛЬКО ДЛЯ ТЕСТИРОВАНИЯ
	//	m_pActiveSelectedGroup = new SelectedGroup();
	//////////////

	//Robot* r = RobotFactory::getInstance().createUnit(m_pSceneManager,
	//												PLANE_CENTER_X - 700, 
	//												0,
	//												PLANE_CENTER_Z + 800);
	//	r->setAnimation("Idle");

	m_pPathLine = new DynamicLines(Ogre::RenderOperation::OT_LINE_STRIP);
	m_pPathLine->setMaterial("BaseColoured1");
	m_pPathLine->setCastShadows(false);

	Ogre::SceneNode *m_pPathLineNode1 = m_pSceneManager->getRootSceneNode()->createChildSceneNode("PathLines");
	m_pPathLineNode1->attachObject(m_pPathLine);

	//createFog();

	//loadMap("testmap");
	//unloadMap();
	//loadMap("testmap");

}

void myapp::setCursorCrosshair() {
	if (m_cursorType == Crosshair) 
		return;
	m_cursorType = Crosshair;
	m_pTrayManager->showCursor("SdkTrays/Crosshair");
	m_pTrayManager->getCursorImage()->setTop(-16);
	m_pTrayManager->getCursorImage()->setLeft(-16);
}
void myapp::setCursorArrow() {
	if (m_cursorType == Arrow) 
		return;
	m_cursorType = Arrow;
	m_pTrayManager->showCursor("SdkTrays/Cursor");
	m_pTrayManager->getCursorImage()->setTop(0);
	m_pTrayManager->getCursorImage()->setLeft(0);
}

Ogre::Matrix4 myapp::BuildScaledOrthoMatrix(float _left, float _right,
	float _bottom, float _top,
	float _near, float _far)
{
	float invw = 1 / (_right - _left);
	float invh = 1 / (_top - _bottom);
	float invd = 1 / (_far - _near);

	Ogre::Matrix4 proj = Ogre::Matrix4::ZERO;

	proj[0][0] = 2 * invw;
	proj[0][3] = -(_right + _left) * invw;
	proj[1][1] = 2 * invh;
	proj[1][3] = -(_top + _bottom) * invh;
	proj[2][2] = -2 * invd;
	proj[2][3] = -(_far + _near) * invd;
	proj[3][3] = 1;

	return proj;
}

void myapp::sayHello(Ogre::StringVector&) {
	_GAMELOG("sayHello");
	_LOG("Hello, stupid!");
	//*m_pConsole << "Hello, stupid! " << 1 << ' ' << 2 << '\n';
}
void myapp::shutDown(Ogre::StringVector&) {
	m_bShutDown = true;
}
void myapp::fullScreen(Ogre::StringVector& vec) {
	_GAMELOG("fullScreen");

	if (m_pRenderWindow->isFullScreen()) {
		m_pRenderWindow->setFullscreen(false, 800, 600);
	} else {
		m_pRenderWindow->setFullscreen(true, 1366, 768);
	}
}
void myapp::printMap(Ogre::StringVector&) {
	_GAMELOG("printMap");
	*m_pConsole << m_pGameMap << '\n';
}
void myapp::setConsoleLineCount(Ogre::StringVector& vec) {
	_GAMELOG("setConsoleLineCount");
	if (vec.size() != 2) {
		_LOG("set_line_count <count>");
		return;
	}

	int a = Ogre::StringConverter::parseInt(vec[1], -8491923);

	if (a == -8491923 || a < 0) {
		_LOG("Specify a positive integer value, you idiot");
		return;
	}

	m_pConsole->setLineCount(a < 35? a : 35);
}
void myapp::setResolution(Ogre::StringVector& vec) {
	_GAMELOG("setResolution");
	if (vec.size() != 3) {
		_LOG("set_resolution <Width> <Height>");
		return;
	}
	Ogre::uint W = Ogre::StringConverter::parseUnsignedInt(vec[1]);
	Ogre::uint H = Ogre::StringConverter::parseUnsignedInt(vec[2]);

	if (W < 640 || H < 480) {
		_LOG("Too small!");
		return;
	}

	m_pRenderWindow->resize(W, H);
	m_pConsole->forceUpdateConsole();
}
void myapp::setOrthoScale(Ogre::StringVector& vec) {
	_GAMELOG("setOrthoScale");

	if (m_pCamera->getProjectionType() != Ogre::ProjectionType::PT_ORTHOGRAPHIC) {
		_LOG("You must be in orthographic mode to set the orthographic scale, idiot");
		return;
	}

	if (vec.size() != 2) {
		_LOG("set_ortho_scale <scale> | " +  Ogre::StringConverter::toString(m_fMinScale) + 
			" <= scale <= " + Ogre::StringConverter::toString(m_fMaxScale));
		return;
	}

	Ogre::Real _scale = Ogre::StringConverter::parseReal(vec[1]);

	if (_scale > m_fMaxScale || _scale < m_fMinScale) {
		_LOG("set_ortho_scale <scale> | " +  Ogre::StringConverter::toString(m_fMinScale) + 
			" <= scale <= " + Ogre::StringConverter::toString(m_fMaxScale));
		return;
	}

	setCameraScale(_scale);
}
void myapp::consoleHelp(Ogre::StringVector& vec) {
	_GAMELOG("consoleHelp");
	//_LOG("Use your mind and get lucky!\nI'm too lazy now to make a help for console -__-\n"
	if (vec.size() != 2) {
		_LOG("List of avaliable commands:");
		m_pConsole->showCommands();
		return;
	}
	if (m_pConsole->isSuchCommand(vec[1])) {
		static int k = 0;
		if (k == 0) {
			_LOG("type_id command " + vec[1] + " without paremeters");
		}
		if (k == 1) {
			_LOG("Use your mind and get lucky!");
		}
		k++;
	}
	else {
		_LOG("There are no such command");
	}

}
void myapp::consoleWhy(Ogre::StringVector& vec) {
	static int k = 0;
	if (k == 0) {
		_LOG("Why what?");
	}
	if (k == 1) {
		_LOG("Because fuck you, that's why");
	}
	k++;
}
void myapp::printCameraOrientation(Ogre::StringVector&) {
	_GAMELOG("printCameraOrientation");
	Ogre::Quaternion q = m_pCamera->getOrientation();
	*m_pConsole << q.w << ' ' << q.x << ' '<< q.y << ' ' << q.z << '\n';
}
void myapp::printCameraDirection(Ogre::StringVector&) {
	_GAMELOG("printCameraDirection");
	Ogre::Vector3 d = m_pCamera->getDirection();
	*m_pConsole << d.x << ' '<< d.y << ' ' << d.z << '\n';
}
//void myapp::setRobotScale(Ogre::StringVector& vec) {
//	_GAMELOG("setRobotScale");
//	if (vec.size() != 2) {
//		_LOG("set_robot_scale <scale> | " +  Ogre::StringConverter::toString(m_fMinScale) + 
//			" <= scale <= " + Ogre::StringConverter::toString(m_fMaxScale));
//		return;
//	}
//
//	Ogre::Real _scale = Ogre::StringConverter::parseReal(vec[1]);
//
//	if (_scale > m_fMaxScale || _scale < m_fMinScale) {
//		_LOG("set_ortho_scale <scale> | " +  Ogre::StringConverter::toString(m_fMinScale) + 
//			" <= scale <= " + Ogre::StringConverter::toString(m_fMaxScale));
//		return;
//	}
//
//	for (auto it = AbstractUnit::g_Units.begin(); it != AbstractUnit::g_Units.end(); ++it) {
//		m_fDefaultRobotScale = _scale;
//		(*it)->setScale(Ogre::Vector3(_scale));
//		(*it)->setSpeed(50 * _scale * _scale);
//	}
//}
void myapp::consoleConnect(Ogre::StringVector& vec) {
	_GAMELOG("consoleConnect");
	if (vec.size() != 3) {
		_LOG("connect <ipv4 address> <username>");
		return;
	}
	sc_res res = start_connection(vec[1], m_DefaultPort, vec[2]);
	if (res != SC_OK) {
		if (res & SC_ALREADY_CONNECTED) {
			if (res & SC_INVALID_IP) {
				_LOG("Already connected. Disconnect first\nAlso, your IP is invalid, you damn idiot");
			}
			else {
				_LOG("Already connected. Disconnect first");
			}
		}
		if (res & SC_INVALID_IP) {
			_LOG("Invalid IP");
		}
	}
}
void myapp::consoleDisconnect(Ogre::StringVector&) {
	_GAMELOG("consoleDisconnect");
	disconnect();
}
void myapp::consoleRunService(Ogre::StringVector&) {
	_GAMELOG("consoleRunService");
#ifdef _DEBUG
	forceRunService();
#else
	_LOG("Access denied");
#endif
}
void myapp::enableUnimportantMsgs(Ogre::StringVector& vec) {
	_GAMELOG("enableUnimportantMsgs");
	if (vec.size() == 2) {
		bool enable = Ogre::StringConverter::parseBool(vec[1]);
		Connection::setEnableUnimportantMessages(enable);
		_LOG("unimportant_messages is set to " + Ogre::String(enable?"1":"0"));
	}
	else {
		_LOG("unimportant_messages " + boost::to_string(Connection::getEnableUnimportantMessages()));
	}
}
void myapp::enableEmulatingPing(Ogre::StringVector& vec) {
	_GAMELOG("enableEmulatingPing");
#ifdef EMULATE_PING	
	if (vec.size() == 3) {
		Ogre::uint32 min = Ogre::StringConverter::parseUnsignedInt(vec[1], 38471543);
		Ogre::uint32 max = Ogre::StringConverter::parseUnsignedInt(vec[2], 38471543);
		if (min == 38471543 || max == 38471543) {
			_LOG("Syntax error");
			return;
		}
		if (min > 5000 || max > 5000 || max < min) {
			//_LOG("emulate_ping <min ping> <max ping> | 0 <= min <= max <= 5000\n \
			//	 Now min_ping = " + Ogre::StringConverter::toString(min) + ", max_ping = "
			//	 + Ogre::StringConverter::toString(max));
			_LOG("0 <= min <= max <= 5000");
			return;
		}
		Connection::set_emulate_ping(min, max);
		_LOG("min_ping is set to " + vec[1]);
		_LOG("max_ping is set to " + vec[2]);
	}
	else {
		Ogre::uint32 min, max;
		Connection::get_emulate_ping(min, max);
		_LOG("emulate_ping <min ping> <max ping> | 0 <= min <= max <= 5000\n \
			 Now min_ping = " + Ogre::StringConverter::toString(min) + ", max_ping = "
			 + Ogre::StringConverter::toString(max));
	}
#else
	_LOG("Access denied");
#endif
}
void myapp::consoleCreateUnit(Ogre::StringVector& vec) {
#ifndef _DEBUG
	_LOG("Access denied");
#else
	if (!connected()) {
		_LOG("Cant't create unit: not connected");
		return;
	}
	if (vec.size() != 5) {
		_LOG("create_unit <unit type_id> <unit PlayerID> <x coord> <y coord>");
	} else {
		char type_id = _DATA.getTypeID(vec[1]);
		if (type_id != UNIT_TYPE::UNDEFINED_TYPE) {
			unsigned int player_id = Ogre::StringConverter::parseUnsignedInt(vec[2], 48313);
			if (player_id == 48313) {
				_LOG("PlayerID should be positive integer value, you idiot");
				return;
			}
			CellCoordinates where;
			where.x = Ogre::StringConverter::parseInt(vec[3]);
			where.y = Ogre::StringConverter::parseInt(vec[4]);
			if (where.x < 0 || where.x > m_pGameMap->getHeight() || 
			where.y < 0 || where.y > m_pGameMap->getWidth() ) {
				_LOG("Incorrect coordinates");
				return;
			}
			UnitDef ud = _DATA.getDefaultUD(type_id);
			ud.PlayerID = player_id;
			req_CreateUnit(ud, where, CellCoordinates());
		}
	}
#endif
}
void myapp::consoleCreateHero(Ogre::StringVector& vec) {
#ifndef _DEBUG
	_LOG("Access denied");
#else
	if (!connected()) {
		_LOG("Cant't create unit: not connected");
		return;
	}
	if (vec.size() < 7 || vec.size() > 11) {
		_LOG("create_hero <name> <class> <type_id> <PlayerID> <x coord> <y coord> [weapon] [armor] [hp] [ap]");
	} else {

		char type_id = _DATA.getTypeID(vec[3]);
		if (type_id == UNIT_TYPE::UNDEFINED_TYPE) {
			_LOG("Invalid type_id");
			return;
		}

		char Weapon = _DATA.getDefaultWeapon(type_id);
		char Armor = _DATA.getDefaultArmor(type_id);
		short HP = _DATA.getDefaultHP(type_id);
		char AP = _DATA.getDefaultAP(type_id);

		if (vec.size() > 7) {
			Weapon = _DATA.getWeaponID(vec[7]);
			if (Weapon == WEAPON::UNDEFINED_WEAPON) {
				_LOG("Incorrect weapon");
				return;
			}
			if (vec.size() > 8) {
				Armor = _DATA.getArmorID(vec[8]);
				if (Armor == ARMOR::UNDEFINED_ARMOR) {
					_LOG("Incorrect armor");
					return;
				}
				if (vec.size() > 9) {
					HP = Ogre::StringConverter::parseUnsignedInt(vec[9], 48313);
					if (HP == 48313) {
						_LOG("HP should be positive integer value, you idiot");
						return;
					}
					if (vec.size() > 10) {
						AP = Ogre::StringConverter::parseUnsignedInt(vec[10], 48313);
						if (AP == 48313) {
							_LOG("AP should be positive integer value, you idiot");
							return;
						}
					}
				}
			}
		}


		unsigned int player_id = Ogre::StringConverter::parseUnsignedInt(vec[4], 48313);
		if (player_id == 48313) {
			_LOG("PlayerID should be positive integer value, you idiot");
			return;
		}

		CellCoordinates where;
		where.x = Ogre::StringConverter::parseInt(vec[5]);
		where.y = Ogre::StringConverter::parseInt(vec[6]);

		if (where.x < 0 || where.x > m_pGameMap->getHeight() || 
		where.y < 0 || where.y > m_pGameMap->getWidth() ) {
			_LOG("Incorrect coordinates");
			return;
		}

		char Class = _DATA.getClassID(vec[2]);
		if (Class == HERO_CLASS::UNDEFINED_CLASS) {
			_LOG("Invalid class");
			return;
		}

		UnitDef ud;
		ud.AP = AP;
		ud.Armor = Armor;
		ud.Class = Class;
		ud.HP = HP;
		ud.PlayerID = player_id;
		ud.type_id = type_id;
		ud.ViewRange = _DATA.getDefaultViewRange(type_id);

		req_CreateHero(vec[1], ud, where, CellCoordinates());
	}
#endif
}
void myapp::consoleTurnEnd(Ogre::StringVector&) {
	_GAMELOG("consoleTurnEnd");
	req_TurnEnd();
}
void myapp::consoleGameStart(Ogre::StringVector&) {
	_GAMELOG("consoleGameStart");
	//#ifndef _DEBUG
	//	_LOG("Access denied");
	//	return;
	//#endif
	//char req = GAME_STARTED;
	//sendRequest(std::string(&req,1));
	req_StartGame();
}
void myapp::consoleCreateLobby(Ogre::StringVector& vec) {
	_GAMELOG("consoleCreateLobby");
	if (!connected()) {
		_LOG("Connect first");
		return;
	}
	if (vec.size() != 2) {
		_LOG("create_lobby <map_name>");
		return;
	}

	char map_id = _DATA.getMapID(vec[1]);

	if (map_id == -1) {
		_LOG("Map " + vec[1] + " doesn't exist");
		return;
	}

	req_CreateLobby(map_id);
}
void myapp::printLightMap(Ogre::StringVector& vec) {
	_GAMELOG("printLightMap");
//#ifndef _DEBUG
//	_LOG("Access denied");
//#else
	printLightMap(m_mouseCellCoordinates);
//#endif
}
void myapp::consoleJoinLobby(Ogre::StringVector& vec) {
	_GAMELOG("consoleJoinLobby");
	if (vec.size() != 2) {
		_LOG("join_lobby <lobby_id>");
		return;
	}
	int lobby_id = Ogre::StringConverter::parseUnsignedInt(vec[1], 48313);
	if (lobby_id == 48313) {
		_LOG("Lobby id should be positive integer value, you idiot");
		return;
	}
	req_JoinLobby(lobby_id);
}
void myapp::consolePlayersInLobby(Ogre::StringVector& vec) {
	_GAMELOG("consolePlayersInLobby");
	if (vec.size() != 2) {
		_LOG("players_in_lobby <lobby_id>");
		return;
	}
	int lobby_id = Ogre::StringConverter::parseUnsignedInt(vec[1], 48313);
	if (lobby_id == 48313) {
		_LOG("Lobby id should be positive integer value, you idiot");
		return;
	}
	req_PlayersInLobby(lobby_id);
}
void myapp::consoleExitLobby(Ogre::StringVector&) {
	_GAMELOG("consoleExitLobby");
	if (!connected()) {
		_LOG("You are not connected");
		return;
	}
	if (m_pCurPlayer && m_pCurPlayer->is_in_lobby() == false) {
		_LOG("You aren't in lobby");
		return;
	}
	req_DropFromLobby();
}
void myapp::consoleShowPlayersInLobby(Ogre::StringVector&) {
	_GAMELOG("consoleShowPlayersInLobby");
	if (!connected()) {
		_LOG("You aren't connected");
		return;
	}
	if (!m_pCurPlayer || !m_pCurPlayer->is_in_lobby()) {
		_LOG("You aren't in lobby");
		return;
	}
	show_players_in_lobby();
}
void myapp::consolePlayerList(Ogre::StringVector&) {
	_GAMELOG("consolePlayersList");
	if (!m_pCurPlayer || !m_pCurPlayer->is_online()) {
		_LOG("You are not logged in");
		return;
	}
	req_PlayerList();
}
void myapp::consoleLobbyList(Ogre::StringVector&) {
	_GAMELOG("consoleLobbyList");
	if (!m_pCurPlayer || !m_pCurPlayer->is_online()) {
		_LOG("You are not logged in");
		return;
	}
	req_LobbyList();
}
void myapp::consoleShowPlayerList(Ogre::StringVector&) {
	_GAMELOG("consoleShowPlayerList");
	if (!m_pCurPlayer || !m_pCurPlayer->is_online()) {
		_LOG("You are not logged in");
		return;
	}
	show_player_list();
}
void myapp::consoleShowLobbyList(Ogre::StringVector&) {
	_GAMELOG("consoleShowLobbyList");
	if (!m_pCurPlayer || !m_pCurPlayer->is_online()) {
		_LOG("You are not logged in");
		return;
	}
	show_lobby_list();
}
void myapp::consoleChangeMap(Ogre::StringVector& vec) {
	if (vec.size() != 2) {
		_LOG("change_map <new_map>");
		return;
	}

	char map_id = _DATA.getMapID(vec[1]);

	if (map_id == -1) {
		_LOG("Map " + vec[1] + " doesn't exist");
		return;
	}

	req_ChangeMap(map_id);

}
void myapp::consoleCreateMerk(Ogre::StringVector& vec) {

	_GAMELOG("consoleCreateMerk");

	if (vec.size() < 3 || vec.size() > 4) {
		_LOG("create_merk <name> <type> [weapon]");
		return;
	}

	if (vec[1].size() < 3 || vec[1].size() > MAX_NICK_LENGTH) {
		_LOG("Name of new merk should contain between 3 and " + int2str(MAX_NICK_LENGTH) + " symbols");
		return;
	}

	char type_id = _DATA.getTypeID(vec[2]);
	if (type_id == UNIT_TYPE::UNDEFINED_TYPE) {
		_LOG("The type \"" + vec[2] + "\" doesn't exist in game");
		return;
	}

	char weapon_id;
	if (vec.size() == 4) {
		weapon_id = _DATA.getWeaponID(vec[3]);

		if (weapon_id == WEAPON::UNDEFINED_WEAPON) {
			_LOG("Weapon \"" + vec[3] + "\" doesn't exist in game");
			return;
		}

	} else {
		weapon_id = WEAPON::BOW;
	}

	req_CreateMerk(type_id, weapon_id, vec[1]);
}
void myapp::consoleDeleteMerk(Ogre::StringVector& vec) {

	_GAMELOG("consoleDeleteMerk");

	if (vec.size() != 2) {
		_LOG("delete_merk <merk_id>");
		return;
	}

	int merk_id = Ogre::StringConverter::parseUnsignedInt(vec[1]);
	if (merk_id == 0) {
		_LOG("merk_id must be integer value between 1 and 127");
		return;
	}

	if (!m_pCurPlayer || !m_pCurPlayer->merk_exist(merk_id)) {
		_LOG("Merk with id " + boost::lexical_cast<Ogre::String>(int(merk_id)) + " wasn't found");
		return;
	}

	req_DeleteMerk(merk_id);
}
void myapp::consoleMerkList(Ogre::StringVector&) {

	_GAMELOG("consoleMerkList");

	show_merk_list();
}
void myapp::consoleRefreshMerkList(Ogre::StringVector&) {

	_GAMELOG("consoleRefreshMerkList");

	req_MerkList();
}
void myapp::consoleChangeWeapon(Ogre::StringVector& vec) {

	_GAMELOG("consoleChangeWeapon");

	if (vec.size() < 2 || vec.size() > 3) {
		_LOG("change_weapon <new_weapon> [merk_id = current_merk_id]");
		return;
	}

	char weapon_id = _DATA.getWeaponID(vec[1]);
	if (weapon_id == WEAPON::UNDEFINED_WEAPON) {
		_LOG("Weapon \"" + vec[3] + "\" doesn't exist in game");
		return;
	}

	int merk_id;
	if (vec.size() == 3) {
		merk_id = Ogre::StringConverter::parseUnsignedInt(vec[2]);
		if (merk_id == 0) {
			_LOG("merk_id must be integer value between 1 and 127");
			return;
		}

		if (!m_pCurPlayer || !m_pCurPlayer->merk_exist(merk_id)) {
			_LOG("Merk with id " + boost::lexical_cast<Ogre::String>(int(merk_id)) + " wasn't found");
			return;
		}
	} else {
		if (!m_pCurPlayer || !m_pCurPlayer->merk_choosen()) {
			_LOG("Merk wasn't specified and current merk isn't choosen. Choose merk or specify merk_id");
			return;
		}
		merk_id = m_pCurPlayer->get_merk_id();
	}

	req_ChangeWeapon(merk_id, weapon_id);
}
void myapp::consoleChooseMerk(Ogre::StringVector& vec) {

	_GAMELOG("consoleChooseMerk");

	if (vec.size() != 2) {
		_LOG("choose_merk <merk_id>");
		return;
	}

	int merk_id = Ogre::StringConverter::parseUnsignedInt(vec[1]);
	if (merk_id == 0) {
		_LOG("merk_id must be integer value between 1 and 127");
		return;
	}

	if (!m_pCurPlayer || !m_pCurPlayer->merk_exist(merk_id)) {
		_LOG("Merk with id " + boost::lexical_cast<Ogre::String>(int(merk_id)) + " wasn't found");
		return;
	}

	req_ChooseMerk(merk_id);
}
void myapp::consoleExitGame(Ogre::StringVector&) {
	req_ExitGame();
}
void myapp::consoleChangeViewRange(Ogre::StringVector& vec) {
#ifndef _DEBUG
	_LOG("Not allowed in release mode");
	return;
#endif
	if (!m_pActiveUnit || vec.size() != 2) return;
	int Range = Ogre::StringConverter::parseUnsignedInt(vec[1]);
	do_SetViewRange(m_pActiveUnit, Range);
}

//void myapp::_OGRELOG(const Ogre::String& msg) {
//	Ogre::LogManager::getSingletonPtr()->logMessage(msg);
//	//_GAMELOG(msg);
//}
//void myapp::_LOG(const Ogre::String& msg) {
//	_OGRELOG(msg);
//	_GAMELOG(msg);
//}
//void myapp::_GAMELOG(const Ogre::String& msg) {
//#ifdef ENABLE_GAME_LOG
//	if (!log_file.is_open()) {
//		if (!createGameLogFile())
//			return;
//	}
//	if (!m_bLogFileCreated) {
//		return;
//	}
//	std::time_t t = time(nullptr);
//	std::tm* aTm = localtime(&t);
//	log_file << std::setw(2) << std::setfill('0') << aTm->tm_hour << ":"
//		<< std::setw(2) << std::setfill('0') << aTm->tm_min <<":"
//		<< std::setw(2) << std::setfill('0') << aTm->tm_sec << ":" << " " << msg << std::endl;
////	delete aTm;
//#endif
//}

void myapp::handle_thread() {
	srand(time(0));
	_OGRELOG("io_service runned");
	m_Service.run();
	_OGRELOG("io_service stopped");
}
void myapp::forceRunService() {
	boost::thread( boost::bind(&myapp::handle_thread, this) );
}
myapp::sc_res myapp::start_connection(const std::string& IPv4, unsigned short port,
	const Ogre::String& username) 
{
	sc_res result = SC_OK;

	if (m_pConnection.get()) {
		if (m_pConnection->connected()) {
			result = SC_ALREADY_CONNECTED;
		}
	}

	boost::system::error_code err;
	boost::asio::ip::address adr = boost::asio::ip::address::from_string( IPv4, err);

	if ( err) {
		result |= SC_INVALID_IP;
	}

	if (result != SC_OK) {
		return result;
	}

	boost::asio::ip::tcp::endpoint ep( boost::asio::ip::address::from_string(IPv4), port);
	if (m_pConnection.get()) {
		m_pConnection->start(ep, username);
	} else {
		m_pConnection = Connection::start( m_Service, ep, username, m_pConsole);
	}

	if (m_Service.stopped()) {
		m_Service.reset();
	}
	boost::thread( boost::bind(&myapp::handle_thread, this) );

	do_RemoveAllUnits();
	do_ClearPlayers();

	return SC_OK;
}
void myapp::disconnect(void) {
	_GAMELOG("disconnect");
	if (connected()) {
		sendRequest(WANT_TO_DISCONNECT);
		if (m_pConnection) m_pConnection->stop();
	}  else {
		_LOG("Connection is not estabilished");
	}
	set_default_state();
	//_LOG("Disconnected");
}
void myapp::set_default_state() {
	do_RemoveAllUnits();
	remove_all_players();
	m_pCurPlayer = nullptr;
	if (m_pConnection) m_pConnection->set_player(nullptr);
	clear_lobby_list();
	//m_pCurPlayer->reset();
	deselectActiveUnit();
	unloadMap();
	unblockActivity();
}
void myapp::clear_lobby_list() {
	m_Lobbies.clear();
}
bool myapp::connected(void) {
	return m_pConnection.get() && m_pConnection->connected();
}

void myapp::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = m_pMouse->getMouseState();
	ms.width = width;
	ms.height = height;

	if (m_pCamera->getProjectionType() == Ogre::ProjectionType::PT_PERSPECTIVE) {
		m_pCamera->setAspectRatio(Ogre::Real(m_pViewport->getActualWidth()) / Ogre::Real(m_pViewport->getActualHeight()));
	}
	else {
		setCameraScale();
	}
}
void myapp::windowClosed(Ogre::RenderWindow* rw)
{
	//Only close for window that created OIS (the main window in these demos)
	if(rw == m_pRenderWindow)
	{
		if(m_pInputManager)
		{
			m_pInputManager->destroyInputObject( m_pMouse );
			m_pInputManager->destroyInputObject( m_pKeyboard );

			OIS::InputManager::destroyInputSystem(m_pInputManager);
			m_pInputManager = nullptr;
		}
	}
}

bool myapp::frameRenderingQueued(const Ogre::FrameEvent& evt) {

	if (m_pRenderWindow->isClosed()) {
		return false;
	}

	if (m_bShutDown) {
		disconnect();
		return false;
	}

	if (connected()) {
		if (m_pConnection->incoming(m_Msg)) {
			if (!parseMsg()) {
				_LOG("Invalid message from server");
				disconnect();	
			}
		}
	} else if (m_pGameMap || !m_Objects.empty()) {	// заглушка, на самом деле надо проверять,
													// что что-то не а дефолтном состоянии
		set_default_state();
	}

	getInput();

	//m_pTrayManager->frameRenderingQueued(evt);

	if (!m_pTrayManager->isDialogVisible())
	{
		m_pCameraMan->frameRenderingQueued(evt);   // if dialog isn't up, then update the camera

		updateParamsPanel();
	}

	//if (m_pKeyboard->isKeyDown(OIS::KC_ESCAPE)) {
	//	return false;
	//}

	for (auto it = AbstractUnit::g_Units.begin(); it != AbstractUnit::g_Units.end(); it++) {
		(*it)->checkStatus(evt.timeSinceLastFrame);
		if ((*it)->needUpdate()) {
			CellCoordinates new_coord = getCell((*it)->getPosition());
			m_pGameMap->moveUnit((*it)->getCoordinates(), new_coord);
			if (m_Players[(*it)->getPlayerID()].get_team() == m_pCurPlayer->get_team()) {
				//drawFOV(new_coord);
				//undrawFOV((*it)->getCoordinates());
				redrawFOV();
			}
			(*it)->setCoordinates(new_coord);
		}
		if (m_bCalculateDeviations)	calculateDeviations(*it);
	}

	if (m_pCurPlayer && m_pCurPlayer->is_in_game()) {
		updateMousePosition();
		set_entity_under_mouse_ray_clear();
	}

	//if (m_bNextTurn && !(AbstractUnit::busy_unit() && AbstractUnit::busy_unit()->isVisible()))
	//{
	//	m_bNextTurn = false;
	//	do_TurnBegin();
	//}

	if (m_bNextDoing && !m_DoList.empty()) {
		m_bNextDoing = false;
		m_DoList.front()();
		m_DoList.pop();
	} 

	if (!AbstractUnit::busy_unit()) {
		m_bNextDoing = true;
	} else {
		m_bNextDoing = false;
	}

	return true;
}
void myapp::getInput() {
	//m_pKeyboard->capture();
	//m_pMouse->capture();
	m_InputContext.capture();
}
bool myapp::keyPressed( const OIS::KeyEvent& arg ) {
	if (m_pTrayManager->isDialogVisible()) return true;   // don't process any more keys if dialog is up

	if (arg.key == OIS::KC_ESCAPE) {
		m_bShutDown = true;
		return true;
	}
	else if (arg.key == OIS::KC_F10) {
		m_pConsole->setVisible(!m_pConsole->isVisible());
		return true;
	} 

	//else if (arg.key == OIS::KC_M) {
	//	m_pCamera->setPosition(1751.12, 780, 2269.85);
	//}
	//else if (arg.key == OIS::KC_N) {
	//	for (auto it = m_Objects.begin(); it != m_Objects.end(); ++it) {
	//		it->second.setTransparency(0.0f);
	//	}
	//}
	//else if (arg.key == OIS::KC_B) {
	//	for (auto it = m_Objects.begin(); it != m_Objects.end(); ++it) {
	//		it->second.getEntity()->setVisible(false);
	//	}
	//}

	if (m_pConsole->isVisible()) {
		m_pConsole->onKeyPressed(arg);
		return true;
	}

	if (inCombat() && arg.key == OIS::KC_TAB) {
		centerCamera();
		return true;
	}

	overlayOnKeyDown(arg);
	m_pCameraMan->injectKeyDown(arg);

	return true;
}
bool myapp::keyReleased( const OIS::KeyEvent& arg ) {
	if (arg.key == OIS::KC_BACK) {
		m_pConsole->stopDeleting();
		return true;
	}
	if (m_pConsole->isVisible()) {
		m_pConsole->onKeyReleased(arg);
		return true;
	}
	m_pCameraMan->injectKeyUp(arg);
	return true;
}
bool myapp::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{

	if (m_pConsole->isVisible()) {
		return true;
	}

	// Left mouse button down
	if (id == OIS::MB_Left)
	{
		//m_pSelectionBox->startSelection(getMouseNormalizedCoordinates());

		m_bLMouseDown = true;

		m_bDontMoveSelectedUnit = false;

		if (!m_pActiveUnit) {
			if (selectObjectByRay(getMouseRay())) {
				if (!m_bActivityBlocked) m_bDontMoveSelectedUnit = true;
			}
			//} else if (m_pActiveUnit->isWalking()) {
			//	deselectActiveUnit();
		} else if (!m_bActivityBlocked && !m_pActiveUnit->isBusy()) {
			auto unit = getUnitByMouseRay();
			if (unit && unit->isAlive() && m_pCurPlayer->get_team() != m_Players[unit->getPlayerID()].get_team()) {
				m_bDontMoveSelectedUnit = true;
				if (!unit->isWalking()) {
					if (!req_Shoot(m_pActiveUnit, unit)) {
						unblockActivity();
					}
					//blockActivity();
					//if (!req_Shoot(m_pActiveUnit, unit)) {
					//	unblockActivity();
					//}
					//do_Shoot(m_pActiveUnit, unit);
					////
				}
			}
			else if (selectObjectByRay(getMouseRay())) {
				m_bDontMoveSelectedUnit = true;
			}
		}

		m_bLMouseDown = true;
	} 
	else if (id == OIS::MB_Right) {
		//std::pair<bool, Ogre::Vector3> res = getPosByMouse();
		//auto res = getCellByMouse();

		//if (res != CellCoordinates() && (!m_pActiveUnit || !m_pActiveUnit->isBusy())) {
		//	////
		//	blockActivity();
		//	if (!req_CreateRobot(res, m_pCurPlayer->get_id())) {
		//		unblockActivity();
		//	}
		//	//do_CreateRobot(res.second);
		//	////

		//	// это синеглазый огр по приколу, надо будет убрать 
		//	m_Objects.front().second.getEntity()->getSubEntity(0)->getMaterial()->setAmbient(0,0,1);
		//}
		if (!(m_pActiveUnit && m_pActiveUnit->isBusy())) {
			deselectActiveUnit();
		}
		m_bRMouseDown = true;
	} // else if
	return true;
}
bool myapp::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{

	if (m_pConsole->isVisible()) {
		return true;
	}

	if (m_bActivityBlocked) {
		return true;
	}

	if (id == OIS::MB_Left)
	{
		//if (performSelection(m_pSelectionBox) == PS_SMALLBOX) {
		//	if (m_pActiveSelectedGroup && m_pActiveSelectedGroup->size() > 0) {

		//		std::pair<bool, Ogre::Vector3> res = getPosByMouse();

		//		if (res.first && !notInPlane(res.second)) {
		//			m_pActiveSelectedGroup->moveSelectedTo(res.second);
		//		}

		//	}
		//}			

		if (m_pActiveUnit && !m_pActiveUnit->isBusy() && !m_bDontMoveSelectedUnit) {
			////
			if (!req_MoveSelectedUnit()) {
				unblockActivity();
			}
			//do_MoveSelectedUnit();
			////
		}
		else {
			m_bDontMoveSelectedUnit = false;
		}

		m_bLMouseDown = false;
	}

	// Right mouse button down
	else if (id == OIS::MB_Right) {
		m_bRMouseDown = false;
	}

	//	m_pCameraMan->injectMouseUp(arg, id);
	return true;
}
bool myapp::mouseMoved( const OIS::MouseEvent &arg )
{
	if (m_pTrayManager->injectMouseMove(arg)) return true; //1.9.0
	//if (m_pTrayManager->injectPointerMove(arg)) return true; //2.0.0

	//CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);

	//if (m_pSelectionBox->Selecting()) {
	//	m_pSelectionBox->update(getMouseNormalizedCoordinates());		
	//}

	if (m_pConsole->isVisible()) {
		m_pConsole->mouseMoved(arg);
		return true;
	}

	return true;
}
void myapp::set_entity_under_mouse_ray_clear() {
	//bool mMovableFound(false);

	//Ogre::RaySceneQuery* mRayScnQuery(nullptr);

	//mRayScnQuery->setRay(getMouseRay());
	//mRayScnQuery->setSortByDistance(true);

	//Ogre::RaySceneQueryResult& result = mRaySceneQuery->execute();
	//Ogre::RaySceneQueryResult::iterator it = result.begin();

	//for ( ; it != result.end(); it++)
	//{
	//	mMovableFound =
	//		it->movable &&
	//		it->movable->getName() != "" &&
	//		it->movable->getName() != "PlayerCam";

	//	if (mMovableFound)
	//	{
	//		it->movable->getParentNode()->
	//		//mCurObject = it->movable->getParentSceneNode();
	//		break;
	//	}
	//}

	//CellCoordinates cc(getCellByMouse());

	if (!m_pCurPlayer || !m_pCurPlayer->is_in_game())
		return;

	Ogre::Ray ray(getMouseRay());
	Ogre::Ray unit_ray;
	if (m_pCurPlayer->has_units()) {
		unit_ray.setOrigin(m_pCamera->getPosition());
		unit_ray.setDirection(m_pGameMap->getMyFirstUnit()->getEntity()->getWorldBoundingSphere().getCenter()
			- m_pCamera->getPosition());
	}
	std::vector<CellCoordinates> ccs(8, CellCoordinates());
	int k = 0;


	for (auto it = m_Objects.begin(); it != m_Objects.end(); ++it) {
		auto box = it->second.getEntity()->getWorldBoundingBox();
		//if (it->second.getMask().getM() == 1) {
		//auto sphere = it->second.getEntity()->getWorldBoundingSphere();
		//sphere.setRadius(sphere.getRadius()*0.3f);
		//if (it->second.getName()[5] == 'X') {
		//	box.scale(Ogre::Vector3(1.0f, 1.0f, 1.0f));
		//} else {
		//	box.scale(Ogre::Vector3(1.0f, 1.0f, 1.0f));
		//}
		////if (ray.intersects(sphere).first) {
		////if (ray.intersects(box).first || unit_ray.intersects(box).first) {
		//////if (abs(cc.x - it->first.x) < 2 && abs(cc.y - it->first.y) < 2) {
		////	it->second.setTransparency(0.4f);
		////} else {
		////	it->second.setTransparency(0.0f);
		//}
		//} else {
		if (ray.intersects(box).first || unit_ray.intersects(box).first) {
			//if (abs(cc.x - it->first.x) < 2 && abs(cc.y - it->first.y) < 2) {
			ccs[k++] = it->first;
			//}
		}
		//if (m_mouseCellCoordinates == it->first) {
		//	ccs[k++] = it->first;
		//}
	}
	for (auto it = m_Objects.begin(), e = m_Objects.end(); it != e; ++it) {
		bool transp = false;
		for (int i = 0; i < k; ++i) {
			if (abs(ccs[i].x - it->first.x) < 2 && abs(ccs[i].y - it->first.y) < 2) {
				transp = true;
				break;
			}
		}
		if (transp) {
			//it->second.getEntity()->getParentSceneNode()->showBoundingBox(true);
			it->second.setTransparency(0.6f);
		} else {
			//it->second.getEntity()->getParentSceneNode()->showBoundingBox(false);
			it->second.setTransparency(0.0f);
		}
	}
}

void myapp::overlayOnKeyDown(const OIS::KeyEvent& arg) {
	if (arg.key == OIS::KC_F)   // toggle visibility of advanced frame stats
	{
		m_pTrayManager->toggleAdvancedFrameStats();
	}
	else if (arg.key == OIS::KC_G)   // toggle visibility of even rarer debugging details
	{
		//if (m_pParamsPanel->getTrayLocation() == OgreBites::TL_NONE)
		//{
		//	m_pTrayManager->moveWidgetToTray(m_pParamsPanel, OgreBites::TL_TOPLEFT, 0);
		//	m_pParamsPanel->show();
		//}
		//else
		//{
		//	m_pTrayManager->removeWidgetFromTray(m_pParamsPanel);
		//	m_pParamsPanel->hide();
		//}
		m_pTrayManager->removeWidgetFromTray(m_pParamsPanels[m_nActivePanelIndex]);
		m_pParamsPanels[m_nActivePanelIndex]->hide();

		++m_nActivePanelIndex;
		if (m_nActivePanelIndex >= PANELS_COUNT) {
			m_nActivePanelIndex = 0;
		}
		if (m_nActivePanelIndex == 3) {
			m_bUnitInfoNeedsToBeUpdated = true;
		}
		m_pTrayManager->moveWidgetToTray(m_pParamsPanels[m_nActivePanelIndex], OgreBites::TL_TOPLEFT, 0);
		m_pParamsPanels[m_nActivePanelIndex]->show();

	}
	else if (arg.key == OIS::KC_T)   // cycle polygon rendering mode
	{
		Ogre::String newVal;
		Ogre::TextureFilterOptions tfo;
		unsigned int aniso;

		switch (m_pParamsPanels[0]->getParamValue(9).asUTF8()[0])
		{
		case 'B':
			newVal = "Trilinear";
			tfo = Ogre::TFO_TRILINEAR;
			aniso = 1;
			break;
		case 'T':
			newVal = "Anisotropic";
			tfo = Ogre::TFO_ANISOTROPIC;
			aniso = 8;
			break;
		case 'A':
			newVal = "None";
			tfo = Ogre::TFO_NONE;
			aniso = 1;
			break;
		default:
			newVal = "Bilinear";
			tfo = Ogre::TFO_BILINEAR;
			aniso = 1;
		}

		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
		Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
		m_pParamsPanels[0]->setParamValue(9, newVal);
	}
	else if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
	{
		Ogre::String newVal;
		Ogre::PolygonMode pm;

		switch (m_pCamera->getPolygonMode())
		{
		case Ogre::PM_SOLID:
			newVal = "Wireframe";
			pm = Ogre::PM_WIREFRAME;
			break;
		case Ogre::PM_WIREFRAME:
			newVal = "Points";
			pm = Ogre::PM_POINTS;
			break;
		default:
			newVal = "Solid";
			pm = Ogre::PM_SOLID;
		}

		m_pCamera->setPolygonMode(pm);
		m_pParamsPanels[0]->setParamValue(10, newVal);
	}
	else if(arg.key == OIS::KC_F5)   // refresh all textures
	{
		Ogre::TextureManager::getSingleton().reloadAll();
	}
	else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
	{
		m_pRenderWindow->writeContentsToTimestampedFile("screenshot", ".png");
	}

}
void myapp::updateParamsPanel() {
	Ogre::Ray mouseRay = m_pTrayManager->getCursorRay(m_pCamera);
	std::pair<bool, Ogre::Real> res = mouseRay.intersects(m_Plane);

	Ogre::Vector3 intersection = mouseRay.getPoint(res.second);

	if (m_pParamsPanels[0]->isVisible())   // if details panel is visible, then update its contents
	{			
		m_pParamsPanels[0]->setParamValue(0, Ogre::StringConverter::toString(m_pCamera->getDerivedPosition().x));
		m_pParamsPanels[0]->setParamValue(1, Ogre::StringConverter::toString(m_pCamera->getDerivedPosition().y));
		m_pParamsPanels[0]->setParamValue(2, Ogre::StringConverter::toString(m_pCamera->getDerivedPosition().z));
		m_pParamsPanels[0]->setParamValue(4, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().w));
		m_pParamsPanels[0]->setParamValue(5, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().x));
		m_pParamsPanels[0]->setParamValue(6, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().y));
		m_pParamsPanels[0]->setParamValue(7, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().z));
	} else if (m_pParamsPanels[1]->isVisible()) {
		m_pParamsPanels[1]->setParamValue(0, Ogre::StringConverter::toString(intersection.x));
		m_pParamsPanels[1]->setParamValue(1, Ogre::StringConverter::toString(intersection.y));
		m_pParamsPanels[1]->setParamValue(2, Ogre::StringConverter::toString(intersection.z));

		m_pParamsPanels[1]->setParamValue(4, Ogre::StringConverter::toString(getCell(intersection).x)
			+ "," + Ogre::StringConverter::toString(getCell(intersection).y));

		if (m_pActiveUnit) {
			m_pParamsPanels[1]->setParamValue(5, Ogre::StringConverter::toString(
				m_pGameMap->line_of_sight_exists(getCell(m_pActiveUnit->getPosition()), getCell(intersection))));
		} else {
			m_pParamsPanels[1]->setParamValue(5, "false");
		}
	} else if (connected() && m_pParamsPanels[2]->isVisible()) {
		m_pParamsPanels[2]->setParamValue(0, Ogre::StringConverter::toString(m_pConnection->getSendedBytesCount())
			+ " bytes");
		m_pParamsPanels[2]->setParamValue(1, Ogre::StringConverter::toString(m_pConnection->getRecievedBytesCount())
			+ " bytes");
		m_pParamsPanels[2]->setParamValue(2, Ogre::StringConverter::toString(m_pConnection->getSendedBytesCount() 
			+ m_pConnection->getRecievedBytesCount()) + " bytes");
		m_pParamsPanels[2]->setParamValue(4, Ogre::StringConverter::toString(m_pConnection->get_ping()));
	} else if (m_pParamsPanels[3]->isVisible() && m_bUnitInfoNeedsToBeUpdated) {
		m_bUnitInfoNeedsToBeUpdated = false;
		if (m_pUnitUnderMouseRay) {
			m_pParamsPanels[3]->setParamValue(0, get_nick( m_pUnitUnderMouseRay->getPlayerID() ));
			m_pParamsPanels[3]->setParamValue(1, m_pUnitUnderMouseRay->getName() );
			m_pParamsPanels[3]->setParamValue(2, _DATA.getTypeString( m_pUnitUnderMouseRay->getType() ));
			//m_pParamsPanels[3]->setParamValue(3, _DATA.getClassString( m_pUnitUnderMouseRay->getClass() ));
			m_pParamsPanels[3]->setParamValue(3, Ogre::StringConverter::toString( m_pUnitUnderMouseRay->getHP() ));
			m_pParamsPanels[3]->setParamValue(4, Ogre::StringConverter::toString( m_pUnitUnderMouseRay->getAC() ));
			m_pParamsPanels[3]->setParamValue(5, Ogre::StringConverter::toString( m_pUnitUnderMouseRay->getAP() ));
			m_pParamsPanels[3]->setParamValue(6, _DATA.getWeaponString( m_pUnitUnderMouseRay->getWeapon() ));		
			m_pParamsPanels[3]->setParamValue(7, _DATA.getArmorString( m_pUnitUnderMouseRay->getArmor() ));	
		} else if (m_pActiveUnit) {
			m_pParamsPanels[3]->setParamValue(0, get_nick( m_pActiveUnit->getPlayerID() ));
			m_pParamsPanels[3]->setParamValue(1, m_pActiveUnit->getName() );
			m_pParamsPanels[3]->setParamValue(2, _DATA.getTypeString( m_pActiveUnit->getType() ));
			//m_pParamsPanels[3]->setParamValue(3, _DATA.getClassString( m_pActiveUnit->getClass() ));
			m_pParamsPanels[3]->setParamValue(3, Ogre::StringConverter::toString( m_pActiveUnit->getHP() ));
			m_pParamsPanels[3]->setParamValue(4, Ogre::StringConverter::toString( m_pActiveUnit->getAC() ));
			m_pParamsPanels[3]->setParamValue(5, Ogre::StringConverter::toString( m_pActiveUnit->getAP() ));
			m_pParamsPanels[3]->setParamValue(6, _DATA.getWeaponString( m_pActiveUnit->getWeapon() ));		
			m_pParamsPanels[3]->setParamValue(7, _DATA.getArmorString( m_pActiveUnit->getArmor() ));
		} else {
			m_pParamsPanels[3]->setParamValue(0, "?");
			m_pParamsPanels[3]->setParamValue(1, "?");
			m_pParamsPanels[3]->setParamValue(2, "?");
			m_pParamsPanels[3]->setParamValue(3, "?");
			m_pParamsPanels[3]->setParamValue(4, "?");
			m_pParamsPanels[3]->setParamValue(5, "?");
			m_pParamsPanels[3]->setParamValue(6, "?");
			m_pParamsPanels[3]->setParamValue(7, "?");
			//m_pParamsPanels[3]->setParamValue(8, "?");
		}
	}
}
void myapp::calculateDeviations(AbstractUnit* unit) {
	Ogre::uint32 res = notInPlane(unit);
	if (res) {
		unit->stop();
		if (res & NIP_SMALL_X) {
			unit->translate(Ogre::Vector3::UNIT_X);
		}
		if (res & NIP_SMALL_Z) {
			unit->translate(Ogre::Vector3::UNIT_Z);
		}
		if (res & NIP_BIG_X) {
			unit->translate(Ogre::Vector3::NEGATIVE_UNIT_X);
		}
		if (res & NIP_BIG_Z) {
			unit->translate(Ogre::Vector3::NEGATIVE_UNIT_Z);
		}
	}
}
void myapp::updateMousePosition() {

	if (m_pActiveUnit && m_pActiveUnit->isBusy()) {
		return;
	}

	CellCoordinates mouse_cell = getCellByMouse();
	if (m_mouseCellCoordinates != mouse_cell) {
		m_mouseCellCoordinates = mouse_cell;

		//printLightMap(mouse_cell);

	}

	if (!m_bActivityBlocked && m_bFindPath) {// && m_mouseCellCoordinates != getCellByMouse() || m_bForceFindPath) {

		//m_bForceFindPath = false;

		//m_mouseCellCoordinates = getCellByMouse();

		if (m_pActiveUnit) {// && !m_pActiveUnit->isBusy()) {
			//CellCoordinates unitPos = getCell(m_pActiveUnit->getPosition());
			CellCoordinates unitPos = m_pActiveUnit->getCoordinates();

			if (m_mouseCellCoordinates != unitPos) {
				clearPath();
				if (m_pGameMap->getPath(unitPos, m_mouseCellCoordinates, m_Path)) {
					drawUnitPath();
				}
			}
		}
	}
	AbstractUnit* unit = getUnitByMouseRay();
	if (m_pActiveUnit) {
		if (unit && unit->isAlive()) {
			if (m_bFindPath) {
				clearPath();
				m_bFindPath = false;
			}
			auto Distance = GameMap::calculate_distance(m_pActiveUnit->getCoordinates(), unit->getCoordinates());
			if (!m_bActivityBlocked && !m_pActiveUnit->isBusy(false) 
				&& Distance	<= _DATA.getMaxDistance(m_pActiveUnit->getWeapon())
				&& Distance	>= _DATA.getMinDistance(m_pActiveUnit->getWeapon())
				&& m_pCurPlayer->get_team() != m_Players[unit->getPlayerID()].get_team()
				&& m_pGameMap->line_of_sight_exists(m_pActiveUnit->getCoordinates(), unit->getCoordinates())) {
					setCursorCrosshair();
					if (!m_pActiveUnit->isRotating()) {
						m_pActiveUnit->rotateToDirection(unit->getPosition() - m_pActiveUnit->getPosition(), 0.4);
					}
			} else {
				//m_bForceFindPath = true;
				setCursorArrow();
			}
		}
		else {
			m_bFindPath = true;
			setCursorArrow();
		}
	}
}

// костыль костыльный
//bool myapp::inUnion(int t1, int t2) {
//	return m_pGameMap->inUnion(t1, t2);
//}
//bool myapp::inUnion(AbstractUnit* unit1, AbstractUnit* unit2) {
//	return m_pGameMap->inUnion(unit1->getPlayerID(), unit2->getPlayerID());
//}

bool myapp::hewston_we_have_a_problem(void) {
	_LOG("Some shit just happened: got the GTFO msg from the server");
	disconnect();
	return false;
}
bool myapp::sendRequest(char command) {
	return sendRequest(std::vector<char>(1, command));
}
bool myapp::sendRequest(const std::vector<char>& msg) {
	if (!connected()) {
		_LOG("Request wasn't sent: not connected");
		return false;
	}
	if (!m_pCurPlayer || !m_pCurPlayer->is_online()) {
		_LOG("Request wasn't sent: you are not logged in");
		return false;
	}
	return m_pConnection->post(msg);
}
bool myapp::parseMsg() {
	int i = 0;
	while (m_Msg[i] != REQUEST_TYPE::END_MESSAGE) {
		bool res = false;
		if			(m_Msg[i] == REQUEST_TYPE::DENIED) {
			return hewston_we_have_a_problem();
		} else if	(m_Msg[i] == REQUEST_TYPE::MOVE) {
			res = true;
			parse_MoveUnit(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::UNBLOCK) {
			res = true;
			unblockActivity();
			++i;
		} else if	(m_Msg[i] == REQUEST_TYPE::BLOCK) {
			res = true;
			blockActivity();
			++i;
		} else if	(m_Msg[i] == REQUEST_TYPE::TURN_END) {
			res = true;
			parse_TurnEnd(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::TURN_BEGIN) {
			res = true;
			parse_TurnBegin(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::ANS_SHOOT) {
			res = true;
			parse_Shoot(m_Msg, i);
		} else if	(m_Msg[i] == REQUEST_TYPE::CREATE_UNIT) {
			res = true;
			parse_CreateUnit(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::PLAYER_IN_GAME) {
			res = true;
			parse_PlayerInGame(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::CREATE_UNION) {
			res = false; // currently a bug
			//parse_CreateUnion(m_Msg, i);
		} else if	(m_Msg[i] == REQUEST_TYPE::NEW_PLAYER) {
			res = true;
			parse_NewPlayer(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::SET) {
			res = true;
			parse_Set(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::LOGIN) {
			res = true;
			parse_Login(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::LOAD_MAP) {
			res = true;
			parse_LoadMap(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::CREATE_LOBBY) {
			res = true;
			parse_CreateLobby(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::DROP_FROM_LOBBY) {
			res = true;
			parse_DropFromLobby(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::PLAYER_NAME) {
			res = true;
			parse_PlayerName(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::LOBBY_NEW_PLAYER) {
			res = true;
			parse_LobbyNewPlayer(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::JOIN_LOBBY) {
			res = true;
			parse_JoinLobby(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::PLAYERS_IN_LOBBY) {
			res = true;
			parse_PlayersInLobby(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::PLAYER_LIST) {
			res = true;
			parse_PlayerList(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::LOBBY_LIST) {
			res = true;
			parse_LobbyList(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::DROP_PLAYER) {
			res = true;
			parse_DropPlayer(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::DROP_LOBBY) {
			res = true;
			parse_DropLobby(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::CHANGE_MAP) {
			res = true;
			parse_ChangeMap(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::CREATE_MERK) {
			res = true;
			parse_CreateMerk(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::DELETE_MERK) {
			res = true;
			parse_DeleteMerk(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::CHOOSE_MERK) {
			res = true;
			parse_ChooseMerk(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::CHANGE_WEAPON) {
			res = true;
			parse_ChangeWeapon(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::MERK_LIST) {
			res = true;
			parse_MerkList(m_Msg, ++i);
		} else if	(m_Msg[i] == REQUEST_TYPE::EXIT_GAME) {
			res = true;
			parse_ExitGame(m_Msg, ++i);
		}
		if (res == false) {
			return false;
		}
	}
	return true;
}
void myapp::parse_DropFromLobby(const std::vector<char>& msg, int& i) {
	int player_id = INT(msg[i]);
	i += sizeof(int);

	do_DropFromLobby(player_id);
}
void myapp::parse_Login(const std::vector<char>& msg, int& i) {
	char ans = msg[i++];
	switch (ans) {
	case SERVER_ANSWER::LOGIN_OK: 
		{
			int player_id = INT(msg[i]);
			i += sizeof(int);

			char len = msg[i++];

			m_Players[player_id] = Player(player_id, Ogre::String(&msg[i],&msg[i+len]));

			m_pCurPlayer = &m_Players[player_id];

			i += len;

			/* этого тут больше нет
			int player_id = msg[i++];
			m_pConnection->set_player_id(player_id);
			*/

			//m_pGameMap->setPlayerID(player_id);
			//m_pConnection->set_logged_in(true);
			m_pCurPlayer->set_online(true);
			//m_pConnection->set_player(m_pCurPlayer->shared_from_this()); // не работает,
			// т.к. m_CurPlayer создан на стэке, а shared_ptr попытается удалить его
			// при своём исчезновении. Поэтому в данном случае нужно вызывать конструктор явно
			// (т.е. возможность выстрелить себе в ногу остаётся, но не даётся так просто)
			m_pConnection->set_player(m_pCurPlayer); // переделал, теперь хранится просто указатель
			_LOG("Succesfully logged in as: " + m_pCurPlayer->get_nick() );
			//req_PlayerList();	// само отсылается сервером
			req_LobbyList();
			//req_MerkList();	// само отсылается сервером
		} break;
	case SERVER_ANSWER::LOGIN_FAILED: 
		{
			_LOG("Incorrect login data");
			disconnect();	
		} break;
	case SERVER_ANSWER::LOGIN_ALREADY: 
		{
			_LOG("This account already is in use");
			disconnect();
		} break;
	}
}
void myapp::parse_CreateUnion(const std::vector<char>& msg, int& i) {
	char count = msg[i+1];
	std::list<int> Union;
	for (int j = 0; j < count; j += sizeof(int)) {
		Union.push_back(*((int*)(&msg[i+2+j])));
	}
	do_CreateUnion( std::move(Union) );
	i += 2 + count*sizeof(int);
}
void myapp::parse_CreateUnit(const std::vector<char>& msg, int& i) {
	//char type_id = msg[i++];
	//if (type_id == UNIT_TYPE::HERO) {
	UnitDef ud = UD(msg[i]);
	i += sizeof(ud);
	int len = msg[i++];
	Ogre::String name(&msg[i], len);// = msg.substr(i, len);
	if (len == 0) {
		name = _DATA.getDefaultName(ud.type_id);
	}
	i += len;
	//ud.type_id = msg[i++];
	//ud.Class = msg[i++];
	//ud.PlayerID = msg[i++];
	//ud.HP = *( (short*)&msg[i] );
	//i += sizeof(short);
	//ud.AP = msg[i++];
	//ud.Weapon = msg[i++];
	//ud.Armor = msg[i++];
	//// костыль мод он
	//ud.ViewRange = _DATA.getDefaultViewRange(ud.type_id);
	//// костыль мод офф
	CellCoordinates where(msg[i], msg[i+1]);
	i += 2;
	CellCoordinates looks_to(msg[i], msg[i+1]);
	i += 2;
	short cur_hp = SHORT(msg[i]);
	i += sizeof(short);
	char cur_ap = msg[i++];
	//do_CreateHero(Name, type_id, Class, Weapon, Armor, AP, HP, PlayerID, where, looks_to);
	m_DoList.push( boost::bind( &myapp::do_CreateHero, this, name, ud, where, looks_to, cur_hp, cur_ap ) );
	//} else {
	////UnitDef ud;
	////ud.type_id = type_id;
	////ud.Class = msg[i++];
	////ud.PlayerID = msg[i++];
	////ud.HP = *( (short*)&msg[i] );
	////i += sizeof(short);
	////ud.AP = msg[i++];
	////ud.Weapon = msg[i++];
	////ud.Armor = msg[i++];
	////// костыль мод он
	////ud.ViewRange = _DATA.getDefaultViewRange(ud.type_id);
	////// костыль мод офф
	//UnitDef ud = *( (UnitDef*)&msg[i] );
	////ud.type_id = msg[i++];
	////ud.Class = msg[i++];
	////ud.PlayerID = msg[i++];
	////ud.HP = *( (short*)&msg[i] );
	////i += sizeof(short);
	////ud.AP = msg[i++];
	////ud.Weapon = msg[i++];
	////ud.Armor = msg[i++];
	////// костыль мод он
	////ud.ViewRange = _DATA.getDefaultViewRange(ud.type_id);
	////// костыль мод офф
	//i += sizeof(ud);
	//CellCoordinates where(msg[i], msg[i+1]);
	//i += 2;
	//CellCoordinates looks_to(msg[i], msg[i+1]);
	//i += 2;
	////do_CreateUnit(type_id, Class, Weapon, Armor, AP, HP, PlayerID, where, looks_to);
	//m_DoList.push( boost::bind( &myapp::do_CreateUnit, this, ud, where, looks_to ) );
	//}
}
void myapp::parse_Shoot(const std::vector<char>& msg, int& i) {
	CellCoordinates who_c(msg[i+1], msg[i+2]),
		in_who_c(msg[i+3], msg[i+4]);
	auto who = m_pGameMap->getUnit(who_c), 
		in_who = m_pGameMap->getUnit(in_who_c);
	short dmg = *((short*)&msg[i+5]);

	m_DoList.push( boost::bind( &myapp::do_Shoot, this, who, in_who, dmg ) );
	//do_Shoot(who, in_who, dmg);

	i += 7;
}
void myapp::parse_MoveUnit(const std::vector<char>& msg, int& i) {
	int N = msg[i++];
	CellCoordinates who(msg[i], msg[i+1]);
	i += 2;
	m_Path.clear();
	for (int j = 0; j < N; ++j) {
		m_Path.push_back(CellCoordinates(msg[i + 2*j], msg[i+1 + 2*j]));
	}

	for (int j = 0; j < N / 2; ++j) {
		auto temp = m_Path[j];
		m_Path[j] = m_Path[N - j - 1];
		m_Path[N - j - 1] = temp;
	}
	//do_MoveUnit(m_pGameMap->getUnit(robot), m_Path);
	m_DoList.push( boost::bind( &myapp::do_MoveUnit, this, m_pGameMap->getUnit(who), m_Path ) );

	i += N * 2;
}
void myapp::parse_TurnBegin(const std::vector<char>& msg, int& i) {
	m_CurrentActivePlayer = INT(msg[i]);
	i += sizeof(int);
	if (m_CurrentActivePlayer == m_pCurPlayer->get_id()) {
		//do_TurnBegin();
		m_DoList.push( boost::bind( &myapp::do_TurnBegin, this ) );
	} else {
		blockActivity();
		_LOG(get_nick(m_CurrentActivePlayer) + "\'s turn");
	}
}
void myapp::parse_TurnEnd(const std::vector<char>& msg, int& i) {
	int player_id = INT(msg[i]);
	i += sizeof(int);
	if (player_id == m_pCurPlayer->get_id()) {
		do_TurnEnd();
		//m_DoList.push( boost::bind( &myapp::do_TurnEnd, this ) );
	} else {
		_LOG("End of " + get_nick(player_id) + "\'s turn");
	}
}
void myapp::parse_Set(const std::vector<char>& msg, int& i) {
	CellCoordinates who(msg[i], msg[i+1]);
	i += 2;
	char Property = msg[i++];
	short Value = SHORT(msg[i]);
	i += 2;
	//boost::function< void() > method = boost::bind( &myapp::do_Set, this, m_pGameMap->getUnit(who), Property, Value );
	//do_Set(m_pGameMap->getUnit(who), Property, Value);
	//method();
	m_DoList.push( boost::bind( &myapp::do_Set, this, who, Property, Value ) );
}
void myapp::parse_NewPlayer(const std::vector<char>& msg, int& i) {
	int player_id = INT(msg[i]);
	i += sizeof(int);
	int len = msg[i++];
	add_player(player_id, Ogre::String(&msg[i],&msg[i+len]));
	i += len;
}
void myapp::parse_LoadMap(const std::vector<char>& msg, int& i) {
	//int len = msg[i++];
	//Ogre::String map = msg.substr(i, len);
	//i += len;
	//loadMap(std::move(map));
	m_pCurPlayer->set_is_in_game(true);
	m_pCurPlayer->set_game_id(msg[i]);
	loadMap(msg[i++]);
}
void myapp::parse_CreateLobby(const std::vector<char>& msg, int& i) {
	int host_id = INT(msg[i]);
	i += sizeof(int);
	char map_id = msg[i++];
	int lobby_id = INT(msg[i]);
	i += sizeof(int);

	do_CreateLobby(host_id, map_id, lobby_id);
}
void myapp::parse_LobbyNewPlayer(const std::vector<char>& msg, int& i) {
	int player_id = INT(msg[i]);
	i += sizeof(int);
	char team = msg[i++];
	i++; // на цвет
	do_LobbyNewPlayer(player_id, team);
}
void myapp::parse_PlayerName(const std::vector<char>& msg, int& i) {
	int player_id = INT(msg[i]);
	i += sizeof(int);
	int len = msg[i++];
	do_ChangePlayerName(player_id, Ogre::String(&msg[i],&msg[i+len]));
	i += len;
}
void myapp::parse_PlayersInLobby(const std::vector<char>& msg, int& i) {
	int players_count = msg[i++];
	for (int j = 0; j < players_count; ++j) {
		int player_id = INT(msg[i]);
		i += sizeof(int);
		do_LobbyNewPlayer(player_id, msg[i++], false);
	}
	show_players_in_lobby();
}
void myapp::parse_JoinLobby(const std::vector<char>& msg, int& i) {
	char ans(msg[i++]);
	if (ans == SERVER_ANSWER::LOGIN_OK) {
		int lobby_id = INT(msg[i]);
		i += sizeof(int);
		do_JoinLobby(lobby_id);
	} else if (ans == SERVER_ANSWER::LOGIN_FAILED) {
		_LOG("Failed to join lobby");
	} else {
		_LOG("You're already in lobby; exit first");
	}
}
void myapp::parse_PlayerList(const std::vector<char>& msg, int& i) {
	int player_count = INT(msg[i]);
	i += sizeof(int);
	//m_Players.clear();
	remove_all_players();
	for (int j = 0; j < player_count; ++j) {
		int player_id = INT(msg[i]);
		i += sizeof(int);
		char name_length = msg[i++];
		add_player(player_id, Ogre::String(&msg[i], &msg[i+name_length]), false);
		i += name_length;
	}
	show_player_list();
}
void myapp::parse_LobbyList(const std::vector<char>& msg, int& i) {
	int lobby_count = INT(msg[i]);
	i += sizeof(int);
	m_Lobbies.clear();
	for (int j = 0; j < lobby_count; ++j) {
		int lobby_id = INT(msg[i]);
		i += sizeof(int);
		int host_id = INT(msg[i]);
		i += sizeof(int);
		char map_id = msg[i++];
		add_lobby(host_id, map_id, lobby_id);
	}
	show_lobby_list();
}
void myapp::parse_DropPlayer(const std::vector<char>& msg, int& i) {
	do_RemovePlayer(INT(msg[i]));
	i += sizeof(int);
}
void myapp::parse_DropLobby(const std::vector<char>& msg, int& i) {
	do_DropLobby(INT(msg[i]));
	i += sizeof(int);
}
void myapp::parse_ChangeMap(const std::vector<char>& msg, int& i) {
	int lobby_id = INT(msg[i]);
	i += sizeof(int);
	do_ChangeMap(lobby_id, msg[i++]);
}
void myapp::parse_CreateMerk(const std::vector<char>& msg, int& i) {

	_GAMELOG("parse_CreateMerk");

	/*
	1) CREATE_MERK
	2) merk_id
	3) UnitDef
	4) количество символов в имени
	5) имя
	*/
	int merk_id = INT(msg[i]);
	i += sizeof(int);
	UnitDef ud = *( (UnitDef*)(&msg[i]) );
	i += sizeof(UnitDef);
	char name_len = msg[i++];
	Ogre::String name(&msg[i], &msg[i+name_len]);
	i += name_len;

	do_CreateMerk(merk_id, ud, name);
}
void myapp::parse_DeleteMerk(const std::vector<char>& msg, int& i) {

	_GAMELOG("parse_DeleteMerk");

	int merk_id = INT(msg[i]);
	i += sizeof(int);
	do_DeleteMerk(merk_id);
}
void myapp::parse_ChooseMerk(const std::vector<char>& msg, int& i) {

	_GAMELOG("parse_ChooseMerk");

	int merk_id = INT(msg[i]);
	i += sizeof(int);
	do_ChooseMerk(merk_id);
}
void myapp::parse_ChangeWeapon(const std::vector<char>& msg, int& i) {

	_GAMELOG("parse_ChangeWeapon");

	int merk_id = INT(msg[i]);
	i += sizeof(int);
	char weapon_id = msg[i++];
	do_ChangeWeapon(merk_id, weapon_id);
}
void myapp::parse_MerkList(const std::vector<char>& msg, int& i) {

	_GAMELOG("parse_MerkList");

	int count = msg[i++];
	for (int j = 0; j < count; ++j) {
		int merk_id = INT(msg[i]);
		i += sizeof(int);
		UnitDef ud = *( (UnitDef*)(&msg[i]) );
		i += sizeof(UnitDef);
		char name_len = msg[i++];
		Ogre::String name(&msg[i], &msg[i+name_len]);
		i += name_len;

		do_CreateMerk(merk_id, ud, name, false);

		if (j == 0) {
			m_pCurPlayer->set_merk_id(merk_id);
		}
	}
	show_merk_list();
}
void myapp::parse_ExitGame(const std::vector<char>& msg, int& i) {
	_GAMELOG("parse_ExitGame");
	do_RemoveAllUnits();
	unloadMap();
	if (m_pCurPlayer) {
		m_pCurPlayer->set_game_id(-1);
		m_pCurPlayer->set_is_in_game(false);
	}
	_LOG("Dropped from the game");
}
void myapp::parse_PlayerInGame(const std::vector<char>& msg, int& i) {
	int player_id = INT(msg[i]);
	i += sizeof(int);
	auto player = m_Players.find(player_id);

	assert_me(player != m_Players.end(), "trying to add unexisted player to the game");
	player->second.set_is_in_game(true);
	player->second.set_team(msg[i++]);

	assert_me(m_pCurPlayer, "No current player and game has started");
	if (player_id == m_pCurPlayer->get_id()) {

		assert_me(m_pGameMap, "No map and game has started");
		m_pGameMap->setTeam(msg[i-1]);

	}

	player->second.set_online(msg[i++] == 1);
	i++; // тут ещё один байт с информацией о существовании юнитов у этого игрока
}


bool myapp::req_TurnEnd() {
	_GAMELOG("req_TurnEnd");
	if (!connected()) {
		return false;
	}
	if (!m_pCurPlayer || !m_pCurPlayer->is_in_game()) {
		return false;
	}
	_LOG("Sending TURN_END request");

	blockActivity();
	//char req = TURN_END;
	//return sendRequest(Ogre::String(&req,1));
	return sendRequest(TURN_END);
}
bool myapp::req_MoveSelectedUnit() {
	_GAMELOG("req_MoveSelectedUnit");
	
	if (m_bActivityBlocked) {
		return false;
	}

	blockActivity();

	if (m_pPathLine->getNumPoints() == 0) {
		return false;
	}

	if (!inCombat()) {
		_LOG("Can't move unit: in combat");
		return false;
	}

	if (!m_Path.size() || !m_pGameMap->passable(*m_Path.begin(), true)) {
		_LOG("Can't move unit: cell is blocked");
		return false;
	}

	if (m_pActiveUnit == nullptr) {
		_LOG("Can't move unit: choose active unit first");
		return false;
	}

	if (m_pActiveUnit->getAP() <= 0) {
		_LOG("Can't move unit: don't have active points");
		return false;
	}

	if (m_Path.size() > m_pActiveUnit->getAP()) {
		std::vector<CellCoordinates> Path;
		//Path.reserve( 31 );
		Path.resize(m_pActiveUnit->getAP());
		for (int j = 0; j < m_pActiveUnit->getAP(); ++j) {
			Path[j] = m_Path[m_Path.size() - m_pActiveUnit->getAP() + j];
		}
		m_Path = std::move( Path );
	}

	std::reverse(m_Path.begin(), m_Path.end());

	m_pGameMap->getRealPath(m_Path, m_pActiveUnit->getViewRange());

#ifdef ENABLE_GAME_LOG
	_GAMELOG("req_Move " + m_pActiveUnit->getName() + " "
		+ m_pActiveUnit->getCoordinates().to_readable_string()
		+ " " + m_Path.begin()->to_readable_string());
#endif

	int i = 0, len = 2 + (m_Path.size() + 1) * 2;
	char* req = new char[len];
	CellCoordinates beg = getCell(m_pActiveUnit->getPosition());

	req[i++] = MOVE;
	req[i++] = m_Path.size();// < m_pActiveUnit->getAP() ? m_Path.size() : m_pActiveUnit->getAP();
	req[i++] = beg.x;
	req[i++] = beg.y;

	//	int j = 0;
	for (auto it = m_Path.begin();/* j < m_pActiveUnit->getAP() && */it != m_Path.end(); ++it/*, ++j*/) {
		req[i++] = it->x;
		req[i++] = it->y;
	}

	_GAMELOG("Trying to move unit from "
		+ m_pActiveUnit->getCoordinates().to_readable_string()
		+ " to " + m_Path.back().to_readable_string());

	clearPath();

	std::vector<char> Req(req, req+len);
	delete[] req;

	return sendRequest(Req);
}
bool myapp::req_Shoot(AbstractUnit* who_shoot, AbstractUnit* who_get_shooted) {

#ifdef ENABLE_GAME_LOG
	_GAMELOG("req_Shoot " + who_shoot->getName() + " " + who_shoot->getCoordinates().to_readable_string()
		+ " " + who_get_shooted->getName() + " " + who_get_shooted->getCoordinates().to_readable_string());
#endif

	blockActivity();

	auto Distance = GameMap::calculate_distance(who_shoot->getCoordinates(), who_get_shooted->getCoordinates());

	if (Distance > _DATA.getMaxDistance(who_shoot->getWeapon())) {
		_LOG("Can's use weapon: too much distance");
		return false;
	}

	if (Distance < _DATA.getMinDistance(who_shoot->getWeapon())) {
		_LOG("Can's use weapon: too small distance");
		return false;
	}

	if (who_shoot->getAP() < _DATA.getShootCost(who_shoot->getWeapon())) {
		_LOG("Can't use weapon: don't have enough active points (required: "
			+ boost::to_string((short)_DATA.getShootCost(who_shoot->getWeapon())) + 
			", you have: " + boost::to_string((short)who_shoot->getAP()) + ")");
		return false;
	}

	if (!connected()) {
		_LOG("Can't shoot unit: not connected");
		return false;
	}

	//char req[6] = {0};
	//int i = 0;
	//req[i++] = REQUEST_TYPE::REQ_SHOOT;
	//req[i++] = who_shoot->getCoordinates().x;
	//req[i++] = who_shoot->getCoordinates().y;
	//req[i++] = who_get_shooted->getCoordinates().x;
	//req[i++] = who_get_shooted->getCoordinates().y;

	//return sendRequest(req);

	std::vector<char> req(5 * sizeof(char), 0);
	int i = 0;
	req[i++] = REQUEST_TYPE::REQ_SHOOT;
	req[i++] = who_shoot->getCoordinates().x;
	req[i++] = who_shoot->getCoordinates().y;
	req[i++] = who_get_shooted->getCoordinates().x;
	req[i++] = who_get_shooted->getCoordinates().y;

	//return sendRequest(char(REQ_SHOOT) + who_shoot->getCoordinates().to_string()
	//	+ who_get_shooted->getCoordinates().to_string() );
	return sendRequest(std::move( req ));
}
bool myapp::req_CreateUnion(int Team1, int Team2, int Team3, int Team4) 
{
#ifdef _USE_ASSERTS_
	assert_me(false, "trying to create union");
#endif
	_GAMELOG("req_CreateUnion");

	for (auto it = m_pGameMap->m_Unions.begin(); it != m_pGameMap->m_Unions.end(); ++it) {
		for (auto jt = it->begin(); jt != it->end(); ++jt) {
			if (*jt == Team1 || *jt == Team2 || Team3 && *jt == Team3 || Team4 && *jt == Team4) {
				return false;
			}
		}
	}
	int count = 2 + !(Team3 == 0) + !(Team4 == 0);
	int len = 2 + count*sizeof(int);
	char* req = new char[len];
	int i = 0;
	req[i++] = CREATE_UNION;
	req[i++] = count;
	*((int*)(&req[i])) = Team1;
	i += sizeof(int);
	*((int*)(&req[i])) = Team2;
	i += sizeof(int);
	if (Team3) {
		*((int*)(&req[i])) = Team3;
		i += sizeof(int);
	}
	if (Team4) {
		*((int*)(&req[i])) = Team4;
		i += sizeof(int);
	}

	std::vector<char> Req(req, req+len);
	delete[] req;

	return sendRequest(Req);
}
//bool myapp::req_CreateUnit(char type_id, char Class, char Weapon, char Armor, char AP, short HP,
//	CellCoordinates where, int player_id, CellCoordinates looks_to) 
bool myapp::req_CreateUnit(UnitDef ud, CellCoordinates where, CellCoordinates looks_to) 
{

	_GAMELOG("req_CreateUnit");

	if (!connected()) {
		_LOG("Can't create unit: not connected");
		return false;
	}

	if (!m_pGameMap->passable(where)) {
		_LOG("Can't create unit: cell is blocked");
		return false;
	}

	if (ud.Weapon == WEAPON::UNDEFINED_WEAPON) {
		ud.Weapon = _DATA.getDefaultWeapon(ud.type_id);
	}
	if (ud.Weapon == ARMOR::UNDEFINED_ARMOR) {
		ud.Weapon = _DATA.getDefaultArmor(ud.type_id);
	}
	if (!ud.AP) ud.AP = _DATA.getDefaultAP(ud.type_id);
	if (!ud.HP) ud.HP = _DATA.getDefaultHP(ud.type_id);

	const int N = sizeof(char) + sizeof(UnitDef) + 5*sizeof(char);
	std::vector<char> req(N, 0);
	int i = 0;
	req[i++] = REQUEST_TYPE::CREATE_UNIT;
	//req[i++] = type_id;
	//req[i++] = Class;
	//req[i++] = player_id;
	//*((short*)(req+i)) = HP;
	//i += sizeof(HP);
	//req[i++] = AP;
	//req[i++] = Weapon;
	//req[i++] = Armor;
	UD(req[i]) = ud;
	i += sizeof(ud);
	req[i++] = 0; // 0 символов в имени
	req[i++] = where.x;
	req[i++] = where.y;
	req[i++] = looks_to.x;
	req[i++] = looks_to.y;

	return sendRequest(req);
}
//bool myapp::req_CreateHero(const Ogre::String& Name, char type_id, char Class, char Weapon, char Armor,
//	char AP, short HP, int player_id, CellCoordinates where, CellCoordinates looks_to) 
bool myapp::req_CreateHero(const Ogre::String& Name, UnitDef ud,
	CellCoordinates where, CellCoordinates looks_to) 
{

	_GAMELOG("req_CreateHero");

	if (!connected()) {
		_LOG("Can't create unit: not connected");
		return false;
	}

	if (!m_pGameMap->passable(where)) {
		_LOG("Can't create unit: cell is blocked");
		return false;
	}

	if (ud.Weapon == WEAPON::UNDEFINED_WEAPON) {
		ud.Weapon = _DATA.getDefaultWeapon(ud.type_id);
	}
	if (ud.Weapon == ARMOR::UNDEFINED_ARMOR) {
		ud.Weapon = _DATA.getDefaultArmor(ud.type_id);
	}
	if (!ud.AP) ud.AP = _DATA.getDefaultAP(ud.type_id);
	if (!ud.HP) ud.HP = _DATA.getDefaultHP(ud.type_id);

	const int N = sizeof(char) + sizeof(UnitDef) + sizeof(char) + Name.size() + 4*sizeof(char);
	std::vector<char> req(N, 0);
	int i = 0;
	req[i++] = REQUEST_TYPE::CREATE_UNIT;
	//req[i++] = UNIT_TYPE::HERO;
	UD(req[i]) = ud;
	i += sizeof(ud);
	req[i++] = Name.size();
	memcpy_s(&req[i], Name.size(), Name.c_str(), Name.size());
	i += Name.size();
	//req[i++] = type_id;
	//req[i++] = Class;
	//req[i++] = player_id;
	//*((short*)(req+i)) = HP;
	//i += sizeof(HP);
	//req[i++] = AP;
	//req[i++] = Weapon;
	//req[i++] = Armor;
	req[i++] = where.x;
	req[i++] = where.y;
	req[i++] = looks_to.x;
	req[i++] = looks_to.y;

	return sendRequest(req); 
}
bool myapp::req_PlayerName(int player_id) {
#ifdef _USE_ASSERTS_
	assert_me(m_Players.find(player_id) != m_Players.end(), "Trying to get name of unexisted player");
#endif
	_GAMELOG("req_PlayerName " + boost::lexical_cast<Ogre::String>(player_id));
	char req[5];
	*req = REQ_NAME;
	INT(req[1]) = player_id;
	return sendRequest(std::vector<char>(req, req+5));
}
bool myapp::req_DropFromLobby() {

	_GAMELOG("req_DropFromLobby");

	if (!m_pCurPlayer || !m_pCurPlayer->is_in_lobby()) {
		_LOG("Already not in lobby");
		return false;
	}
	return sendRequest(DROP_FROM_LOBBY);
}
bool myapp::req_CreateLobby(char map_id) {
	_GAMELOG("req_CreateLobby");
	if (!connected() || !m_pCurPlayer) {
		_LOG("Connect first");
		return false;
	}
	if (m_pCurPlayer->is_in_lobby()) {
		_LOG("Already in lobby. Exit first");
		return false;
	}
	if (m_pCurPlayer->is_in_game()) {
		_LOG("Already in the game. Exit first");
		return false;
	}
	if (m_pCurPlayer && m_pCurPlayer->get_merk() == nullptr) {
		_LOG("You must choose a merk");
		return false;
	}

	_LOG("Trying to create lobby with map " + _DATA.getMapName(map_id) + " (id " 
		+ boost::lexical_cast<Ogre::String>((int)map_id) + ")");

	char req[2] = {CREATE_LOBBY, map_id};
	return sendRequest(std::vector<char>(req,req+2));
}
bool myapp::req_JoinLobby(int lobby_id) {
	_GAMELOG("req_JoinLobby");
	if (!connected()) {
		_LOG("Connect first");
		return false;
	}
	if (m_pCurPlayer && m_pCurPlayer->is_in_lobby()) {
		_LOG("Already in lobby. Exit first");
		return false;
	}
	if (m_Lobbies.find(lobby_id) == m_Lobbies.end()) {
		_LOG("Lobby doesn't exist");
		return false;
	}

	_LOG("Trying to join lobby " + int2str(lobby_id) + " hosted by "
		+ m_Players[m_Lobbies[lobby_id].get_host_id()].get_nick());

	char req[5];
	*req = JOIN_LOBBY;
	INT(req[1]) = lobby_id;

	return sendRequest(std::vector<char>(req, req + 5));

}
bool myapp::req_StartGame() {

	if (m_pCurPlayer && m_pCurPlayer->is_in_game()) {
		_LOG("Already in game");
		return false;
	}
	if (!m_pCurPlayer || !m_pCurPlayer->is_in_lobby()) {
		_LOG("You are not in lobby");
		return false;
	} else if (m_Lobbies[m_pCurPlayer->get_lobby_id()].get_host_id() != m_pCurPlayer->get_id()) {
		_LOG("You are not a host of the lobby");
		return false;
	}

	return sendRequest(GAME_STARTED);
}
bool myapp::req_PlayersInLobby(int lobby_id) {
	_GAMELOG("req_PlayersInLobby");
	if (!connected()) {
		_LOG("Connect first");
		return false;
	}
	if (m_Lobbies.find(lobby_id) == m_Lobbies.end()) {
		_LOG("Lobby " + int2str(lobby_id) + " doesn't exist");
		return false;
	}
	const int N = sizeof(char) + sizeof(int);
	char req[N];
	*req = PLAYERS_IN_LOBBY;
	INT(req[1]) = lobby_id;
	return sendRequest(std::vector<char>(req, req + N));
}
bool myapp::req_PlayerList() {
	_GAMELOG("req_PlayerList");
	return sendRequest(PLAYER_LIST);
}
bool myapp::req_LobbyList() {
	_GAMELOG("req_LobbyList");
	return sendRequest(LOBBY_LIST);
}
bool myapp::req_ChangeMap(char map_id) {
	_GAMELOG("req_ChangeMap to " + _DATA.getMapName(map_id)
		+ " (id " + int2str(map_id) + ")");
	if (!m_pCurPlayer || !m_pCurPlayer->is_in_lobby()) {
		_LOG("You are not in lobby");
		return false;
	}
	if (m_pCurPlayer && m_pCurPlayer->get_id() != m_Lobbies[m_pCurPlayer->get_lobby_id()].get_host_id()) {
		_LOG("You are not a host of the lobby");
		return false;
	}
	char req[2] = {REQUEST_TYPE::CHANGE_MAP, map_id};
	return sendRequest(std::vector<char>(req, req + 2));
}
bool myapp::req_CreateMerk(char type_id, char weapon_id, Ogre::String name) {

	_GAMELOG("req_CreateMerk");

	if (m_pCurPlayer && m_pCurPlayer->is_in_lobby()) {
		_LOG("You can not create new merk in lobby");
		return false;
	}
	if (m_pCurPlayer && m_pCurPlayer->is_in_game()) {
		_LOG("You can not create new merk in combat");
		return false;
	}

	int N = sizeof(char)*4 + name.size();
	std::vector<char> req(N, 0);
	int i = 0;
	req[i++] = REQUEST_TYPE::CREATE_MERK;
	req[i++] = type_id;
	req[i++] = weapon_id;
	req[i++] = name.size();
	memcpy_s(&req[i], name.size(), name.c_str(), name.size());

	return sendRequest( std::move(req) );
}
bool myapp::req_DeleteMerk(int merk_id) {

	_GAMELOG("req_DeleteMerk");

	if (m_pCurPlayer && m_pCurPlayer->is_in_lobby()) {
		_LOG("You can not delete merk in lobby");
		return false;
	}
	if (m_pCurPlayer && m_pCurPlayer->is_in_game()) {
		_LOG("You can not delete merk in combat");
		return false;
	}
	if (m_pCurPlayer && m_pCurPlayer->get_merk_count() <= 1) {
		_LOG("You can not delete the last merk");
		return false;
	}

	std::vector<char> req(sizeof(char) + sizeof(int), 0);
	req[0] = REQUEST_TYPE::DELETE_MERK;
	INT(req[1]) = merk_id;

	return sendRequest(std::move( req ));
}
bool myapp::req_ChooseMerk(int merk_id) {

	_GAMELOG("req_ChooseMerk");

	if (m_pCurPlayer && m_pCurPlayer->is_in_lobby()) {
		_LOG("You can not choose new merk in lobby");
		return false;
	}
	if (m_pCurPlayer && m_pCurPlayer->is_in_game()) {
		_LOG("You can not choose new merk in combat");
		return false;
	}

	std::vector<char> req(sizeof(char) + sizeof(int), 0);
	req[0] = REQUEST_TYPE::CHOOSE_MERK;
	INT(req[1]) = merk_id;

	return sendRequest(std::move( req ));
}
bool myapp::req_ChangeWeapon(int merk_id, char weapon_id) {

	_GAMELOG("req_ChangeWeapon");

	if (m_pCurPlayer && m_pCurPlayer->is_in_lobby()) {
		_LOG("You can not change weapon in lobby");
		return false;
	}
	if (m_pCurPlayer && m_pCurPlayer->is_in_game()) {
		_LOG("You can not change weapon in combat");
		return false;
	}

	std::vector<char> req(sizeof(char)*2 + sizeof(int), 0);
	int i = 0;
	req[i++] = REQUEST_TYPE::CHANGE_WEAPON;
	INT(req[i]) = merk_id;
	i += sizeof(int);
	req[i++] = weapon_id;
	return sendRequest(std::move( req ));
}
bool myapp::req_MerkList() {
	_GAMELOG("req_MerkList");
	return sendRequest(REQUEST_TYPE::MERK_LIST);
}
bool myapp::req_ExitGame() {
	if (!m_pCurPlayer || !connected()) {
		_LOG("You are not connected");
		return false;
	}
	if (!m_pCurPlayer->is_in_game()) {
		_LOG("You are not in the battle");
		return false;
	}
	if (m_pGameMap->team_count() > 1 && m_pCurPlayer->has_units()) {
		_LOG("You can not leave the battle now");
		return false;
	}
	return sendRequest(EXIT_GAME);
}

void myapp::do_Set(tile unit_coord, char Property, short Value) {
	auto unit = m_pGameMap->getUnit(unit_coord);
#ifdef _USE_ASSERTS_
	assert_me( unit, "Unit pointer can't be zero");
#endif
#ifdef ENABLE_GAME_LOG
	_GAMELOG("do_Set " + unit->getName() + " " + unit->getCoordinates().to_readable_string() + " "
		+ _DATA.getPropertyString(Property) + " " + boost::to_string(Value));
#endif
	switch (Property) {
	case PROPERTY::AP:
#ifdef _USE_ASSERTS_
		assert_me( Value >= 0, "Value to set can't be negative");
#endif
		unit->setActivePoints(Value);
		break;
	case PROPERTY::ARMOR:
		unit->setArmor(Value);
		break;
	case PROPERTY::HP:
		unit->setHitPoints(Value);
		break;
	case PROPERTY::WEAPON:
		unit->setWeapon(Value);
		break;
	default:
		// Если попали сюда, то команда неверна
#ifdef _USE_ASSERTS_
		assert_me( false, "Illigal command from the server");
#endif
	}

	m_bUnitInfoNeedsToBeUpdated = true;
	m_bNextDoing = true;
}
void myapp::do_MoveSelectedUnit() {

	_GAMELOG("do_MoveSelectedUnit");

	for (auto it = m_Path.begin(); it != m_Path.end(); it++) {
		m_pActiveUnit->addWalkPoint(getCellCenter(*it));
	}
}
void myapp::do_MoveUnit(AbstractUnit* unit, std::vector<CellCoordinates> path) {
#ifdef ENABLE_GAME_LOG
	_GAMELOG("do_MoveUnit " + unit->getName() + " " + unit->getCoordinates().to_readable_string()
		+ " " + path.begin()->to_readable_string());
#endif
	clearPath();

	for (auto it = path.begin(); it != path.end(); it++) {
		unit->addWalkPoint(getCellCenter(*it));
	}

	//m_pGameMap->moveUnit(unit->getCoordinates(), *path.begin());
	//unit->setCoordinates(*path.begin());
}
//void myapp::do_CreateUnit(char type_id, char Class, char Weapon, char Armor, char AP, short HP, int player_id,
//	CellCoordinates where, CellCoordinates looks_to) 
//{
//#ifdef ENABLE_GAME_LOG
//	_GAMELOG("do_CreateUnit " + _DATA.getTypeString(type_id) + " " + _DATA.getClassString(Class) + " "
//		+ _DATA.getWeaponString(Weapon) + " " + _DATA.getArmorString(Armor) + " AP "
//		+ boost::to_string(short(AP)) + " HP " + boost::to_string(HP) + " " + where.to_readable_string()
//		+ " PlayerID " + boost::to_string(short(player_id)) + " " + looks_to.to_readable_string());
//#endif
//	//if (!scale) {
//	//	scale = m_fDefaultRobotScale;
//	//}
//	//if (!speed) {
//	//	speed = 50.0f * scale * scale;
//	//}
//	AbstractUnit::ptr r;
//	if (looks_to != CellCoordinates()) {
//		r = AbstractFactory::getInstance().createUnit(getCellCenter(where), PlayerID, type_id, Class,
//			Weapon, Armor, AP, HP,
//			getCellCenter(looks_to) - getCellCenter(where));
//	} else {
//		 r = AbstractFactory::getInstance().createUnit(getCellCenter(where), PlayerID, type_id, Class,
//			 Weapon, Armor, AP, HP);
//	}
//	//if (type_id == UNIT_TYPE::ROBOT) {
//	//	r = RobotFactory::getInstance().createUnit(pos, PlayerID);
//	//} else {
//	//	return;
//	//}
//	r->setCoordinates(where);
//	//Ogre::Vector3(1.3f), 85);
//	// Ogre::Vector3(1.5f), 112);
//
//	if (inUnion(player_id, m_pCurPlayer->get_id())) {
//		drawFOV(where);
//	}
//
//	m_pGameMap->addUnit(where, r.get());
//
//	//static bool firstTeam = true;
//	//if (firstTeam) r->setTeamIndex(1);
//	//else r->setTeamIndex(2);
//	//firstTeam = !firstTeam;
//	//r->setPlayerID(player_id);
//
//	m_bNextDoing = true;
//}
//void myapp::do_CreateUnit(UnitDef ud, tile where, tile looks_to) 
//{
//#ifdef ENABLE_GAME_LOG
//	_GAMELOG("do_CreateUnit " + _DATA.getTypeString(ud.type_id) + " " + _DATA.getClassString(ud.Class) + " "
//		+ _DATA.getWeaponString(ud.Weapon) + " " + _DATA.getArmorString(ud.Armor) + " AP "
//		+ boost::to_string(short(ud.AP)) + " HP " + boost::to_string(ud.HP) + " " + where.to_readable_string()
//		+ " PlayerID " + boost::to_string(ud.PlayerID) + " " + looks_to.to_readable_string());
//#endif
//
//	AbstractUnit::ptr r;
//	if (looks_to != CellCoordinates()) {
//		r = AbstractFactory::getInstance().createUnit(getCellCenter(where), ud,
//			getCellCenter(looks_to) - getCellCenter(where));
//	} else {
//		 r = AbstractFactory::getInstance().createUnit(getCellCenter(where), ud);
//	}
//
//	r->setCoordinates(where);
//
//	if (inUnion(ud.PlayerID, m_pCurPlayer->get_id())) {
//		drawFOV(where);
//	}
//
//	m_Players[ud.PlayerID].new_unit();
//	m_pGameMap->addUnit(where, r.get());
//
//	m_bNextDoing = true;
//}
void myapp::do_CreateUnion(std::list<int> new_union) 
{
#ifdef ENABLE_GAME_LOG
	Ogre::String msg("do_CreateUnion");
	for (auto it = new_union.begin(); it != new_union.end(); ++it) {
		msg += " " + boost::to_string(short(*it));
	}
	_GAMELOG(msg);
#endif
	//std::list<unsigned char> Union;
	//Union.push_back(Team1);
	//Union.push_back(Team2);
	//if (Team3) Union.push_back(Team3);
	//if (Team4) Union.push_back(Team4);
	if (std::find(m_pGameMap->m_Unions.begin(), m_pGameMap->m_Unions.end(), new_union) == m_pGameMap->m_Unions.end())
		m_pGameMap->m_Unions.push_back( std::move( new_union ) );
}
//void myapp::do_CreateHero(const Ogre::String& Name, char type_id, char Class, char Weapon,
//	char Armor, char AP, short HP, int player_id, tile where, tile looks_to) 
//{
//#ifdef ENABLE_GAME_LOG
//	_GAMELOG("do_CreateHero " + Name + " " + _DATA.getTypeString(type_id) + " " + _DATA.getClassString(Class)
//		+ " " + _DATA.getWeaponString(Weapon) + " " + _DATA.getArmorString(Armor) + " AP "
//		+ boost::to_string(short(AP)) + " HP " + boost::to_string(HP) + " "
//		+ where.to_readable_string() + " PlayerID " + boost::to_string(short(player_id)) + " "
//		+ looks_to.to_readable_string());
//#endif
//	AbstractUnit::ptr r;
//	if (looks_to != CellCoordinates()) {
//		r = HeroFactory::getInstance().createUnit(getCellCenter(where), PlayerID, 
//			type_id, Class, Weapon, Armor, AP, HP, Name, getCellCenter(looks_to) - getCellCenter(where));
//	} else {
//		r = HeroFactory::getInstance().createUnit(getCellCenter(where), PlayerID, type_id, Class,
//			Weapon, Armor, AP, HP, Name);
//	}
//	r->setCoordinates(where);
//	if (inUnion(player_id, m_pCurPlayer->get_id())) {
//		drawFOV(where);
//	}
//
//	m_pGameMap->addUnit(where, r.get());
//	m_bNextDoing = true;
//}
void myapp::do_CreateHero(const Ogre::String& Name, UnitDef ud, tile where, tile looks_to, short cur_hp, char cur_ap) 
{
#ifdef ENABLE_GAME_LOG
	_GAMELOG("do_CreateHero " + Name + " " + _DATA.getTypeString(ud.type_id) + " " + _DATA.getClassString(ud.Class)
		+ " " + _DATA.getWeaponString(ud.Weapon) + " " + _DATA.getArmorString(ud.Armor) + " AP "
		+ boost::to_string(short(ud.AP)) + " HP " + boost::to_string(ud.HP) + " "
		+ where.to_readable_string() + " PlayerID " + boost::to_string(ud.PlayerID) + " "
		+ looks_to.to_readable_string());
#endif
	AbstractUnit::ptr r;
	if (looks_to != CellCoordinates()) {
		r = HeroFactory::getInstance().createUnit(getCellCenter(where), ud, Name,
			getCellCenter(looks_to) - getCellCenter(where));
	} else {
		r = HeroFactory::getInstance().createUnit(getCellCenter(where), ud, Name);
	}
	r->setCoordinates(where);
	r->setTeam(m_Players[r->getPlayerID()].get_team());
	r->setHitPoints(cur_hp);
	r->setActivePoints(cur_ap);

	m_Players[ud.PlayerID].new_unit();
	m_pGameMap->addUnit(where, r.get());

	if (m_Players[ud.PlayerID].get_team() == m_pCurPlayer->get_team()) {
		//drawFOV(where);
		redrawFOV();
	}

	if (ud.PlayerID == m_pCurPlayer->get_id()) {
		r->createDecal(
			//Ogre::TextureManager::getSingleton().getByHandle(m_DecalTextureHandle).dynamicCast<Ogre::Texture>(),
			//Ogre::MaterialManager::getSingleton().getByHandle(m_GroundMaterialHandle).dynamicCast<Ogre::Material>());
			m_DecalTexturePtr,
			//m_MapMaterialPtr);
			m_DecalMaterialPtr);
		centerCamera();
	}

	m_bNextDoing = true;
}
void myapp::do_Shoot(AbstractUnit* who, AbstractUnit* in_who, short Damage) {
#ifdef ENABLE_GAME_LOG
	_GAMELOG("do_Shoot " + who->getName() + " " + who->getCoordinates().to_readable_string() +
		" " + in_who->getName() + " " + in_who->getCoordinates().to_readable_string());
#endif
	who->shoot(in_who);
	if (Damage) {
		if (in_who == m_pUnitUnderMouseRay) m_bUnitInfoNeedsToBeUpdated = true;
		if (Damage < in_who->getHP()) {
			in_who->getShooted(Damage);
			_LOG(who->getName() + " shooted in " + in_who->getName() + ": "
				+ boost::to_string(Damage) + " dmg, " + boost::to_string(in_who->getHP()) + " hp remaining");
		} else {
			if (in_who == m_pActiveUnit) {
				deselectActiveUnit();
			}
			m_pGameMap->removeUnit(in_who->getCoordinates());
			m_Players[in_who->getPlayerID()].loose_unit();
			//Hero* h = dynamic_cast<Hero*>(in_who);
			//if (h) {
			//	HeroFactory::removeUnit(in_who);
			//} else {
			//	AbstractFactory::removeUnit(in_who);
			//}
			in_who->getKilled(who);
			redrawFOV();
			_LOG(who->getName() + " have just killed " + in_who->getName() + ": "
				+ boost::to_string(Damage) + " dmg");
			//do_RemoveUnit(in_who->getCoordinates());
		}
	} else {
		_LOG(who->getName() + " shooted in " + in_who->getName() + ": missed, still "
			+ boost::to_string(in_who->getHP()) + " hp");
	}
	m_bNextDoing = true;
}
void myapp::do_SetViewRange(AbstractUnit* unit, char new_range) {
	m_pGameMap->getCurFOV()->decrement(unit->getCoordinates(),
		m_pGameMap->getLightMap(unit->getCoordinates(), unit->getViewRange()));

	unit->setViewRange(new_range);

	m_pGameMap->getCurFOV()->increment(unit->getCoordinates(),
		m_pGameMap->getLightMap(unit->getCoordinates(), unit->getViewRange()));

	redrawFOV();
}
void myapp::do_RemoveAllUnits() {

	_GAMELOG("do_RemoveAllUnits");

	deselectActiveUnit();

	for (auto it = AbstractUnit::g_Units.begin(); it != AbstractUnit::g_Units.end(); ++it) {
		m_pGameMap->removeUnit((*it)->getCoordinates());
		(*it)->removeEntity();
	}

	AbstractFactory::getInstance().removeAllUnits();
	//	RobotFactory::getInstance().removeAllUnits();
	HeroFactory::getInstance().removeAllUnits();

	if (m_pLightMap) m_pLightMap->clear();

}
void myapp::do_RemoveUnit(CellCoordinates where, bool with_entity) {

	_GAMELOG("do_RemoveUnit");

	auto a = m_pGameMap->getUnit(where);

#ifdef _USE_ASSERTS_
	assert_me( a, "Trying to remove unit with 0-pointer to it");
#endif

	auto h = dynamic_cast<Hero*>(a);

	if (m_pGameMap) {
		m_pGameMap->removeUnit(where);
	}

	if (with_entity) a->removeEntity();

	if (h) HeroFactory::getInstance().removeUnit(a);
	else AbstractFactory::getInstance().removeUnit(a);
}
void myapp::do_RemovePlayer(int player_id) {

	_GAMELOG("do_RemovePlayer");

	remove_player(player_id);
}
void myapp::do_ClearPlayers() {

	_GAMELOG("do_ClearPlayers");

	remove_all_players();
}
void myapp::do_TurnEnd() {

	_GAMELOG("do_TurnEnd");

	blockActivity();
	clearPath();
	_LOG("End of your turn");
	m_bNextDoing = true;
}
void myapp::do_TurnBegin() {

	_GAMELOG("do_TurnBegin");

	//for (auto it = AbstractUnit::g_Units.begin(); it != AbstractUnit::g_Units.end(); ++it) {
	//	if ((*it)->getPlayerID() == m_pCurPlayer->get_id()) {
	//		(*it)->setActivePoints(_DATA.getDefaultAP((*it)->getType()));
	//	}
	//}
	unblockActivity();
	_LOG("Your turn");
	m_DoList.push( boost::bind( &myapp::centerCamera, this, true ) );
	m_bNextDoing = true;
}
void myapp::do_DropFromLobby(int player_id) {

	_GAMELOG("do_DropFromLobby");

	if (m_pCurPlayer && m_pCurPlayer->is_in_lobby() == false) {
		return;
	}

	m_Lobbies[m_pCurPlayer->get_lobby_id()].remove_player(player_id);

	if (player_id == m_pCurPlayer->get_id()) {
		m_pCurPlayer->set_is_in_lobby(false);
		_LOG("Dropped from lobby");
	} else {
		_LOG("Player " + m_Players[player_id].get_nick() + " left the lobby");
	}
}
void myapp::do_LobbyNewPlayer(int player_id, char team, bool message) {
	if (m_Players.find(player_id) == m_Players.end()) {
		add_player(player_id);
	}
	_GAMELOG("do_LobbyNewPlayer: "  + m_Players[player_id].get_nick()
		+ " (id " + int2str(player_id) + ")");

	if (m_Lobbies[m_pCurPlayer->get_lobby_id()].add_player(player_id)) {
		m_Players[player_id].set_team(team);
		m_Players[player_id].set_is_in_lobby(true);
		m_Players[player_id].set_lobby_id(m_pCurPlayer->get_lobby_id());
		if (message && player_id != m_pCurPlayer->get_id()) {
			_LOG("Player " + m_Players[player_id].get_nick()
				+ " (id " + int2str(player_id) + ") just entered lobby");
		}
	} else {
		m_Players[player_id].set_team(team);
		_GAMELOG("Lobby: trying to add already existed player " + boost::lexical_cast<Ogre::String>(player_id));
	}
}
void myapp::do_ChangePlayerName(int player_id, Ogre::String new_name) {
	_GAMELOG("do_ChangePlayerName: " + int2str(player_id)
		+ " from " + m_Players[player_id].get_nick() + " to " + new_name);
#ifdef _USE_ASSERTS_
	assert_me(m_Players.find(player_id) != m_Players.end(), "Trying to change name of not existed player");
#endif
	m_Players[player_id].set_nick( std::move(new_name) );
}
void myapp::do_CreateLobby(int host_id, char map_id, int lobby_id) {

	_GAMELOG("do_CreateLobby");

	add_lobby(host_id, map_id, lobby_id);

	if (host_id == m_pCurPlayer->get_id()) {
		m_pCurPlayer->set_is_in_lobby(true);
		m_pCurPlayer->set_lobby_id(lobby_id);
		_LOG("Lobby succesfully created (id " + int2str(lobby_id) + ")");
	} else {
		_LOG("New lobby: " + int2str(lobby_id) + " hosted by " + m_Players[host_id].get_nick());
	}

	req_PlayersInLobby(lobby_id);
	//else {
	//	_LOG("You are in lobby hosted by " + m_Players[host_id].get_nick());
	//}
}
void myapp::do_JoinLobby(int lobby_id) {

	_GAMELOG("do_JoinLobby");

	auto it = m_Lobbies.find(lobby_id);
#ifdef _USE_ASSERTS_
	assert_me( it != m_Lobbies.end(), "Unexisted lobby to join");
#endif
	it->second.add_player(m_pCurPlayer->get_id());
	m_pCurPlayer->set_lobby_id(lobby_id);
	_LOG("Successfully joined lobby " + int2str(lobby_id) + " hosted by "
		+ m_Players[it->second.get_host_id()].get_nick());
}
void myapp::do_DropLobby(int lobby_id) {

	_GAMELOG("do_DropLobby");

	auto it = m_Lobbies.find(lobby_id);
	if (it != m_Lobbies.end()) {
		_LOG("Lobby " + int2str(it->second.get_id()) + " no longer exist");
		if (m_pCurPlayer && m_pCurPlayer->is_in_lobby() && m_pCurPlayer->get_lobby_id() == it->first) {
			m_pCurPlayer->set_is_in_lobby(false);
			_LOG("Dropped from lobby");
		}
		m_Lobbies.erase(it);
	}
}
void myapp::do_ChangeMap(int lobby_id, char map_id) {

	_GAMELOG("do_ChangeMap");

	auto it = m_Lobbies.find(lobby_id);
	if (it != m_Lobbies.end()) {
		if (m_pCurPlayer && m_pCurPlayer->is_in_lobby()
			&& m_Lobbies[m_pCurPlayer->get_lobby_id()].get_id() == it->first) {
				_LOG("Host of your lobby has changed map to "
					+ _DATA.getMapName(map_id) + " (id " + int2str(map_id) + ")");
		} else {
			_LOG("Lobby " + int2str(it->first) + " has changed map to "
				+ _DATA.getMapName(map_id) + " (id " + int2str(map_id) + ")");
		}
		it->second.set_map_id(map_id);
	} else {
		req_LobbyList();
	}
}
void myapp::do_CreateMerk(int merk_id, UnitDef ud, Ogre::String name, bool log) {

	_GAMELOG("do_CreateMerk");

	m_pCurPlayer->add_merk(Merk::create(merk_id, name, ud));

	if (log) _LOG("Merk created");
}
void myapp::do_DeleteMerk(int merk_id) {

	_GAMELOG("do_DeleteMerk");

	m_pCurPlayer->delete_merk(merk_id);

	_LOG("Merk deleted");
}
void myapp::do_ChooseMerk(int merk_id) {

	_GAMELOG("do_ChooseMerk");

	m_pCurPlayer->set_merk_id(merk_id);

	_LOG("Current merk: " + m_pCurPlayer->get_merk()->get_name());
}
void myapp::do_ChangeWeapon(int merk_id, char weapon_id) {

	_GAMELOG("do_ChangeWeapon");

	m_pCurPlayer->change_weapon(merk_id, weapon_id);

	_LOG(m_pCurPlayer->get_merk(merk_id)->get_name() + " has changed weapon to "
		+ _DATA.getWeaponString(weapon_id));
}

void myapp::show_players_in_lobby() {

	_GAMELOG("show_players_in_lobby");

	if (m_pCurPlayer && m_pCurPlayer->is_in_lobby() && m_pConsole) {
		auto players = m_Lobbies[m_pCurPlayer->get_lobby_id()].get_players();
		m_pConsole->print("Players in your lobby:");
		for (auto it = players.begin(); it != players.end(); ++it) {
			m_pConsole->print("*   " + m_Players[*it].get_nick()
				+ ": team " + int2str(m_Players[*it].get_team()));
		}
	}
}
void myapp::show_player_list() {
	_GAMELOG("show_player_list");
	if (m_Players.empty()) {
		_LOG("Smth is wrong, player list is empty (even you are not included). Call develeper");
		return;
	}

	_LOG("Player list:");
	for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
		if (it->second.is_online()) {
			_LOG("*   " + it->second.get_nick() + " (id " + int2str(it->second.get_id()) + ")");
		}
	}
}
void myapp::show_lobby_list() {
	_GAMELOG("show_lobby_list");
	if (m_Lobbies.empty()) {
		_LOG("There are no lobbies on the server");
		return;
	}

	_LOG("Lobby list:");
	for (auto it = m_Lobbies.begin(); it != m_Lobbies.end(); ++it) {
		_LOG("*   Lobby " + int2str(it->first) + ": hosted by "
			+ m_Players[it->second.get_host_id()].get_nick()
			+ " on map " + _DATA.getMapName(it->second.get_map_id()));
	}
}
void myapp::show_merk_list() {
	if (m_pCurPlayer && m_pCurPlayer->is_online() == false) {
		_LOG("You are not logged in");
		return;
	}
	m_pCurPlayer->show_merk_list();
}

//UnitDef getDefaultParameters(char type_id) {
//	UnitDef ud;
//	ud.Class = _DATA.getDefaultClass(type_id);
//	ud.Weapon = _DATA.getDefaultWeapon(type_id);
//	ud.Armor = _DATA.getDefaultArmor(type_id);
//	ud.AP = _DATA.getDefaultAP(type_id);
//	ud.HP = _DATA.getDefaultHP(type_id);
//	ud.ViewRange = _DATA.getDefaultViewRange(type_id);
//	ud.type_id = type_id;
//	return std::move( ud );
//}

void myapp::blockActivity() {
	m_bActivityBlocked = true;
}
void myapp::unblockActivity() {
	m_bActivityBlocked = false;
}
void myapp::centerCamera(bool into_do_list) {
	AbstractUnit* unit = m_pGameMap->getMyFirstUnit();
	if (!unit) return;

	//Ogre::Real length m_pCamera->getCameraToViewportRay(0, 0).intersects(m_Plane).second;
	Ogre::Ray cam_ray = m_pCamera->getCameraToViewportRay(0.5, 0.5);
	Ogre::Ray ray;

	// берем луч с началом в положении первого юнита и направлением, противоположным направлению
	// луча из камеры и ставим камеру в точку, находящуюся на этом луче на расстоянии от
	// начала таком же, на каком находится от камеры точка, на которую она смотрит из позиции (0, 0)

	Ogre::Real cam_y = m_pCamera->getPosition().y;

	Ogre::Real len = cam_ray.intersects(m_Plane).second;
	Ogre::Vector3 point = cam_ray.getPoint(len);


	ray.setOrigin(unit->getPosition());
	ray.setDirection(cam_ray.getDirection()*(-1));

	Ogre::Vector3 pos = ray.getPoint(len);

	m_pCamera->setPosition(pos);

	if (into_do_list) {
		m_bNextDoing = true;
	}

}

inline Ogre::Real posMod(Ogre::Real a, Ogre::Real b) {
#ifdef _USE_ASSERTS_
	assert_me(a > 0 && b > 0, "Negative numbers in posMod");
#endif

	while (a > b) {
		a -= b;
	}

	return a;
}
Ogre::Vector2 myapp::					getMouseNormalizedCoordinates() const {
	//CEGUI::Vector2f mousePos = CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().getPosition();
	//mousePos.d_x /= m_pRenderWindow->getWidth();
	//mousePos.d_y /= m_pRenderWindow->getHeight();

	//return Ogre::Vector2(mousePos.d_x,mousePos.d_y);

	Ogre::Real mx = m_pTrayManager->getCursorContainer()->getLeft();
	Ogre::Real my = m_pTrayManager->getCursorContainer()->getTop();

	mx /= m_pRenderWindow->getWidth();
	my /= m_pRenderWindow->getHeight();

	return Ogre::Vector2(mx,my);
}
Ogre::Ray myapp::						getMouseRay() const {
	//#ifdef _USE_ASSERTS_
	//	assert_me( false );
	//#endif
	//CEGUI::Vector2f mousePos = CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().getPosition();
	//return m_pCamera->getCameraToViewportRay(mousePos.d_x/float(m_pRenderWindow->getWidth()),mousePos.d_y/float(m_pRenderWindow->getHeight()));
	return m_pTrayManager->getCursorRay(m_pCamera);
}
std::pair<bool,Ogre::Vector3> myapp::	getMouseIntersectTerrain() const {
	Ogre::Ray mouseRay = m_pTrayManager->getCursorRay(m_pCamera);
	std::pair<bool, Ogre::Real> res = mouseRay.intersects(m_Plane);
	Ogre::Vector3 intersection = mouseRay.getPoint(res.second);

	return std::pair<bool,Ogre::Vector3>(res.first && !notInPlane(intersection), intersection);
}
std::pair<bool,Ogre::Vector3> myapp::	getPosByMouse() const {
	Ogre::Ray mouseRay = m_pTrayManager->getCursorRay(m_pCamera);
	std::pair<bool, Ogre::Real> res = mouseRay.intersects(m_Plane);
	Ogre::Vector3 intersection = mouseRay.getPoint(res.second);

	return std::pair<bool,Ogre::Vector3>(res.first && !notInPlane(intersection), getNormalPos(intersection));
}
Ogre::Vector3 myapp::					getNormalPos(const Ogre::Vector3& pos) const {
	Ogre::Vector3 newpos(pos);
	newpos.x -= posMod(pos.x - PLANE_MIN_X, FIELD_CELL_LENGTH) - FIELD_CELL_LENGTH / 2;
	newpos.z -= posMod(pos.z - PLANE_MIN_Z, FIELD_CELL_LENGTH) - FIELD_CELL_LENGTH / 2;

	return newpos;
}
CellCoordinates myapp::					getCell(const Ogre::Vector3& pos) const {
	int m = (pos.x - PLANE_MIN_X) / FIELD_CELL_LENGTH;
	int n = (pos.z - PLANE_MIN_Z) / FIELD_CELL_LENGTH;

	if (m < 0) m = 0;
	if (n < 0) n = 0;

	if (m >= FIELD_SIZE) m = FIELD_SIZE - 1;
	if (n >= FIELD_SIZE) n = FIELD_SIZE - 1;

	return CellCoordinates(m,n);
}
CellCoordinates myapp::					getCellByMouse() const {
	std::pair<bool, Ogre::Vector3> res = getMouseIntersectTerrain();
	//	if (res.first) {
	return getCell(res.second);
	//}
	//return CellCoordinates(-1, -1);
}
Ogre::Vector3 myapp::					getCellCenter(CellCoordinates cell, Ogre::Real _y) const {
	return Ogre::Vector3(cell.x * FIELD_CELL_LENGTH + FIELD_CELL_LENGTH / 2, _y, 
		cell.y * FIELD_CELL_LENGTH + FIELD_CELL_LENGTH / 2);
}
Ogre::Vector3 myapp::					getCellMin(CellCoordinates cell, Ogre::Real _y) const {
	return Ogre::Vector3(cell.x * FIELD_CELL_LENGTH, _y, cell.y * FIELD_CELL_LENGTH);
}
Ogre::Vector3 myapp::					getCellMax(CellCoordinates cell, Ogre::Real _y) const {
	return Ogre::Vector3(cell.x * FIELD_CELL_LENGTH + FIELD_CELL_LENGTH, _y, 
		cell.y * FIELD_CELL_LENGTH + FIELD_CELL_LENGTH);
}
AbstractUnit* myapp::					getUnitByRay(Ogre::Ray ray) {
	for (auto i = AbstractUnit::g_Units.begin(); i != AbstractUnit::g_Units.end(); i++) {

		if ((*i)->intersects(ray) && (*i)->isVisible()) {

			if (Ogre::rayIntersectsEntity(ray, (*i)->getEntity())) {

				return *i;

			}
		}
	}

	return 0;
}
AbstractUnit* myapp::					getUnitByMouseRay() {
	Ogre::Ray ray = getMouseRay();

	//for (auto i = AbstractUnit::g_Units.begin(); i != AbstractUnit::g_Units.end(); i++) {
	//	if ((*i)->intersects(ray) && (*i)->isVisible()) {
	//		


	//		if (*i != m_pUnitUnderMouseRay) {
	//			m_bUnitInfoNeedsToBeUpdated = true;
	//			m_pUnitUnderMouseRay = *i;
	//		}
	//		return *i;

	//	}
	//}

	//if (m_pUnitUnderMouseRay) m_bUnitInfoNeedsToBeUpdated = true;
	//m_pUnitUnderMouseRay = 0;
	//return 0;

	AbstractUnit* unit = getUnitByRay(ray);

	//if (unit) {
	//	if (unit != m_pUnitUnderMouseRay) {
	//		m_bUnitInfoNeedsToBeUpdated = true;
	//		m_pUnitUnderMouseRay = unit;
	//	}
	//} else if (m_pUnitUnderMouseRay) {
	//	m_bUnitInfoNeedsToBeUpdated = true;
	//	m_pUnitUnderMouseRay = 0;
	//}

	if (unit != m_pUnitUnderMouseRay) {
		m_bUnitInfoNeedsToBeUpdated = true;
		m_pUnitUnderMouseRay = unit;
	}

	return unit;
}

int myapp::performSelection(SelectionBox* sb) {

	//sb->finishSelection();

	//float left = sb->getStart().x, right = sb->getStop().x,
	//top = sb->getStart().y, bottom = sb->getStop().y;
	//
	//if (left > right)
	//	std::swap(left, right);
	//
	//if (top > bottom)
	//	std::swap(top, bottom);

	//if ((right - left) * (bottom - top) < 0.0001) {
	//       return selectObjectByRay(m_pTrayManager->getCursorRay(m_pCamera)/*getMouseRay()*/) ? PS_OK : PS_SMALLBOX;
	//}

	//Ogre::Ray topLeft = m_pCamera->getCameraToViewportRay(left, top);
	//Ogre::Ray topRight = m_pCamera->getCameraToViewportRay(right, top);
	//Ogre::Ray bottomLeft = m_pCamera->getCameraToViewportRay(left, bottom);
	//Ogre::Ray bottomRight = m_pCamera->getCameraToViewportRay(right, bottom);

	//Ogre::PlaneBoundedVolume vol;

	//vol.planes.push_back(Ogre::Plane(topLeft.getPoint(3), 
	//	topRight.getPoint(3), 
	//	bottomRight.getPoint(3)));         // front plane

	//vol.planes.push_back(Ogre::Plane(topLeft.getOrigin(), 
	//	topLeft.getPoint(100), 
	//	topRight.getPoint(100)));         // top plane

	//vol.planes.push_back(Ogre::Plane(topLeft.getOrigin(), 
	//	bottomLeft.getPoint(100), 
	//	topLeft.getPoint(100)));       // left plane

	//vol.planes.push_back(Ogre::Plane(bottomLeft.getOrigin(), 
	//	bottomRight.getPoint(100), 
	//	bottomLeft.getPoint(100)));   // bottom plane

	//vol.planes.push_back(Ogre::Plane(topRight.getOrigin(), 
	//	topRight.getPoint(100), 
	//	bottomRight.getPoint(100)));     // right plane

	//// число 100 тут ни на что не влияет, просто надо же выбрать какую-то точку на луче, и 
	//// я выбираю ее на расстоянии 100 пойнтов от начала луча

	//Ogre::PlaneBoundedVolumeList volList;
	//volList.push_back(vol);
	//
	//m_pVolQuery->setVolumes(volList);
	//Ogre::SceneQueryResult result = m_pVolQuery->execute();

	////Robot::deselectRobots();
	//m_pActiveSelectedGroup->deselectUnits();
	//Ogre::SceneQueryResultMovableList::iterator iter;

	//int res = PS_NO_SELECTED;

	//for (iter = result.movables.begin(); iter != result.movables.end(); iter++) {
	//	int a = (*iter)->getQueryFlags();
	//	if (a != 0xFFFFFFFF && a & ROBOT_QUERY_FLAG) {
	//		//selectObject(*iter); 
	//		// тут надо выделить объект, а не модельку
	//		Ogre::any_cast<Robot*>((*iter)->getUserObjectBindings().getUserAny())->addToSelectedGroup(m_pActiveSelectedGroup); 
	//		// криво -_-
	//		res = PS_OK;
	//	}
	//}

	//return res;
	return 0;
}
void myapp::selectActiveUnit(AbstractUnit* unit) {
	if (m_bDontSelectUnit) {
		return;
	}
	unit->setActive(true);
	m_pActiveUnit = unit;
	m_pGameMap->setActiveUnit(unit);
}
void myapp::deselectActiveUnit() {
	if (!m_pActiveUnit) {
		return;
	}
	m_pActiveUnit->setActive(false);
	m_pActiveUnit = 0;
	m_pGameMap->deselectActiveUnit();
	clearPath();
}
bool myapp::selectObjectByRay(Ogre::Ray ray) {

	if (m_bDontSelectUnit) {
		return false;
	}

	for (std::list<AbstractUnit*>::iterator i = AbstractUnit::g_Units.begin(); i != AbstractUnit::g_Units.end(); i++) {
		if ((*i)->intersects(ray) && (*i)->getPlayerID() == m_pCurPlayer->get_id() && (*i)->isAlive()) {
			if (Ogre::rayIntersectsEntity(ray, (*i)->getEntity())) {

				if (m_pActiveUnit) {
					deselectActiveUnit();
				}

				//m_pActiveSelectedGroup->deselectUnits();

				////(*i)->addToSelectedGroup(m_pActiveSelectedGroup);

				////(*i)->setActive(true);
				////m_pActiveUnit = *i;

				selectActiveUnit(*i);

				return true;
			}
		}
	}

	return false;
}

Ogre::uint32 myapp::notInPlane(const Ogre::Vector3& vec) const {
	// x 762,998
	// z 986,991
	if (vec.z > PLANE_MAX_Z - MIN_DEVIATION) return NIP_BIG_Z;
	if (vec.z < PLANE_MIN_Z + MIN_DEVIATION) return NIP_SMALL_Z;
	if (vec.x > PLANE_MAX_X - MIN_DEVIATION) return NIP_BIG_X;
	if (vec.x < PLANE_MIN_X + MIN_DEVIATION) return NIP_SMALL_X;

	return NIP_FALSE;
}
Ogre::uint32 myapp::notInPlane(AbstractUnit* u) const {
#ifdef _USE_ASSERTS_
	assert_me( u, "Unit pointer can't be zero" );
#endif
	Ogre::AxisAlignedBox box = u->getAAB();
	if (!box.isFinite())
		return NIP_FALSE;

	Ogre::uint32 res;

	if (m_bCalculateDeviations) {
		res = notInPlane(box.getMinimum()) | notInPlane(box.getMaximum());//NIP_FALSE;
	}
	else {
		return NIP_FALSE;
	}
	//if (box.getMinimum().z < -845) res |= NIP_SMALL_Z;
	//if (box.getMaximum().z > 980) res |= NIP_BIG_Z;
	//if (box.getMinimum().x < fmin(box.getMinimum().z)) res |= NIP_SMALL_X;
	//if (box.getMaximum().x > fmax(box.getMaximum().z)) res |= NIP_BIG_X;
	return res;
}

Ogre::String myapp::int2str(int x) {
	return boost::lexical_cast<Ogre::String>(x);
}

bool myapp::drawUnitPath() {
#ifdef _USE_ASSERTS_
	assert_me( m_pActiveUnit, "Active unit pointer is zero, but is being used in drawUnitPath" );
#endif
	if (m_Path.empty()) {
		return false;
	}

	m_pPathLine->clear();
	auto max_length = m_pActiveUnit->getAP();

	Ogre::Vector3 unitPos = m_pActiveUnit->getPosition();
	m_pPathLine->addPoint(unitPos.x, unitPos.y + 1.5, unitPos.z);

	auto it = m_Path.rbegin();
	for (int i = 0; it != m_Path.rend() && i < max_length; ++it, ++i) {
		Ogre::Vector3 unitPos = getCellCenter(*it);
		m_pPathLine->addPoint(unitPos.x, unitPos.y + 1.5, unitPos.z);
	}

	m_pPathLine->update();
	m_pPathLine->getParentNode()->needUpdate();

	return true;
}
void myapp::clearPath(bool physically) {
	if (physically) m_Path.clear();
	m_pPathLine->clear();
	m_pPathLine->update();
}
void myapp::printLightMap(CellCoordinates cc) {
	//array2<bool>& lm = m_pGameMap->getLightMapPtr()->getLightMap(cc, UNIT_MIN_VISIBILITY_RADIUS);
	array2<bool> lm = m_pGameMap->getLightMap(cc, UNIT_MIN_VISIBILITY_RADIUS);
	Ogre::String line;
	line.resize(lm.getM());
	// вывожу в транспонированном виде
	for (int i = 0; i < lm.getN(); ++i) {
		for (int j = 0; j < lm.getM(); ++j) {
			if ( lm(j, i)) {
				line[j] = '1';
			} else {
				line[j] = '0';
			}
		}
		_LOG(line);
	}
	line = " ";
	_LOG(line);
	//drawFOV(cc);
	//*m_pConsole << *static_cast<array2<char>*>(m_pGameMap->getCurFOV()) << '\n';
}
void myapp::undrawFOV(CellCoordinates cc) {
	//if (!m_pGameMap->inRange(cc))
	//	return;
	//if (m_LightMaps(cc))
	//	m_LightMaps(cc)->setVisible(false);
	m_pLightMap->clear();
}
//void myapp::drawFOV(CellCoordinates cc, char radius) {
//	if (!m_pGameMap->in_map(cc) || !m_pGameMap->passable(cc, false))
//		return;
//
//	//if (m_LightMaps(cc)) {
//	//	m_LightMaps(cc)->setVisible(true);
//	//	return;
//	//}
//
//	//Ogre::MaterialPtr matptr = Ogre::MaterialManager::getSingleton().load("BaseColoured1","General");
//	//matptr->load();
//	//matptr->getBestTechnique()->getPass(0)->setVertexColourTracking(Ogre::TVC_DIFFUSE);
//	//matptr->getBestTechnique()->getPass(0)->setLightingEnabled(false); 
//	//matptr->setDiffuse(1,1,1,1);//(0,1,0,0.8);
//
//	bool need_attach = false;
//
//	if (m_pLightMap) {
//		m_pLightMap->clear();
//		m_pLightMap->estimateVertexCount(4 * 17 * 17);
//		m_pLightMap->estimateIndexCount(8 * 17 * 17);
//	} else {
//		m_pLightMap = new Ogre::ManualObject("FOV");
//		//m_pLightMap->setDynamic(true);
//		m_pLightMap->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
//		m_pLightMap->estimateVertexCount(4 * 17 * 17);
//		m_pLightMap->estimateIndexCount(8 * 17 * 17);
//		m_pLightMap->setCastShadows(false);
//		need_attach = true;
//	}
//
//	//m_LightMaps(cc) = new Ogre::ManualObject("FOV "+cc.to_readable_string());
//	//array2<bool>& lm = m_pGameMap->getLightMapPtr()->getLightMap(cc, UNIT_MIN_VISIBILITY_RADIUS);
//	array2<bool> lm = m_pGameMap->getLightMap(cc, radius);
//
//	//Ogre::String line;
//	//line.resize(lm.getN());
//	//for (int i = 0; i < lm.getM(); ++i) {
//	//	for (int j = 0; j < lm.getN(); ++j) {
//	//		if ( lm(i, j)) {
//	//			line[j] = '1';
//	//		} else {
//	//			line[j] = '0';
//	//		}
//	//	}
//	//	_LOG(line);
//	//}
//
//	Ogre::Real line_y = 0.55f;
//	Ogre::Vector3 cur_cell_min;
//	Ogre::Vector3 cur_cell_max;
//
//	m_pLightMap->begin("BaseColoured1", Ogre::RenderOperation::OT_LINE_LIST);
//	m_pLightMap->colour(COLOUR_FOV);
//	int k = 0;
//	for(int i = 0; i < lm.getM(); ++i) {
//		for (int j = 0; j < lm.getN(); ++j) {
//			if ( lm( i, j) ) {
//				CellCoordinates cur_cell(cc.x + i - UNIT_MIN_VISIBILITY_RADIUS,
//					cc.y + j - UNIT_MIN_VISIBILITY_RADIUS);
//				cur_cell_min = getCellMin(cur_cell, line_y);
//				cur_cell_max = getCellMax(cur_cell, line_y);
//				m_pLightMap->position(cur_cell_min);
//				m_pLightMap->position(cur_cell_min.x, line_y, cur_cell_max.z);
//				m_pLightMap->position(cur_cell_max);
//				m_pLightMap->position(cur_cell_max.x, line_y, cur_cell_min.z);
//
//				m_pLightMap->index(4*k + 0);
//				m_pLightMap->index(4*k + 1);
//				m_pLightMap->index(4*k + 1);
//				m_pLightMap->index(4*k + 2);
//				m_pLightMap->index(4*k + 2);
//				m_pLightMap->index(4*k + 3);
//				m_pLightMap->index(4*k + 3);
//				m_pLightMap->index(4*k + 0);
//				//m_pLightMap->position(cur_cell_min);
//				++k;
//			}
//		}
//	}
//
//	m_pLightMap->end();	
//	//m_LightMaps(cc)->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
//
//	if (need_attach) m_pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(m_pLightMap);
//
//	//matptr->unload();
//}

void myapp::redrawFOV(bool full) {
	
	if (!m_pGameMap || m_LightMapTexturePtr.isNull()) {
		return;
	}

	FieldOfView lm = *m_pGameMap->getCurFOV();

	bool need_attach = false;

	if (m_pLightMap) {
		m_pLightMap->clear();
		m_pLightMap->estimateVertexCount(4 * 19 * 19 * 2); // числа надо сделать зависящими от радиуса обзора
		m_pLightMap->estimateIndexCount(8 * 19 * 19 * 2);
	} else {
		m_pLightMap = new Ogre::ManualObject("FOV");
		m_pLightMap->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
		m_pLightMap->estimateVertexCount(4 * 19 * 19 * 2);
		m_pLightMap->estimateIndexCount(8 * 19 * 19 * 2);
		m_pLightMap->setCastShadows(false);
		need_attach = true;
	}

	//////////////// вывод поля зрения в консоль (сейчас нинада)
	////#ifdef _DEBUG
	//	Ogre::String line;
	//	line.resize(lm.getN());
	//	for (int i = 0; i < lm.getM(); ++i) {
	//		for (int j = 0; j < lm.getN(); ++j) {
	//			if ( lm(i, j) >= 0) {
	//				line[j] = '0' + lm(i ,j);
	//			} else {
	//				line[j] = '8';
	//			}
	//		}
	//		_LOG(line);
	//	}
	////#endif
	////////////////

	Ogre::Real line_y = 0.55f;
	Ogre::Vector3 cur_cell_min;
	Ogre::Vector3 cur_cell_max;

	m_pLightMap->begin("BaseColoured1", Ogre::RenderOperation::OT_LINE_LIST);

	m_pLightMap->colour(COLOUR_FOV);
	int k = 0;
	for(int i = 0; i < lm.getM(); ++i) {
		for (int j = 0; j < lm.getN(); ++j) {
			if ( lm(i, j) > 0 ) {
				CellCoordinates cur_cell(i, j);
				cur_cell_min = getCellMin(cur_cell, line_y);
				cur_cell_max = getCellMax(cur_cell, line_y);

				m_pLightMap->position(cur_cell_min);
				m_pLightMap->position(cur_cell_min.x, line_y, cur_cell_max.z);
				m_pLightMap->position(cur_cell_max);
				m_pLightMap->position(cur_cell_max.x, line_y, cur_cell_min.z);

				m_pLightMap->index(4*k + 0);
				m_pLightMap->index(4*k + 1);
				m_pLightMap->index(4*k + 1);
				m_pLightMap->index(4*k + 2);
				m_pLightMap->index(4*k + 2);
				m_pLightMap->index(4*k + 3);
				m_pLightMap->index(4*k + 3);
				m_pLightMap->index(4*k + 0);
				++k;
			}
		}
	}

//#ifdef _DEBUG
//	if (m_pActiveUnit) {
//		line_y *= 1.1;
//		CellCoordinates cell_unit = m_pActiveUnit->getCoordinates();
//		char range = m_pActiveUnit->getViewRange();
//		auto lm1 = m_pGameMap->getLightMap(cell_unit, range);
//		int k = 0;
//		for(int i = 0; i < lm1.getM(); ++i) {
//			for (int j = 0; j < lm1.getN(); ++j) {
//				//		if (m_pGameMap->in_map(cell_unit.x - range + i, cell_unit.y - range +  j)) {
//				//			if ( lm(cell_unit.x - range + i, cell_unit.y - range +  j) > 0 && lm1(i, j) == false ) {
//				//				//m_pLightMap->begin("BaseColoured1", Ogre::RenderOperation::OT_LINE_LIST);
//				//				m_pLightMap->colour(COLOUR_RED);
//
//				//				CellCoordinates cur_cell(cell_unit.x - range + i, cell_unit.y - range +  j);
//				//				cur_cell_min = getCellMin(cur_cell, line_y);
//				//				cur_cell_max = getCellMax(cur_cell, line_y);
//
//				//				m_pLightMap->position(cur_cell_min.x, line_y, cur_cell_min.z);
//				//				m_pLightMap->position(cur_cell_min.x, line_y, cur_cell_max.z);
//				//				m_pLightMap->position(cur_cell_max.x, line_y, cur_cell_max.z);
//				//				m_pLightMap->position(cur_cell_max.x, line_y, cur_cell_min.z);
//
//				//				m_pLightMap->index(4*k + 0);
//				//				m_pLightMap->index(4*k + 1);
//				//				m_pLightMap->index(4*k + 1);
//				//				m_pLightMap->index(4*k + 2);
//				//				m_pLightMap->index(4*k + 2);
//				//				m_pLightMap->index(4*k + 3);
//				//				m_pLightMap->index(4*k + 3);
//				//				m_pLightMap->index(4*k + 0);
//				//				++k;
//
//				//				//m_pLightMap->end();	
//				//			}
//				//			if ( lm(cell_unit.x - range + i, cell_unit.y - range +  j) <= 0 && lm1(i, j) == true ) {
//				//				//m_pLightMap->begin("BaseColoured1", Ogre::RenderOperation::OT_LINE_LIST);
//				//				m_pLightMap->colour(COLOUR_GREEN);
//
//				//				CellCoordinates cur_cell(cell_unit.x - range + i, cell_unit.y - range +  j);
//				//				cur_cell_min = getCellMin(cur_cell, line_y);
//				//				cur_cell_max = getCellMax(cur_cell, line_y);
//
//				//				m_pLightMap->position(cur_cell_min.x, line_y, cur_cell_min.z);
//				//				m_pLightMap->position(cur_cell_min.x, line_y, cur_cell_max.z);
//				//				m_pLightMap->position(cur_cell_max.x, line_y, cur_cell_max.z);
//				//				m_pLightMap->position(cur_cell_max.x, line_y, cur_cell_min.z);
//
//				//				m_pLightMap->index(4*k + 0);
//				//				m_pLightMap->index(4*k + 1);
//				//				m_pLightMap->index(4*k + 1);
//				//				m_pLightMap->index(4*k + 2);
//				//				m_pLightMap->index(4*k + 2);
//				//				m_pLightMap->index(4*k + 3);
//				//				m_pLightMap->index(4*k + 3);
//				//				m_pLightMap->index(4*k + 0);
//				//				++k;
//
//				//				//m_pLightMap->end();	
//				//			}
//				//		}
//				if ( lm1(i, j) ) {
//
//					m_pLightMap->colour(COLOUR_GREEN);
//
//					CellCoordinates cur_cell(cell_unit.x - range + i, cell_unit.y - range +  j);
//					cur_cell_min = getCellMin(cur_cell, line_y);
//					cur_cell_max = getCellMax(cur_cell, line_y);
//
//					m_pLightMap->position(cur_cell_min);
//					m_pLightMap->position(cur_cell_min.x, line_y, cur_cell_max.z);
//					m_pLightMap->position(cur_cell_max);
//					m_pLightMap->position(cur_cell_max.x, line_y, cur_cell_min.z);
//
//					m_pLightMap->index(4*k + 0);
//					m_pLightMap->index(4*k + 1);
//					m_pLightMap->index(4*k + 1);
//					m_pLightMap->index(4*k + 2);
//					m_pLightMap->index(4*k + 2);
//					m_pLightMap->index(4*k + 3);
//					m_pLightMap->index(4*k + 3);
//					m_pLightMap->index(4*k + 0);
//					++k;
//
//				}
//			}
//		}
//	}
//#endif

	m_pLightMap->end();	

	if (need_attach) m_pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(m_pLightMap);
	
	////////////////////////

	FieldOfView& lm_old = *m_pRenderedFOV;

	//Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_LightMapTexturePtr->getBuffer();
	Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_LightMapTexturePtr->getBuffer();
	// Lock the pixel buffer and get a pixel box
	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NO_OVERWRITE); // for best performance use HBL_DISCARD!

	const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();

	Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);

	const int W = m_pGameMap->getWidth(), H = m_pGameMap->getHeight(); // размеры поля

	const int Width = m_LightMapTexturePtr->getWidth(),
				Height = m_LightMapTexturePtr->getHeight();

	int y_render, x_render, y_skip, x_skip, y_edge, x_edge;
	Ogre::uint8* shadow_line;
	const int bytePerPixel = Ogre::PixelUtil::getNumElemBytes(pixelBox.format);

//	for (int x = 0; x < W; ++x)
//		for (int y = 0; y < H; ++y) {
//			if ((lm(x, y) > 0) && (full || lm_old(x, y) <= 0)) {
//				shadow_line = m_LightLine;
//			} else if ((lm(x, y) <= 0) && (full || lm_old(x, y) > 0)) {
//				shadow_line = m_DarkLine;
//			} else continue;
////			if ((lm(x, y) > 0) && (full || lm_old(x, y) <= 0)) {
//				y_render = H - y; // хз, может быть работает только для квадратных карт
//				x_render = W - x;
//
//				pDest = static_cast<Ogre::uint8*>(pixelBox.data) +
//					( (Width + pixelBox.getRowSkip()) * m_LightMapPixelPerTile * (y_render - 1)
//						+ m_LightMapPixelPerTile * (x_render - 1) )	* bytePerPixel;
//
//				for (int j = 0; j < m_LightMapPixelPerTile; ++j) {
//					//for (int i = 0; i < Width_1; ++i) {
//					//	*pDest++ = COLOUR_LIGHT_VALUE;
//					//	*pDest++ = COLOUR_LIGHT_VALUE;
//					//	*pDest++ = COLOUR_LIGHT_VALUE;
//					//	++pDest;
//					//}
//					//pDest += (Width - Width_1)*4;
//					memcpy(pDest, shadow_line,
//						m_LightMapPixelPerTile * bytePerPixel);
//					pDest += (Width + pixelBox.getRowSkip()) * bytePerPixel;
//				}
//			//} else if ((lm(x, y) <= 0) && (full || lm_old(x, y) > 0)) {
//			//	y_render = H - y; // хз, может быть работает только для квадратных карт
//			//	x_render = W - x;
//			//	pDest = static_cast<Ogre::uint8*>(pixelBox.data) +
//			//		( (Width + pixelBox.getRowSkip()) * Height_1 * (y_render - 1) + Width_1 * (x_render - 1) )
//			//			* Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
//			//	//pDest = static_cast<Ogre::uint8*>(pixelBox.data) +
//			//	//	(Width * Height_1 * (y_render - 1) + Width_1 * (x_render - 1))*4;
//			//	for (int j = 0; j < Height_1; ++j) {
//			//		//for (int i = 0; i < Width_1; ++i) {
//			//		//	*pDest++ = COLOUR_SHADOW_VALUE;
//			//		//	*pDest++ = COLOUR_SHADOW_VALUE;
//			//		//	*pDest++ = COLOUR_SHADOW_VALUE;
//			//		//	++pDest;
//			//		//}
//			//		//pDest += (Width - Width_1)*4;
//			//		memcpy(pDest, m_DarkLine, Width_1 * Ogre::PixelUtil::getNumElemBytes(pixelBox.format));
//			//		pDest += (Width + pixelBox.getRowSkip()) * Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
//			//	}
//			//}
//		}

	const int pixelPerTile = m_LightMapPixelPerTile + 2;
	std::list<CellCoordinates> light_tiles;
//	light_tiles.push_back(CellCoordinates(13, 13));

	for (int x = 0; x < W; ++x) {
		for (int y = 0; y < H; ++y) {
			if ((lm(x, y) > 0) && (full || lm_old(x, y) <= 0)) {
				light_tiles.push_back(CellCoordinates(x, y));
			} else if ((lm(x, y) <= 0) && (full || lm_old(x, y) > 0)) {

				for (int i = -1; i <= 1; ++i)
					for (int j = -1; j <= 1; ++j)
						if (lm.inRange(x + i, y + j) && lm(x + i, y + j) > 0)
							light_tiles.push_back(CellCoordinates(x + i, y + j));

				y_render = H - y;
				x_render = W - x;

				if (y_render == 1) {
					y_skip = 0;
					y_edge = 1;
				} else if (y_render < H) {
					y_skip = 1;
					y_edge = 0;
				} else {
					y_skip = 1;
					y_edge = 1;
				}

				if (x_render == 1) {
					x_skip = 0;
					x_edge = 1;
				} else if (x_render < W) {
					x_skip = 1;
					x_edge = 0;
				} else {
					x_skip = 1;
					x_edge = 1;
				}

				pDest = static_cast<Ogre::uint8*>(pixelBox.data) +
					( (Width + pixelBox.getRowSkip()) * (m_LightMapPixelPerTile * (y_render - 1) - y_skip)
						+ m_LightMapPixelPerTile * (x_render - 1) - x_skip )	* bytePerPixel;

				for (int j = 0; j < pixelPerTile - y_edge; ++j) {
					memcpy(pDest, m_DarkLine, (pixelPerTile - x_edge) * bytePerPixel);
					pDest += (Width + pixelBox.getRowSkip()) * bytePerPixel;
				}

			} else continue;
		}
	}

	for (auto it = light_tiles.begin(), e = light_tiles.end(); it != e; ++it) {
		y_render = H - it->y;
		x_render = W - it->x;

		if (y_render == 1) {
			y_skip = 0;
			y_edge = 1;
		} else if (y_render < H) {
			y_skip = 1;
			y_edge = 0;
		} else {
			y_skip = 1;
			y_edge = 1;
		}

		if (x_render == 1) {
			x_skip = 0;
			x_edge = 1;
		} else if (x_render < W) {
			x_skip = 1;
			x_edge = 0;
		} else {
			x_skip = 1;
			x_edge = 1;
		}

		pDest = static_cast<Ogre::uint8*>(pixelBox.data) +
			( (Width + pixelBox.getRowSkip()) * (m_LightMapPixelPerTile * (y_render - 1) - y_skip)
				+ m_LightMapPixelPerTile * (x_render - 1) - x_skip)	* bytePerPixel;

		for (int j = 0; j < pixelPerTile - y_edge; ++j) {
			memcpy(pDest, m_LightLine,
				(pixelPerTile - x_edge) * bytePerPixel);
			pDest += (Width + pixelBox.getRowSkip()) * bytePerPixel;
		}
	}

	pixelBuffer->unlock();

	//if (full) full = false;

	lm_old = std::move( lm );
}

bool myapp::inCombat() {
	return connected() && m_pGameMap != nullptr;
}
Ogre::String myapp::get_nick(int player_id) {
	if (player_id == -1) {
		return "Neutral";
	}
	return m_Players[player_id].get_nick();// != "" ? m_Players[player_id] : boost::to_string(player_id);
}
void myapp::add_player(int player_id, const Ogre::String& Nick, bool log) {
	//boost::mutex::scoped_lock lk(m_Mutex);
	auto it = m_Players.find(player_id);

	if (it == m_Players.end()) {
		m_Players[player_id] = Player(player_id, Nick);
	}

	m_Players[player_id].set_online(true);

	if (Nick == Ogre::StringUtil::BLANK) {
		if (log) _LOG("Add player with id " + int2str(player_id));
		req_PlayerName(player_id);
	} else {
		if (log) _LOG("Add player " + Nick + " (id " + int2str(player_id) + ")");
	}
}
void myapp::add_player(int player_id, Ogre::String&& Nick, bool log) {
	//boost::mutex::scoped_lock lk(m_Mutex);
	auto it = m_Players.find(player_id);

	if (it == m_Players.end()) {
		m_Players[player_id] = Player(player_id, Nick);
	}

	m_Players[player_id].set_online(true);

	if (log) _LOG("Player " + Nick + " (id " + int2str(player_id) + ") is online now");
}
void myapp::remove_player(int player_id) {
	_GAMELOG("remove_player");
	for (auto it = m_Players.begin(); it != m_Players.end(); ) {
		if (it->first == player_id) {
			if (it->first == m_pCurPlayer->get_id()) {
				_LOG("You have been disconnected by the server");
				disconnect();
			} else {
				it->second.set_online(false);
				_LOG("Player " + it->second.get_nick() + " left the server");
			}
			//it = m_Players.erase(it);
			break;
		} else {
			++it;
		}
	}
}
void myapp::remove_all_players() {
	_GAMELOG("remove_all_players");
	if (m_pCurPlayer) {
		Player me = *m_pCurPlayer;
		m_Players.clear();
		m_Players[me.get_id()] = me;
		m_pCurPlayer = &m_Players[me.get_id()];
		if (m_pConnection) m_pConnection->set_player(m_pCurPlayer);
	} else {
		m_Players.clear();
	}
}
//void myapp::remove_user(int player_id) {
//	if (m_Players.find(player_id) != m_Players.end()) {
//		m_Players[player_id] = "";
//	}
//}
void myapp::add_lobby(int host_id, char map_id, int lobby_id) {
	m_Lobbies[lobby_id] = Lobby(host_id, map_id, lobby_id); 
}

//Player* myapp::get_player(int player_id) {
//	auto it = m_Players.find(player_id);
//	if (it == m_Players.end()) {
//		return nullptr;
//	}
//	return &it->second;
//}