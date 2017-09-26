#include "AbstractUnit.h"
#include "GameMap.h"
//#include "TerrainDecal.h"
//#include "SelectedGroup.h"

//extern std::list<AbstractUnit*> g_Units;

//void AbstractUnit::addToSelectedGroup(SelectedGroup* sg) {
//	sg->addUnit(this);
//	setActive(true);
//	m_pSelectedGroup = sg;
//}

std::list<AbstractUnit*> AbstractUnit::g_Units;
AbstractUnit* AbstractUnit::is_busy = 0;
//GameMap* AbstractUnit::m_pGameMap = 0;

#define COLOUR_YELLOW Ogre::ColourValue(0.8f, 0.8f, 0.2f, 0.9f)
#define COLOUR_GREEN Ogre::ColourValue(0.1f, 0.8f, 0.1f, 0.9f)

AbstractUnit::AbstractUnit(const AbstractUnit& R):
	m_fDistance(R.m_fDistance),
	m_v3Direction(R.m_v3Direction), 
	m_v3FaceDirection(R.m_v3FaceDirection),
	m_v3InitialDirection(R.m_v3InitialDirection), 
	m_v3Destination(R.m_v3Destination), 
	m_pAnimationState(R.m_pAnimationState), 
	m_pSceneManager(R.m_pSceneManager), 
	m_pEntity(R.m_pEntity), 
	m_pNode(R.m_pNode),
	//m_pSelectDecal(R.m_pSelectDecal),
	m_WalkList(R.m_WalkList), 
	m_sName(R.m_sName),
	m_fWalkSpeed(R.m_fWalkSpeed),
	m_bIsActive(R.m_bIsActive),
	m_bIsWalking(R.m_bIsWalking),
	m_bIsRotating(R.m_bIsRotating),
	m_bPaused(R.m_bPaused),
//	m_pSelectedGroup(R.m_pSelectedGroup),
	m_rRotatingDistance(R.m_rRotatingDistance),
	m_v3RotatingDestination(R.m_v3RotatingDestination),
	m_rRotatingSpeed(R.m_rRotatingSpeed),
	m_bIsShooting(R.m_bIsShooting),
	m_Coord(R.m_Coord),
	m_bBlockingRotation(R.m_bBlockingRotation),
	//m_PlayerID(R.m_PlayerID),
	//m_Type(R.m_Type),
	//m_Class(R.m_Class),
	//m_Weapon(R.m_Weapon),
	//m_Armor(R.m_Armor),
	m_UnitDef(R.m_UnitDef),
	m_ActivePoints(R.m_ActivePoints),
	m_HitPoints(R.m_HitPoints),
	m_ArmorClass(R.m_ArmorClass),
	m_bIsAlive(R.m_bIsAlive),
	m_bIsDying(R.m_bIsDying),
	m_bIsSlumping(R.m_bIsSlumping),
	m_bVisible(R.m_bVisible),
	m_bNeedUpdate(R.m_bNeedUpdate)
{
	g_Units.push_front(this);
	//if (m_pSelectedGroup)
	//	m_pSelectedGroup->addUnit(this);
	//m_pEntity->setUserAny(Ogre::Any(this));
}

AbstractUnit::AbstractUnit(	Ogre::SceneManager *SM,
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
//				const Ogre::String& Mesh,
//				const Ogre::String& Material,
//				Ogre::Vector3 Scale,
//				Ogre::Vector3 InitialDirection,
//				Ogre::Real WalkSpeed,
		Ogre::Vector3 StartDirection,
		Ogre::uint32 QueryFlags,
		Ogre::SceneNode *SN)
	: m_pSceneManager(SM)
	, m_fWalkSpeed(_DATA.getTypeSpeed(ud.type_id))
	, m_pNode(SN)
	//, m_pSelectDecal(nullptr)
	, m_v3Direction(_DATA.getInitialDirection(ud.type_id))
	, m_v3FaceDirection(_DATA.getInitialDirection(ud.type_id))
	, m_v3Destination(0.0f)
	, m_bIsWalking(false)
	, m_bIsRotating(false)
	, m_pAnimationState(0)
	, m_bIsActive(false)
	, m_sName(Name)
	, m_v3InitialDirection(_DATA.getInitialDirection(ud.type_id))
