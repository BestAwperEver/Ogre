#ifndef _myapp_h_
#define _myapp_h_

#include <time.h>
#include <iomanip>
#include <queue>
#include "OgreConsoleForGorilla.h"
#include "tcp_connection.h"
#include "Player.h"
#include "Lobby.h"
#include "array2.h"

class SelectionBox;
class PlayerCamera;
class AbstractUnit;
struct UnitDef;
class Hero;
class MyOverlayManager;
class GameMap;
class MapBinParser;
class MapBinManager;
class MapObject;
class AbstractFactory;
class HeroFactory;
class DynamicLines;
class CellCoordinates;


// perform selection
#define PS_OK			10
#define PS_SMALLBOX		11
#define PS_NO_SELECTED	12
// not in plane
#define NIP_FALSE		0
#define NIP_SMALL_X		1
#define NIP_SMALL_Z		2
#define NIP_BIG_X		4
#define NIP_BIG_Z		8
// start connection
#define SC_OK					0
#define SC_INVALID_IP			1
#define SC_ALREADY_CONNECTED	2

#define PLANE_CENTER_X	1000.0f
#define PLANE_CENTER_Z	1000.0f
#define PLANE_RADIUS	1000.0f	// ������ ��������� ����������
#define SG_HEIGHT		500.0f	// ������ ������ ���������

#define LINE_WIDTH		0.7		// �������� ������ ����� �����

#define FIELD_SIZE		32		// ������ ���� � ������� (�.�. �� ���������� �� ����������� � ���������)
#define FIELD_CELL_LENGTH (2 * PLANE_RADIUS / FIELD_SIZE)	// ������ ������

#define COLOUR_BLACK Ogre::ColourValue(0,0,0)
#define COLOUR_WHITE Ogre::ColourValue::White
#define COLOUR_GREY	Ogre::ColourValue(0.5, 0.5, 0.5)
#define COLOUR_RED Ogre::ColourValue(1.0, 0.0, 0.0)
#define COLOUR_GREEN Ogre::ColourValue(0.0, 1.0, 0.0)
#define COLOUR_BLUE Ogre::ColourValue(0.0, 0.0, 1.0)
#define COLOUR_DARK_GREEN Ogre::ColourValue(2.0f/255, 100.0f/255, 32.0f/255)
#define COLOUR_LIGHT_GREEN Ogre::ColourValue(2.0f/255, 225.0f/255, 64.0f/255)
#define COLOUR_LIGHT_BLUE Ogre::ColourValue(85.0f/255, 104.0f/255, 200.0f/255)

#define COLOUR_GRID COLOUR_LIGHT_BLUE	// ���� �����
#define COLOUR_FOV COLOUR_BLACK // ���� ��������� � �������� ���� ���������
#define COLOUR_PATH COLOUR_WHITE // ���� ����

#define COLOUR_LIGHT_VALUE 255 // ��� ����� � ���� �� ��������
#define COLOUR_SHADOW_VALUE 135

//#define COLOUR_GRID COLOUR_BLACK	// ���� �����
//#define COLOUR_FOV COLOUR_LIGHT_GREEN // ���� ��������� � �������� ���� ���������
//#define COLOUR_PATH COLOUR_WHITE // ���� ����

#define PLANE_MIN_X (PLANE_CENTER_X - PLANE_RADIUS)
#define PLANE_MAX_X (PLANE_CENTER_X + PLANE_RADIUS)
#define PLANE_MIN_Z (PLANE_CENTER_Z - PLANE_RADIUS)
#define PLANE_MAX_Z (PLANE_CENTER_Z + PLANE_RADIUS)

#define MIN_DEVIATION	FIELD_CELL_LENGTH		// ����������� ���������� ������� �� ���� �����

#define PANELS_COUNT	4 // ���������� ��������� ������� � ����������� � ����� ������� ����

//#define ENABLE_GAME_LOG - ����� �� �����, ������ ��������� �����

#define UNIT_MIN_VISIBILITY_RADIUS 8
#define UNIT_MAX_VISIBILITY_RADIUS 10

#define SELECT_DECAL_STRING "selection_decal.png"

typedef std::pair<CellCoordinates, MapObject> pairCoordObj;

enum CursorType {
	Arrow,
	Crosshair
};

