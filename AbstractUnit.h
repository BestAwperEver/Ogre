#pragma once
#ifndef _Abstract_Unit_h_
#define _Abstract_Unit_h_

//#include "DataBase.h"
#include "..\..\DataBase0\DataBase0\Vector3.h"
#include "..\..\DataBase0\DataBase0\DataBase.h"
#include "..\..\DataBase0\DataBase0\CellCoordinates.h"

//class TerrainDecal;

//class SelectedGroup;
//class GameMap;

class AbstractUnit
{
protected:
	Ogre::Real m_fDistance;							// The distance the object has left to travel
	Ogre::Radian m_rRotatingDistance;
	Ogre::Vector3 m_v3Direction;					// The direction the object is moving
	Ogre::Vector3 m_v3FaceDirection;				// The direction the object is face to
	Ogre::Vector3 m_v3InitialDirection;
	Ogre::Vector3 m_v3Destination;					// The destination the object is moving towards
	Ogre::Vector3 m_v3RotatingDestination;

	Ogre::AnimationState* m_pAnimationState;		// The current animation state of the object

	Ogre::SceneManager*	m_pSceneManager;			// SceneMeneger which contains the unit
	Ogre::Entity*		m_pEntity;
	Ogre::SceneNode*	m_pNode;					// The SceneNode that the Entity is attached to

	// тут всё, что относится к визуальному выделению робота
	// по идее, надо бы это выделить в отдельный класс
	// но лень -__-
	Ogre::Texture*	m_pDecalTexture;
	//Ogre::Material*	m_pMaterialToProject;
	Ogre::Pass*		m_pDecalPass;
	Ogre::Frustum*	m_pDecalFrustum;


	std::deque<Ogre::Vector3> m_WalkList;			// The list of points we are walking to

	Ogre::String m_sName;

	Ogre::Real m_fWalkSpeed;						// The speed at which the object is moving 
	Ogre::Radian m_rRotatingSpeed;

	bool m_bIsAlive;
	bool m_bIsDying;
	bool m_bIsSlumping;
	bool m_bIsActive;
	bool m_bIsWalking;
	bool m_bIsRotating;
	bool m_bIsShooting; // или в него, мне лениво делать две переменные
	bool m_bBlockingRotation;
	bool m_bPaused;
	bool m_bVisible;
	bool m_bNeedUpdate;
	static AbstractUnit* is_busy;

	//SelectedGroup* m_pSelectedGroup; // рудимент

	//char m_PlayerID;
	//char m_Type;
	//char m_Class;
	char m_ActivePoints;
	short m_HitPoints;
	//char m_Weapon;
	//char m_Armor;
	UnitDef m_UnitDef;
	char m_Team;
	short m_ArmorClass;
	UNIT_COLOUR m_Colour;
	CellCoordinates m_Coord;
//public:
//	static GameMap* m_pGameMap;
protected:

	enum SA_RESULT {
		SA_OK,
		SA_ENTITY_IS_NOT_ANIMABLE,
		SA_NO_SUCH_ANIMATION
	};

	void set_walking(bool set);
	void set_shooting(bool set);

public:

	typedef Ogre::SharedPtr<AbstractUnit> ptr;

	static AbstractUnit* busy_unit() { return is_busy; }
	static void remove_all_units();

	AbstractUnit() {}
	AbstractUnit(const AbstractUnit&);
	AbstractUnit(	Ogre::SceneManager *SM,
		Ogre::Vector3 StartDestination,
		//int player_id,
		//char type_id,
		//char Class,
		//char Weapon,
		//char Armor,
		//char AP,
		//short HP,
		UnitDef ud,
		const Ogre::String& Name,
		//					const Ogre::String& Mesh,
		//					const Ogre::String& Material,
		//					Ogre::Vector3 Scale,
		//					Ogre::Vector3 InitialDirection,
		//					Ogre::Real WalkSpeed,
		Ogre::Vector3 StartDirection,
		Ogre::uint32 QueryFlags = 0,
		Ogre::SceneNode *SN = 0);
	virtual ~AbstractUnit(void);
	virtual void removeEntity();

	static std::list<AbstractUnit*> g_Units;

	virtual void createDecal(Ogre::TexturePtr decal_texture, Ogre::MaterialPtr material_to_project_on);
	virtual void removeDecal();
	virtual void setDecalColour(Ogre::ColourValue colour);

	//static Ogre::Vector3	getInitialDirection(char type_id);
	//static Ogre::String		getMesh(char type_id);
	//static Ogre::String		getMaterial(char type_id);
	//static Ogre::Real		getTypeSpeed(char type_id);
	//static Ogre::Real		_DATA.getClassSpeedMod(char Class);
	//static Ogre::Vector3	getScale(char type_id);
	//static Ogre::String		getDefaultName(char type_id);
	//static char				_DATA.getUnitID(const Ogre::String& Type_string);

public:
	virtual bool nextLocation();