//		, m_pSelectedGroup(0)
	, m_bPaused(false)
	, m_fDistance(0)
	, m_rRotatingDistance(0)
	, m_v3RotatingDestination(0.0f)
	, m_rRotatingSpeed(0)
	, m_bIsShooting(false)
	, m_bBlockingRotation(false)
	, m_UnitDef(ud)
	//, m_PlayerID(player_id)
	//, m_Type(type_id)
	, m_ActivePoints(ud.AP)
	, m_HitPoints(ud.HP)
	//, m_Weapon(Weapon)
	//, m_Armor(Armor)
	//, m_Class(_DATA.getDefaultClass(type_id))
	, m_ArmorClass(0)
	, m_bIsAlive(true)
	, m_bIsDying(false)
	, m_bIsSlumping(false)
	, m_bVisible(false)
	, m_bNeedUpdate(true)
	//, m_pMaterialToProject(nullptr)
	, m_pDecalPass(nullptr)
	, m_pDecalFrustum(nullptr)
{	
#ifdef _USE_ASSERTS_
	assert( SM );
#endif

	//char buf[4];
	//_itoa_s<4>(g_Units.size(), buf, 16);
	//Ogre::String name = Name + buf;

	g_Units.push_front(this);

	m_pEntity = m_pSceneManager->createEntity(_DATA.getMesh(ud.type_id));
	//m_pEntity->setQueryFlags(ABSTRACT_UNIT_QUERY_FLAG | QueryFlags);
	//m_pEntity->setUserAny(Ogre::Any(this));
	m_pEntity->setMaterialName(_DATA.getMaterial(ud.type_id));
	m_pEntity->setVisible(m_bVisible);

	// Create the scene node
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
	if (!m_pNode) m_pNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode(Ogre::SCENE_DYNAMIC, StartDestination);
#else
	if (!m_pNode) m_pNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode(StartDestination);
#endif
	m_pNode->attachObject(m_pEntity);

	m_pNode->setScale(_DATA.getScale(ud.type_id));

	recalculateArmorClass();
	m_fWalkSpeed *= _DATA.getClassSpeedMod(ud.Class);

	rotateToDirection(StartDirection);
}

AbstractUnit::~AbstractUnit(void)
{
	//if (m_pSelectDecal) {
	//	delete m_pSelectDecal;
	//	m_pSelectDecal = 0;
	//}
	//if (m_pDecalFrustum) {
	//	m_pDecalFrustum->detachFromParent();
	//	delete m_pDecalFrustum;
	//	m_pDecalFrustum = 0;
	//}
	//if (m_pDecalPass) {
	//	m_pDecalPass->getParent()->removePass(m_pDecalPass->getIndex());
	//	m_pDecalPass->hasP
	//}
	g_Units.remove(this);
}

void AbstractUnit::removeEntity() {
	if (m_pDecalPass) {
		m_pDecalPass->getParent()->removePass(m_pDecalPass->getIndex());
	}
	//if (m_pDecalFrustum) {
	//	m_pDecalFrustum->detachFromParent();
	//	delete m_pDecalFrustum;
	//	m_pDecalFrustum = 0;
	//}
	if (m_pNode) {
		m_pNode->detachObject(m_pEntity);
	} else return;
	if (m_pSceneManager) {
		m_pSceneManager->destroyEntity(m_pEntity);
	} else return;
	
	// optionally destroy node
	m_pSceneManager->destroySceneNode(m_pNode);

	m_pNode = 0;
	m_pEntity = 0;
	m_pSceneManager = 0;
}

void AbstractUnit::addWalkPoint(const Ogre::Vector3& dest) {
	m_WalkList.push_front(dest);
}
bool AbstractUnit::nextLocation() {
	if (m_WalkList.empty())
		return false;

	if (!m_pNode) return false;

	m_v3Destination = m_WalkList.front();  // this gets the front of the deque
	
	set_walking(true);
    m_v3Direction = m_v3Destination - m_pNode->getPosition();
    m_fDistance = m_v3Direction.normalise();
	return true;
}