class myapp: 
	public Ogre::WindowEventListener, 
	public Ogre::FrameListener,
	public OIS::KeyListener, 
	public OIS::MouseListener, 
	public OgreBites::SdkTrayListener,
	public Ogre::RenderSystem::Listener
{
	typedef talk_to_svr Connection;
	typedef CellCoordinates tile;
private: // �������
	Ogre::Root*					m_pRoot;
	Ogre::String				m_sPluginsCfg;
	Ogre::String				m_sResourcesCfg;
	Ogre::RenderWindow*			m_pRenderWindow;
	Ogre::SceneManager*			m_pSceneManager;
	Ogre::Viewport*				m_pViewport;
	Ogre::Camera*				m_pCamera;

	//MapObject*					m_pObjectUnderMouseRay;
	
	//Fix for 1.9
	Ogre::OverlaySystem*		m_pOverlaySystem;

	MapBinManager*				m_pMapBinManager;

	PlayerCamera*				m_pCameraMan;

	MyOverlayManager*			m_pMyOverlayManager; //������ �� ����� �����, �� ����������
	// � ������ ������ �� ������ �������� ���������� � ��������� ������� � �������

	GameMap*					m_pGameMap;

	//Fix for 1.9:
	OgreBites::InputContext		m_InputContext;
	//bool						m_bCursorWasVisible;	// was cursor visible before dialog appeared

	OgreBites::SdkTrayManager*	m_pTrayManager;
	OgreBites::ParamsPanel**	m_pParamsPanels;
	int							m_nActivePanelIndex;

	Ogre::ManualObject*			m_pGrid;
	Ogre::ManualObject*			m_pLightMap;
	Ogre::Plane					m_Plane;

	Gorilla::Silverback*		m_pGorilla;
	Gorilla::Screen*			m_pScreen;
	OgreConsole<myapp>*			m_pConsole;
	//OgreConsoleM*				m_pMConsole;
	//OgreBites::TextBox*			m_pTextBox;	// ������

	OIS::InputManager*			m_pInputManager;
	OIS::Mouse*					m_pMouse;
	OIS::Keyboard*				m_pKeyboard;

	//SelectionBox*				m_pSelectionBox;

	//std::list<SelectedGroup>	m_SelectedGroups;
	//SelectedGroup*				m_pActiveSelectedGroup;
	AbstractUnit*				m_pActiveUnit;

	bool m_bShutDown;
	bool m_bCalculateDeviations; // ������������� ������ ������ �� ����
	// ������ �� �����, ��� ���� ���� ������� ��� ����, �� ������������ ������ �� ����������

	CursorType	m_cursorType;
	bool m_bLMouseDown;
	bool m_bRMouseDown;

	int	m_LightMapPixelPerTile; // ��� ������, ��� ����� �������� ��������

	//Ogre::Rectangle2D* rect; // ����� �� �����
//	CEGUI::Renderer *m_pRenderer;		// CEGUI renderer �� �����
private: // ������-��������� �����

	bool hewston_we_have_a_problem(void);

	boost::asio::io_service m_Service;
	unsigned short m_DefaultPort;
	std::vector<char> m_Msg;

	// false � ������ ��������� ���������
	bool parseMsg();

	std::queue<boost::function< void() >> m_DoList;

	bool sendRequest(char command);
	bool sendRequest(const std::vector<char>& msg);

	void forceRunService();
	void handle_thread();

	Connection::ptr m_pConnection;
	Player* m_pCurPlayer;

	typedef unsigned int sc_res;
	sc_res	start_connection(const std::string& IPv4, unsigned short port, const Ogre::String& username);
	void	disconnect();
	bool	connected();

	//bool m_bLogFileCreated;

private: // ������������� � ����
	bool setup();
	void setupResources();
	bool configure();
	void createRenderSystemListener();
	void chooseSceneManager();
	void initFactories();
	void createCamera();
	void createViewports();
	void createResourceListener();
	void loadResources();
	void createScene();
	void createLights();
	void createFog();
	void createFrameListener();
	void createOverlay();
	void setRenderedLightMapDark();
	Ogre::TexturePtr createLightMapTexture();
	Ogre::TexturePtr createGroundTexture();
	Ogre::MaterialPtr createMapMaterial();
	void createPlane();
	void createGrid();
	bool createGameLogFile();
	//void initCEGUI(void);	// �� ��������� �����
	void initGorilla();
	void initConsole();
	//void initMConsole();
	inline void getInput();
	bool frameRenderingQueued(const Ogre::FrameEvent&);

	// Ogre::WindowEventListener
	void windowResized(Ogre::RenderWindow*);
	//Unattach OIS before window shutdown (very important under Linux)
	void windowClosed(Ogre::RenderWindow*);

	// OIS::KeyListener
	bool keyPressed( const OIS::KeyEvent& );
	bool keyReleased( const OIS::KeyEvent& );

	// OIS::MouseListener
	bool mouseMoved( const OIS::MouseEvent& );
	bool mousePressed( const OIS::MouseEvent&, OIS::MouseButtonID );
	bool mouseReleased( const OIS::MouseEvent&, OIS::MouseButtonID );

	void set_entity_under_mouse_ray_clear();

	inline Ogre::String int2str(int);

private: // ������

	void eventOccurred(const Ogre::String& eventName, const Ogre::NameValuePairList* parameters = 0);

//#ifdef ENABLE_GAME_LOG
//	std::ofstream log_file;
//#endif

	//void Log(const Ogre::String&);
	//void AddGameEvent(const Ogre::String&);
	//void GameLog(const Ogre::String&);

	void overlayOnKeyDown(const OIS::KeyEvent& arg);
	void updateParamsPanel();

	// � ����-������
	Ogre::Real m_fScale;
	Ogre::Real m_fMinScale;
	Ogre::Real m_fMaxScale;

	Ogre::Matrix4 BuildScaledOrthoMatrix(float _left, float _right, float _bottom, float _top, float _near, float _far);
	void setCameraScale(Ogre::Real _scale = 0.0f);

	// ����� �� ��������� ��������� ������� �� �������
	bool m_bNextDoing; 

	bool inCombat();

private: // ��������

	// true, ���� ���� ������ ���� �� ��������� ����� �� ��������� ����
	// ����� ���� ����, ��������, �� ����� ������������
	bool m_bFindPath;
	//bool m_bForceFindPath;
	bool m_bDontSelectUnit;
	bool m_bDontMoveSelectedUnit;
	bool m_bActivityBlocked;

	//std::list<std::list<unsigned char>> m_Unions;

	// �������� ���� �������� ����� � ������� �����
	std::vector<CellCoordinates>	m_Path;
	DynamicLines*					m_pPathLine;
	Ogre::Real						m_fDefaultRobotScale;

	// ������� �������, � ������� ��������� ��������� ���� (����� �� ��������� ������)
	CellCoordinates m_mouseCellCoordinates;
	AbstractUnit* m_pUnitUnderMouseRay;
	int m_CurrentActivePlayer;
	bool m_bUnitInfoNeedsToBeUpdated;

	// �������

	void parse_Login(const std::vector<char>& msg, int& pos);

	//������ ����
	void parse_TurnBegin(const std::vector<char>& msg, int& pos);
	void do_TurnBegin();
	//����� ����
	void parse_TurnEnd(const std::vector<char>& msg, int& pos);
	bool req_TurnEnd();
	void do_TurnEnd();

	// ������� ���� ������ � �����
	void do_RemoveAllUnits();
	// ������� �����
	void do_RemoveUnit(CellCoordinates where, bool with_entity = false);
	// ������� ���� ������� �� ������
	void do_ClearPlayers();
	// ������� ������ �� ������
	void do_RemovePlayer(int player_id);

	// ������������� �������� ����� � ��������
	void parse_Set(const std::vector<char>& msg, int& pos);
	void do_Set(CellCoordinates unit_coord, char Property, short Value);

	// ������ ��� ���������� ������������ �� ��������� � �����������
	void blockActivity();
	void unblockActivity();

	// ���������� ������ �� ����� �����
	void centerCamera(bool into_do_list = false);

	bool inUnion(int First_Team, int Second_Team);
	bool inUnion(AbstractUnit* unit1, AbstractUnit* unit2);

	// ������� ���� ������, ������ �� ������������
	bool req_CreateUnion(int Team1,
		int Team2,
		int Team3 = 0,
		int Team4 = 0);
	void parse_CreateUnion(const std::vector<char>& msg, int& pos);
	void do_CreateUnion(std::list<int> new_union);

	bool req_StartGame();

	// ������� ����� � ��������� �����
	//bool req_CreateHero(const Ogre::String& Name, char type_id, char Class, char Weapon, char Armor, char AP, short HP,
	//	int player_id, CellCoordinates where, CellCoordinates looks_to);
	bool req_CreateHero(const Ogre::String& Name, UnitDef ud, CellCoordinates where, CellCoordinates looks_to);
	//void do_CreateHero(const Ogre::String& Name, char type_id, char Class, char Weapon, char Armor, char AP, short HP,
	//	int player_id, CellCoordinates where, CellCoordinates looks_to);
	void do_CreateHero(const Ogre::String& Name, UnitDef,
		CellCoordinates where, CellCoordinates looks_to,
		short cur_hp, char cur_ap);

	// ������� ����� � ��������� �����
	void parse_CreateUnit(const std::vector<char>& msg, int& pos);
	//bool req_CreateUnit(char type_id, char Class, char Weapon, char Armor, char AP, short HP,
	//	CellCoordinates where, int player_id, CellCoordinates looks_to);
	bool req_CreateUnit(UnitDef ud, CellCoordinates where, CellCoordinates looks_to);
	//void do_CreateUnit(char type_id, char Class, char Weapon, char Armor, char AP, short HP, int player_id,
	//	CellCoordinates where, CellCoordinates looks_to);
	//void do_CreateUnit(UnitDef ud, CellCoordinates where, CellCoordinates looks_to);

	//UnitDef getDefaultParameters(char type_id);

	//�������� ������ ������ �����
	void do_SetViewRange(AbstractUnit* unit, char new_range);

	// ���������� �������� �������� � ���������; �� ���-�� ���
	void parse_Shoot(const std::vector<char>& msg, int& pos);
	bool req_Shoot(AbstractUnit* who_shoot, AbstractUnit* who_get_shooted);
	void do_Shoot(AbstractUnit* who_shoot, AbstractUnit* who_get_shooted, short Damage);
	// ���������� ���������� ����� �� ���� �� m_Path
	bool req_MoveSelectedUnit();
	void do_MoveSelectedUnit();
	// ���������� ����� �� ���� path
	//bool req_MoveUnit(AbstractUnit*, std::vector<CellCoordinates> path);
	void parse_MoveUnit(const std::vector<char>& msg, int& pos);
	void do_MoveUnit(AbstractUnit*, std::vector<CellCoordinates> path);
	// ��������� ������ � ���� (� ������ ���� � ��� ���������������)
	void parse_PlayerInGame(const std::vector<char>& msg, int& pos);


	// ������������ ������������ � ����� ����, ������ �� ��������� 
	void calculateDeviations(AbstractUnit* unit);

	void selectActiveUnit(AbstractUnit* unit);
	void deselectActiveUnit();

	// ��������� ������ � ��������, �� ������� ��������� ��������� ����;
	// ��� ������������� ������������� ���� ����������� ����� �� �����
	// ����� ����, ������ �� ���������� ������������ �������� ��� ������� ����
	void updateMousePosition();

	// ������ m_Path �� �����, ���� �� ����
	bool drawUnitPath();
	// ������� ���� � ��������� ���
	void clearPath(bool physically = true);

	// ���������� ��������� �� ������
	Ogre::Vector2 getMouseNormalizedCoordinates() const;

	int performSelection(SelectionBox*);
	//Ogre::PlaneBoundedVolumeListSceneQuery* m_pVolQuery;

	bool selectObjectByRay(Ogre::Ray);
	AbstractUnit* getUnitByRay(Ogre::Ray ray);
	AbstractUnit* getUnitByMouseRay();

	Ogre::Ray getMouseRay() const;
	Ogre::uint32 notInPlane(const Ogre::Vector3&) const;
	Ogre::uint32 notInPlane(AbstractUnit*) const;

	//	���������� ����� ����������� ���� �� ���� � �����
	std::pair<bool,Ogre::Vector3> getMouseIntersectTerrain() const;

	//	���������� ���������� ������ ��������, � ������� ��������� ����� pos
	Ogre::Vector3 getNormalPos(const Ogre::Vector3&) const; 

	//	���������� ���������� ������ ��������
	Ogre::Vector3 getCellCenter(CellCoordinates, Ogre::Real _y = 0) const;
	//	���������� ���������� �������� � 0,0 ����
	Ogre::Vector3 getCellMin(CellCoordinates, Ogre::Real _y = 0) const;
	//	���������� ���������� ������ �������� �� 0,0 ����
	Ogre::Vector3 getCellMax(CellCoordinates, Ogre::Real _y = 0) const;

	//	���������� ���������� ������ ��������, � ������� ������ ����
	std::pair<bool,Ogre::Vector3> getPosByMouse() const;

	//	���������� ���������� ���������� �� ����
	CellCoordinates getCell(const Ogre::Vector3&) const;

	//	���������� ���������� �������� ��� ���������� ����
	//	���������� (-1,-1) � ������ ������
	CellCoordinates getCellByMouse() const;

	void setCursorCrosshair();
	void setCursorArrow();

private: // �����

	std::map<int, std::string> m_Maps;
	std::list<pairCoordObj> m_Objects;
	//array2<MapObject*> m_ObjectsMap;
	Ogre::StaticGeometry* m_pStaticGeometry;
	Ogre::Entity* m_pEntGround;
	Ogre::Entity* m_pTransparentGroundEntity;
	//Ogre::Texture* m_pLightMapTexture;
	Ogre::TexturePtr m_LightMapTexturePtr;
	Ogre::String m_sCurMap;
	char m_CurMapID;
	FieldOfView* m_pRenderedFOV; // ��, ��� ������ ���������� �� �����
	Ogre::uint8* m_DarkLine; // ��� ���������� ������ �����
	Ogre::uint8* m_LightLine;
	//Ogre::ResourceHandle m_DecalTextureHandle;
	//Ogre::ResourceHandle m_GroundMaterialHandle;
	Ogre::TexturePtr m_DecalTexturePtr;
	Ogre::TexturePtr m_GroundTexturePtr;
	Ogre::MaterialPtr m_MapMaterialPtr;
	Ogre::MaterialPtr m_DecalMaterialPtr;

	void loadStandartObjects();
	void parse_LoadMap(const std::vector<char>& msg, int& pos);
	void loadMap(char map_id);
	void loadMap(Ogre::String MapName);
	void unloadMap();

	void constructDynamicMapObject(CellCoordinates, MapObject&);
	void constructStaticMapObject(CellCoordinates, MapObject&);

	void printLightMap(CellCoordinates);
	//void drawFOV(CellCoordinates, char radius);
	void undrawFOV(CellCoordinates);
	//void drawOneTile(int x, int y, bool light);
	void redrawFOV(bool full = false);
	//inline void drawQuad(CellCoordinates);
	//inline void drawQuad(char x, char y);
	
private: // ���������� �������

	void consoleCreateMerk(Ogre::StringVector&);
	void consoleDeleteMerk(Ogre::StringVector&);
	void consoleMerkList(Ogre::StringVector&);
	void consoleRefreshMerkList(Ogre::StringVector&);
	void consoleChangeWeapon(Ogre::StringVector&);
	void consoleChooseMerk(Ogre::StringVector&);
	void consoleChangeViewRange(Ogre::StringVector&);

	void consoleExitGame(Ogre::StringVector&);
	void consoleExitLobby(Ogre::StringVector&);
	void consoleJoinLobby(Ogre::StringVector&);
	void consoleCreateLobby(Ogre::StringVector&);
	void consoleChangeMap(Ogre::StringVector&);
	void consolePlayersInLobby(Ogre::StringVector&);
	void consoleGameStart(Ogre::StringVector&);
	void consoleShowPlayersInLobby(Ogre::StringVector&);

	void consolePlayerList(Ogre::StringVector&);
	void consoleLobbyList(Ogre::StringVector&);
	void consoleShowPlayerList(Ogre::StringVector&);
	void consoleShowLobbyList(Ogre::StringVector&);

	void consoleConnect(Ogre::StringVector&);
	void consoleDisconnect(Ogre::StringVector&);
	void enableUnimportantMsgs(Ogre::StringVector&);
	void enableEmulatingPing(Ogre::StringVector&);

	void consoleTurnEnd(Ogre::StringVector&);
	void consoleCreateUnit(Ogre::StringVector&);
	void consoleCreateHero(Ogre::StringVector&);
	void consoleRunService(Ogre::StringVector&);

	void consoleHelp(Ogre::StringVector&);
	void consoleWhy(Ogre::StringVector&);

	void sayHello(Ogre::StringVector&);
	void shutDown(Ogre::StringVector&);

	void fullScreen(Ogre::StringVector&);

	void setConsoleLineCount(Ogre::StringVector&);
	void setResolution(Ogre::StringVector&);
	void setOrthoScale(Ogre::StringVector&);
	//void setRobotScale(Ogre::StringVector&);

	void printMap(Ogre::StringVector&);
	void printLightMap(Ogre::StringVector&);
	void printCameraOrientation(Ogre::StringVector&);
	void printCameraDirection(Ogre::StringVector&);

private: // ������ ��� ����

	// ���������� ������ � ������ � �������� ���������
	void set_default_state();
	void clear_lobby_list();
	
	void show_player_list();
	void show_lobby_list();
	void show_merk_list();

	bool req_ExitGame();
	void parse_ExitGame(const std::vector<char>& msg, int& pos);
	void do_ExitGame();

	bool req_CreateMerk(char type_id, char weapon_id, Ogre::String name);
	void parse_CreateMerk(const std::vector<char>& msg, int& pos);
	void do_CreateMerk(int merk_id, UnitDef ud, Ogre::String name, bool log = true);

	bool req_DeleteMerk(int merk_id);
	void parse_DeleteMerk(const std::vector<char>& msg, int& pos);
	void do_DeleteMerk(int merk_id);

	bool req_ChooseMerk(int merk_id);
	void parse_ChooseMerk(const std::vector<char>& msg, int& pos);
	void do_ChooseMerk(int merk_id);

	bool req_ChangeWeapon(int merk_id, char weapon_id);
	void parse_ChangeWeapon(const std::vector<char>& msg, int& pos);
	void do_ChangeWeapon(int merk_id, char weapon_id);

	bool req_MerkList();
	void parse_MerkList(const std::vector<char>& msg, int& pos);

	void parse_DropPlayer(const std::vector<char>& msg, int& pos);
	void parse_DropLobby(const std::vector<char>& msg, int& pos);
	void do_DropLobby(int lobby_id);

	bool req_ChangeMap(char map_id);
	void parse_ChangeMap(const std::vector<char>& msg, int& pos);
	void do_ChangeMap(int lobby_id, char map_id);

	bool req_PlayerList();
	void parse_PlayerList(const std::vector<char>& msg, int& pos);

	bool req_LobbyList();
	void parse_LobbyList(const std::vector<char>& msg, int& pos);

	bool req_JoinLobby(int lobby_id);
	void parse_JoinLobby(const std::vector<char>& msg, int& pos);
	void do_JoinLobby(int lobby_id);

	bool req_DropFromLobby();
	void parse_DropFromLobby(const std::vector<char>& msg, int& pos);
	void do_DropFromLobby(int player_id);

	bool req_PlayerName(int player_id);
	void parse_PlayerName(const std::vector<char>& msg, int& pos);
	void do_ChangePlayerName(int player_id, Ogre::String new_name);

	bool req_CreateLobby(char map_id);
	void parse_CreateLobby(const std::vector<char>& msg, int& pos);
	void do_CreateLobby(int host_id, char map_id, int lobby_id);

	void parse_NewPlayer(const std::vector<char>& msg, int& pos);

	void parse_LobbyNewPlayer(const std::vector<char>& msg, int& pos);
	void do_LobbyNewPlayer(int player_id, char team, bool message = true);

	bool req_PlayersInLobby(int lobby_id);
	void parse_PlayersInLobby(const std::vector<char>& msg, int& pos);
	void show_players_in_lobby();

	std::map<int, Player> m_Players;
	std::map<int, Lobby> m_Lobbies;

	void add_lobby(int host_id, char map_id, int lobby_id);
	void clear_lobbies() { m_Lobbies.clear(); }

	void add_player(int player_id, const Ogre::String& Nick = Ogre::StringUtil::BLANK, bool log = true);
	void add_player(int player_id, Ogre::String&& Nick, bool log = true);
	//void add_player(int player_id);
	void remove_player(int player_id);
	void remove_all_players();
	Ogre::String get_nick(int player_id);
	//void remove_user(int player_id);

	//void set_logged_in(bool logged_in) { m_bLoggedIn = logged_in; }

	//int get_player_id() const { return m_PlayerID; }
	//void set_player_id(int player_id) { m_PlayerID = player_id; }

	//Ogre::String get_name() const { return username_; }
	//void set_name(const Ogre::String& new_name) { username_ = new_name; }

public:

	myapp();
	~myapp();

	bool start();
	//Player* get_player(int player_id);

};

#endif