	virtual bool rotateToDirection(Ogre::Vector3 Direction, Ogre::Real Time, bool blocking = false);
	virtual bool rotateToDirection(Ogre::Vector3 Direction);
	virtual bool translate(Ogre::Vector3 RelativePos);

	virtual void	setPlayerID(int player_id) {m_UnitDef.PlayerID = player_id;}
	virtual int		getPlayerID() const {return m_UnitDef.PlayerID;}

	virtual void	addWalkPoint(const Ogre::Vector3&);
	virtual void	recalculateArmorClass();
	virtual bool	setColour(char Colour);
	virtual bool	setPosition(Ogre::Vector3 Position);
	virtual int		setAnimation(Ogre::String Animation, bool Loop = true);
	virtual void	setActive(bool Active = true);
	virtual void	setScale(Ogre::Vector3 Scale);
	virtual void	setScale(Ogre::Real scaleX,
							 Ogre::Real scaleY,
							 Ogre::Real scaleZ);
	virtual void	setSpeed(Ogre::Real speed) {m_fWalkSpeed=speed;}
	virtual void	setCoordinates(CellCoordinates c) {m_bNeedUpdate = false;m_Coord=c;}
	virtual void	setName(const Ogre::String& Name);
	virtual void	setActivePoints(unsigned char points) {m_ActivePoints = points;}
	virtual void	setHitPoints(short points) {m_HitPoints = points;}
	virtual void	setWeapon(char Weapon) {m_UnitDef.Weapon = Weapon;}
	virtual void	setArmor(char Armor) {m_UnitDef.Armor = Armor;recalculateArmorClass();}
	virtual void	setClass(char Class);
	virtual void	setViewRange(char Range) {m_UnitDef.ViewRange = Range;}
	virtual void	setVisible(bool visible);
	virtual void	setTeam(char team) {m_Team = team;}

	virtual CellCoordinates			getCoordinates() const {return m_Coord;}
	virtual Ogre::Vector3			getPosition() const;
	virtual Ogre::Real				getSpeed() const {return m_fWalkSpeed;}
	virtual Ogre::int32				getRadius() const;
	virtual Ogre::Vector3			getCenter() const;
	virtual Ogre::AxisAlignedBox	getAAB() const;	//Axis Aligned Box
	virtual Ogre::String			getName() const;
	virtual Ogre::MeshPtr			getMesh() const {return m_pEntity->getMesh();}
	virtual Ogre::Node*				getNode() const {return m_pNode;}
	virtual Ogre::Entity*			getEntity() const {return m_pEntity;}
	virtual char					getAP() const {return m_ActivePoints;}
	virtual short					getHP() const {return m_HitPoints;}
	virtual char					getWeapon() const {return m_UnitDef.Weapon;}
	virtual char					getArmor() const {return m_UnitDef.Armor;}
	virtual short					getArmorClass() const {return m_ArmorClass;}
	virtual short					getAC() const {return m_ArmorClass;}
	virtual char					getClass() const {return m_UnitDef.Class;}
	virtual char					getType() const {return m_UnitDef.type_id;}
	virtual char					getViewRange() const {return m_UnitDef.ViewRange;}
	virtual char					getTeam() const {return m_Team;}

	//virtual void gotDamage(short Damage);

	virtual	bool crashExpected() const;
	Ogre::AxisAlignedBox nextBox(Ogre::Real time = 1, bool self = false) const;

	bool iAmForward(const AbstractUnit*) const;

	virtual bool intersects(const Ogre::Ray&) const;

	//virtual void addToSelectedGroup(SelectedGroup*);
	//virtual void setActive(bool Active = true);

	virtual void stop();

	virtual bool isVisible() const {return m_bVisible;}
	virtual bool isAlive() const {return m_bIsAlive;}
	virtual bool isWalking() const {return m_bIsWalking;}
	virtual bool isRotating() const {return m_bIsRotating;}
	virtual bool isShooting() const {return m_bIsShooting;}
	virtual bool isBusy(bool RotatingCounted = true) const;
	virtual bool needUpdate() const {return m_bNeedUpdate;}

	virtual void moveTo(Ogre::Vector3 Destination);

	virtual void shoot(AbstractUnit* inWho);
	virtual bool checkStatus(Ogre::Real timeSinceLastFrame);
	virtual void getShooted(short Damage);//(AbstractUnit* fromWho);
	virtual void getKilled(AbstractUnit* fromWho);

	virtual bool operator == (const AbstractUnit&);
};

#endif