bool AbstractUnit::rotateToDirection(Ogre::Vector3 Direction, Ogre::Real Time, bool blocking) {
#ifdef _USE_ASSERTS_
	assert ( Time > 0 );
#endif

	m_bBlockingRotation = blocking;

	Direction.y = 0;
	Direction.normalise();
	m_v3FaceDirection.y = 0;
	m_v3FaceDirection.normalise();

	if (abs(Time) < 0.2f) return rotateToDirection(Direction);

	if (Direction == Ogre::Vector3::ZERO) return false;

	if ( Direction.dotProduct(m_v3FaceDirection) - 1 > -0.05 ) return rotateToDirection(Direction);

	m_rRotatingDistance = m_v3FaceDirection.angleBetween(Direction);

	if (m_v3FaceDirection.crossProduct(Direction).dotProduct(Ogre::Vector3::UNIT_Y) < 0) {
		m_rRotatingDistance = -m_rRotatingDistance;
	}

	m_v3RotatingDestination = Direction;
	m_bIsRotating = true;
	m_rRotatingSpeed = m_rRotatingDistance / Time;

	return true;
}
bool AbstractUnit::rotateToDirection(Ogre::Vector3 Direction) {

	if ( Direction == Ogre::Vector3::ZERO)
		return false;

	Ogre::Vector3 src = m_pNode->getOrientation() * m_v3InitialDirection;	// Orientation from initial direction
	src.y = 0;	
	m_v3Direction = Direction;
	m_v3FaceDirection = Direction;
	Direction.y = 0;
	src.normalise();
	Direction.normalise();

	if ((1.0f + src.dotProduct(Direction)) < 0.0001f) 
        {
            m_pNode->yaw(Ogre::Degree(180));
        }
        else
        {
            Ogre::Quaternion quat = src.getRotationTo(Direction);
            m_pNode->rotate(quat);
        } // else

	return true;
}

bool AbstractUnit::setPosition(Ogre::Vector3 Position) {

	if ( Position == m_pNode->getPosition() )
		return false;

	m_pNode->setPosition(Position);
	//if (m_pSelectDecal) m_pSelectDecal->updatePosition(Position);

	return true;
}
int AbstractUnit::setAnimation(Ogre::String Animation, bool Loop) {
	if (!m_pEntity->getAllAnimationStates())
		return SA_ENTITY_IS_NOT_ANIMABLE;

	if (m_pAnimationState && 
		m_pAnimationState->getAnimationName() == Animation) 
		if (m_pAnimationState->getLoop() == Loop) {
			return SA_OK;
		}
		else {
			m_pAnimationState->setLoop(Loop);
			return SA_OK;
		}

	if (m_pEntity->hasAnimationState(Animation)) {

		if (m_pAnimationState) {
			m_pAnimationState->setLoop(false);
			//mAnimationState->addTime(5);
			m_pAnimationState->setEnabled(false);
		}

		m_pAnimationState = m_pEntity->getAnimationState(Animation);

		m_pAnimationState->setLoop(Loop);
		m_pAnimationState->setEnabled(true);
			
		return SA_OK;
	}

	return SA_NO_SUCH_ANIMATION;
}

Ogre::Vector3 AbstractUnit::getPosition() const {
	return m_pNode->getPosition();
}
Ogre::int32 AbstractUnit::getRadius() const {
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
	Ogre::Aabb box = m_pEntity->getWorldAabbUpdated();
#else
	Ogre::Sphere box = m_pEntity->getWorldBoundingSphere();
#endif
	
	return box.getRadius() / 2;
}

bool AbstractUnit::intersects(const Ogre::Ray& ray) const {
	if ( !isVisible() )
		return false;
	//if ( !m_pEntity->getWorldAabb().isFinite() )
	//	return false;
	return ray.intersects(getAAB()).first;
}

void AbstractUnit::setScale(Ogre::Vector3 scale) {
	m_pNode->setScale(scale);
}
void AbstractUnit::setScale(Ogre::Real sX, Ogre::Real sY, Ogre::Real sZ) {
	m_pNode->setScale(sX, sY, sZ);
}

void AbstractUnit::stop() {
	m_WalkList.clear();
	set_walking(false);
	m_bIsRotating = false;
	m_bPaused = false;
	setAnimation("Idle");
}

Ogre::Vector3 AbstractUnit::getCenter() const {
//#ifdef _USE_ASSERTS_
//	assert ( m_pEntity->getWorldBoundingBox().isFinite() );
//#endif
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
	return m_pEntity->getWorldAabbUpdated().mCenter;
#else
	return m_pEntity->getWorldBoundingBox().getCenter();
#endif
}
Ogre::AxisAlignedBox AbstractUnit::getAAB() const {
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
	Ogre::Aabb box = m_pEntity->getWorldAabbUpdated();
	Ogre::AxisAlignedBox old_box(box.getMinimum(), box.getMaximum());
	return old_box;
#else
	return m_pEntity->getWorldBoundingBox();
#endif
}
Ogre::String AbstractUnit::getName() const {
	return m_sName;//_DATA.getDefaultName(m_Type);
}

bool AbstractUnit::iAmForward(const AbstractUnit* r) const {
	if (!(r->getAAB().isFinite() && getAAB().isFinite())) {
		return true;
	}
	Ogre::Vector3 mycenter = getAAB().getCenter();
	Ogre::Vector3 itscenter = r->getAAB().getCenter();
	Ogre::Vector3 vec = mycenter - itscenter;
	return (vec + 0.1 * m_v3Direction).length() > vec.length();
}

Ogre::AxisAlignedBox AbstractUnit::nextBox(Ogre::Real time, bool self) const {
//#ifdef _USE_ASSERTS_
//	assert( m_pEntity->getWorldBoundingBox().isFinite() );
//#endif
	const float scale = 0.4;
	if (!m_bIsWalking || (m_bPaused && !self)) {
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
		Ogre::Aabb new_box = m_pEntity->getWorldAabbUpdated();
		Ogre::AxisAlignedBox box(new_box.getMinimum(), new_box.getMaximum());
#else
		Ogre::AxisAlignedBox box(getAAB());
#endif
		box.scale(Ogre::Vector3(scale, 1, scale));
		return box;
	}

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
	Ogre::Aabb new_box = m_pEntity->getWorldAabbUpdated();
	Ogre::AxisAlignedBox box(new_box.getMinimum(), new_box.getMaximum());
#else
	Ogre::AxisAlignedBox box(getAAB());
#endif
	Ogre::Vector3 mv = box.getMinimum(),
		Mv = box.getMaximum(),
		nmv = mv + time * m_v3Direction * m_fWalkSpeed,
		nMv = Mv + time * m_v3Direction * m_fWalkSpeed;
	box = Ogre::AxisAlignedBox(nmv, nMv);
	box.scale(Ogre::Vector3(scale, 1, scale));
	return box;
}

bool AbstractUnit::crashExpected() const {
	//if (!m_pEntity->getWorldBoundingBox().isFinite()) {
	//	return false;
	//}
	//const Ogre::Real time = 0.1f;
	//Ogre::AxisAlignedBox 
	//	curBox = nextBox(time, true), 
	//	itBox;
	//for (std::list<AbstractUnit*>::iterator it = g_Units.begin(); it != g_Units.end(); it++) {
	//	if (*it == this) {// && !curBox.isFinite()) {
	//		continue;
	//	}
	//	if (!(*it)->m_pEntity->getWorldBoundingBox().isFinite()) {
	//		continue;
	//	}
	//	if (
	//	//	((m_pNode->getOrientation() * Ogre::Vector3::UNIT_X).dotProduct(
	//	//	((*it)->m_pNode->getOrientation() * Ogre::Vector3::UNIT_X)) - 1 > -0.1f // если движемся в одну сторону
	//	//	&& 
	//		iAmForward(*it) // если я впереди
	//		|| (abs((m_pNode->getOrientation() * m_v3InitialDirection).dotProduct(
	//		((*it)->m_pNode->getOrientation() * (*it)->m_v3InitialDirection))) < 0.2f // или почти ортогонально
	//		&& ((*it)->m_bPaused || !((*it)->m_bIsWalking))))	// и он уже на паузе или вообще не двигается
	//		// очень спорный момент, тут надо думать и думать
	//		continue;
	//	itBox = (*it)->nextBox(time);
	//	if (curBox.intersects(itBox)) 
	//		return true;
	//}
	return false;
}

bool AbstractUnit::translate(Ogre::Vector3 pos) {
#ifdef _USE_ASSERTS_
	assert( m_pNode );
#endif
	// из-за этого в релизе не ходил
	//if (pos.squaredLength() < 1) {
	//	return false;
	//}
	m_pNode->translate(pos);
	return true;
}

void AbstractUnit::moveTo(Ogre::Vector3 Destination) {
	m_v3Destination = Destination;
	set_walking(false);

	if (m_WalkList.empty()) {
		m_WalkList.push_front(Destination);
	}
	else {
		m_WalkList.clear();
		m_WalkList.push_front(Destination);
	}
}

void AbstractUnit::shoot(AbstractUnit* inWho) {
	set_shooting(true);
	rotateToDirection(inWho->getPosition() - getPosition(), 0.2f, true);
	setAnimation("Shoot", false);
}
void AbstractUnit::getShooted(short Damage) {//AbstractUnit* fromWho) {
#ifdef _USE_ASSERTS_
	assert( Damage < m_HitPoints );
#endif
	//rotateToDirection(fromWho->getPosition() - getPosition(), 0.4f);
	m_HitPoints -= Damage;
	setAnimation("Slump", false);
	m_bIsSlumping = true;
}
void AbstractUnit::getKilled(AbstractUnit* fromWho) {
	m_bIsDying = true;
	rotateToDirection(fromWho->getPosition() - getPosition(), 0.2f, true);
	//if (m_pSelectDecal) {
	//	delete m_pSelectDecal;
	//	m_pSelectDecal = 0;
	//}
	setAnimation("Die", false);
}
void AbstractUnit::recalculateArmorClass() {
	m_ArmorClass = _DATA.calculateArmorClass(m_UnitDef);
		//_DATA.getDefaultAC(type_id) +
		//_DATA.getClassACBonus(m_Class) +
		//_DATA.getArmorAC(m_Armor);
}

bool AbstractUnit::checkStatus(Ogre::Real time) {
	if (!m_bIsAlive) {
		return true;
	}
#ifdef _USE_ASSERTS_
	assert(m_pNode);
#endif
	if (m_bIsRotating) {
		//m_pEntity->setMaterialName("Redbot");

		Ogre::Radian move = m_rRotatingSpeed * time;

		if (abs(m_rRotatingDistance.valueRadians()) <= abs(move.valueRadians()))
		{	
			rotateToDirection(m_v3RotatingDestination);
			m_rRotatingSpeed = Ogre::Radian();
			m_rRotatingDistance = Ogre::Radian();
			m_bIsRotating = false;
		}
		else
		{
			m_pNode->yaw(move);
		}

		m_rRotatingDistance -= move;

		if (m_bBlockingRotation) {
			//if (m_pAnimationState) m_pAnimationState->addTime(time);
			return true;
		}

	}
	//else {
	//	m_pEntity->setMaterialName("Robot");
	//}

	if ((m_bIsShooting || m_bIsSlumping) && m_pAnimationState->hasEnded()) {
		set_shooting(false);
		m_bIsSlumping = false;
		m_pAnimationState->setTimePosition(0);
		setAnimation("Idle");
	}
	if (m_bIsDying && m_pAnimationState->hasEnded()) {
		m_bIsDying = false;
		m_bIsAlive = false;
		//m_pAnimationState->setTimePosition(0);
		//setAnimation("Idle");
	}

	if (m_bIsWalking == false)//(mDirection == Ogre::Vector3::ZERO) 
    {
		if (nextLocation()) 
		{
			// Set walking animation
			setAnimation("Walk");

			rotateToDirection(m_v3Direction, 0.2f);
		}
    }
	else
    {
		//if (g_Units.size() > 1) {
		//	if (crashExpected()) {
		//		if (!m_bPaused) {
		//			setAnimation("Idle");
		//			m_bPaused = true;
		//		}
		//		m_pAnimationState->addTime(time);
		//		return true;
		//	}
		//	else if (m_bPaused) {
		//		m_bPaused = false;
		//		setAnimation("Walk");
		//	}
		//}

		Ogre::Real move = m_fWalkSpeed * time;
		m_fDistance -= move;
		if (m_fDistance <= 0.0f)
		{	
			m_WalkList.pop_front();             // this removes the front of the deque
			setPosition(m_v3Destination);
			m_bNeedUpdate = true;
			//m_v3Direction = Ogre::Vector3::ZERO;
			set_walking(false);
			// Set animation based on if the robot has another point to walk to. 
			if (! nextLocation())
			{
				// Set Idle animation                     
				setAnimation("Idle");
				//mFinished = true;
			} 
			else
			{
				rotateToDirection(m_v3Direction, 0.2f);
				//Vector3 mDestination = mWalkList.front( );                    // mDestination is the next location
				//Vector3 mDirection = mDestination - m_pNode->getPosition();     // B-A = A->B (see vector questions above)
				//Vector3 src = m_pNode->getOrientation() * Vector3::UNIT_X;      // Orientation from initial direction
				//src.y = 0;                                                    // Ignore pitch difference angle
				//mDirection.y = 0;
				//src.normalise();
				//Real m_fDistance = mDirection.normalise( );                     // Both vectors modified so renormalize them
				//Quaternion quat = src.getRotationTo(mDirection);
 
				//m_pNode->rotate(quat);
			}
		}
		else
		{
			/*m_pNode->*/translate(m_v3Direction * move);
			//if (m_pSelectDecal) m_pSelectDecal->updatePosition(m_pSelectDecal->getPosition()  + m_v3Direction * move);
			//if (m_pLight) m_pLight->setPosition(m_pLight->getPosition() + m_v3Direction * move);
		} // else
	} // if

	if (m_pAnimationState) m_pAnimationState->addTime(time);

	return true;
}

bool AbstractUnit::operator == (const AbstractUnit& r) {
	return m_sName == r.m_sName;
}

void AbstractUnit::setActive(bool Active) {
#ifdef _USE_ASSERTS_
//	assert (m_pEntity->getWorldBoundingBox().isFinite() && "Infinite unit bounding box");
	assert (m_pDecalPass && "Trying to select unit without decal pass");
#endif
	////m_pNode->showBoundingBox(Active);
	setDecalColour(Active ? COLOUR_GREEN : COLOUR_YELLOW);
	////if (!m_pSelectDecal) {
	////	m_pSelectDecal = new TerrainDecal;
	////}
	////if (!m_pSelectDecal->initialized()) {
	////	m_pSelectDecal->init(m_pSceneManager, Ogre::Vector2(40, 40), "selection_decal.png");
	////}
	////m_pSelectDecal->updatePosition(Ogre::Vector3(getCenter().x, 0, getCenter().z)); // возможно, надо брать центр и занулять игрик
	////m_pSelectDecal->show();
	////if (Active) {
	////	m_pSelectDecal->setColour(Ogre::ColourValue::Green);
	////} else {
	////	m_pSelectDecal->setColour(Ogre::ColourValue(0.8f, 0.8f, 0.0f)); // yellow
	////}
	m_bIsActive = Active;
	//if (!m_pLight) {	
	//	//Ogre::Light* spotLight = m_pSceneManager->createLight("spotLight");
 ////   spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
 ////   spotLight->setDiffuseColour(0, 0, 1.0);
 ////   spotLight->setSpecularColour(0, 0, 1.0);
 ////
 ////   spotLight->setDirection(0, -1, 0);
 ////   spotLight->setPosition(Ogre::Vector3(-900, 100, 900));
 ////
 ////   spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(40));
	//	m_pLight = m_pSceneManager->createLight(m_sName + "Light");
	//	m_pLight->setType(Ogre::Light::LT_SPOTLIGHT);

	//	m_pLight->setDiffuseColour(Ogre::ColourValue::Blue);
	//	m_pLight->setSpecularColour(Ogre::ColourValue::Blue);

	//	m_pLight->setDirection(0,-1,0);
	//	Ogre::Vector3 ololo = getPosition();
	//	ololo.y += m_pEntity->getBoundingBox().getSize().y*2;
	//	m_pLight->setPosition(ololo);
	//	// т.е. над роботом на высоте его баундинг бокса, хз зачем я так делаю
	//	m_pLight->setSpotlightRange(Ogre::Degree(15), Ogre::Degree(20));
	//	m_pLight->setAttenuation(ololo.y - 1, 
	//		m_pLight->getAttenuationConstant(),
	//		m_pLight->getAttenuationLinear(),
	//		m_pLight->getAttenuationQuadric());

	//	m_pLight->setCastShadows(false);
	//	//m_pLight->detachFromParent();
	//	//m_pNode->attachObject(m_pLight);
	//	m_pLight->setVisible(Active);
	//}
	//else {
	//	m_pLight->setVisible(Active);
	//}
	//if (!Active) {
	//	//m_pSelectedGroup->removeUnit(this);
	//	auto m_pMat = m_pEntity->getSubEntity(0)->_DATA.getMaterial();
	//	m_pMat->getTechnique(0)->getPass(0)->setAmbient(0, 0, 1);
	//	m_pMat->getTechnique(0)->getPass(0)->setDiffuse(0, 0, 1, 0);
	//	//m_pEntity->setMaterialName(m_pMat->getName());
	//	m_pEntity->setMaterial(m_pMat);
	//	m_pSelectedGroup = 0;
	//}
	//else {
	//	m_pEntity->getSubEntity(0)->_DATA.getMaterial()->getBestTechnique()->getPass(0)->setAmbient(Ogre::ColourValue::Red);
	//}

}

bool AbstractUnit::setColour(char COLOUR) {
	if (COLOUR == static_cast<char>(UNIT_COLOUR::RED)) {
		m_pEntity->setMaterialName("Redbot");
	} else if (COLOUR == static_cast<char>(UNIT_COLOUR::BLUE)) {
		m_pEntity->setMaterialName("Bluebot");
	} else if (COLOUR == static_cast<char>(UNIT_COLOUR::GREEN)) {
		m_pEntity->setMaterialName("Greenbot");
	} else if (COLOUR == static_cast<char>(UNIT_COLOUR::YELLOW)) {
		m_pEntity->setMaterialName("Yellowbot");
	} else if (COLOUR == static_cast<char>(UNIT_COLOUR::BROWN)) {
		m_pEntity->setMaterialName("Brownbot");
	} else if (COLOUR == static_cast<char>(UNIT_COLOUR::ORANGE)) {
		m_pEntity->setMaterialName("Orangebot");
	} else {
		m_pEntity->setMaterialName("Robot");
	}
	return COLOUR != static_cast<char>(UNIT_COLOUR::DEFAULT_COLOUR);
}
void AbstractUnit::setClass(char Class) {
	m_UnitDef.Class = Class;

	//switch (Class) {
	//case HERO_CLASS::MAGE:
	//	m_pEntity->setMaterialName("Orangebot");
	//	break;
	//case HERO_CLASS::MONK:
	//	m_pEntity->setMaterialName("Bluebot");
	//	break;
	//case HERO_CLASS::ROGUE:
	//	m_pEntity->setMaterialName("Redbot");
	//	break;
	//case HERO_CLASS::WARRIOR:
	//	m_pEntity->setMaterialName("Brownbot");
	//	break;
	//}

	recalculateArmorClass();
}
void AbstractUnit::setName(const Ogre::String& name) {
	m_sName = name;
}

void AbstractUnit::set_walking(bool set) {
	m_bIsWalking = set;
	if (set) 
		is_busy = this;
	else 
		is_busy = 0;
}
void AbstractUnit::set_shooting(bool set) {
	m_bIsShooting = set;
	if (set) 
		is_busy = this;
	else 
		is_busy = 0;
}

bool AbstractUnit::isBusy(bool RotatingCounted) const {
		return m_bIsWalking || m_bIsShooting || m_bIsDying || !m_bIsAlive || (m_bIsRotating && RotatingCounted);
}

void AbstractUnit::setVisible(bool visible) { 
	m_bVisible = visible;
	m_pEntity->setVisible(visible);
}

void AbstractUnit::createDecal(Ogre::TexturePtr sourceTexture, Ogre::MaterialPtr material) {
	Ogre::TexturePtr manualTexture = Ogre::TextureManager::getSingleton().createManual(
		"DecalTexture", "General", Ogre::TEX_TYPE_2D,
		sourceTexture->getWidth(), sourceTexture->getHeight(),
		sourceTexture->getNumMipmaps(),
		sourceTexture->getFormat());

	// Get the pixel buffer
	Ogre::HardwarePixelBufferSharedPtr	pixelBuffer = manualTexture->getBuffer(),
										sourcePixelBuffer = sourceTexture->getBuffer();

	// Lock the pixel buffer and get a pixel box
	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD); // for best performance use HBL_DISCARD!
	sourcePixelBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);
	const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
	const Ogre::PixelBox& rockBox = sourcePixelBuffer->getCurrentLock();

	Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);
	Ogre::uint8* pSource = static_cast<Ogre::uint8*>(rockBox.data);

	memcpy(pDest, pSource, sourceTexture->getWidth()*sourceTexture->getHeight()*4);

	pixelBuffer->unlock();
	sourcePixelBuffer->unlock();

	m_pDecalTexture = manualTexture.get();
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
	m_pDecalFrustum = new Ogre::Frustum(Ogre::Id::generateNewId<Ogre::Frustum>(),
		&m_pSceneManager->_getEntityMemoryManager(Ogre::SCENE_DYNAMIC));
#else
	m_pDecalFrustum = new Ogre::Frustum();
#endif
	m_pDecalFrustum->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	m_pDecalFrustum->setOrthoWindowHeight(66.6f); // some hardcoded value?
	m_pDecalFrustum->setAspectRatio(1.0f);

	auto projectorNode = m_pNode->createChildSceneNode();
	//auto projectorNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
	projectorNode->attachObject(m_pDecalFrustum);
	//projectorNode->setPosition(0, 0, 0);//getPosition());
	projectorNode->setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y);
	
	m_pDecalPass = material->getTechnique(0)->createPass();
	m_pDecalPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	//m_pDecalPass->setSceneBlending(Ogre::SBT_TRANSPARENT_COLOUR);

	//m_pDecalPass->setDepthBias(1); // no alpha
	//m_pDecalPass->setLightingEnabled(false); // no lightning

	m_pDecalPass->setAmbient(COLOUR_YELLOW);
	m_pDecalPass->setEmissive(COLOUR_YELLOW);
	m_pDecalPass->setDiffuse(COLOUR_YELLOW);

	Ogre::TextureUnitState* texState = m_pDecalPass->createTextureUnitState(manualTexture->getName());
	texState->setProjectiveTexturing(true, m_pDecalFrustum);
	texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	texState->setTextureFiltering(Ogre::FO_POINT, Ogre::FO_LINEAR, Ogre::FO_NONE);
}

void AbstractUnit::removeDecal() {
	if (m_pDecalPass) {
		m_pDecalPass->getParent()->removePass(m_pDecalPass->getIndex());
		m_pDecalPass = 0;
	}
	if (m_pDecalFrustum) {
		m_pDecalFrustum->detachFromParent();
		delete m_pDecalFrustum;
		m_pDecalFrustum = 0;
	}
}
void AbstractUnit::setDecalColour(Ogre::ColourValue colour) {
	if (m_pDecalPass) {
		m_pDecalPass->setAmbient(colour);
		m_pDecalPass->setEmissive(colour);
		m_pDecalPass->setDiffuse(colour);
	}
}