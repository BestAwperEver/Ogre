#include "PlayerCamera.h"


PlayerCamera::PlayerCamera(Ogre::Camera* cam, Ogre::Real maxX, Ogre::Real minX, Ogre::Real maxZ, Ogre::Real minZ)//,
//	void (myapp::*_CallBackFunction)(void), myapp* _CallBackObject)
	: m_pCamera(cam)
	, m_bZooming(false)
	, m_fTopSpeed(400)
	, m_v3Velocity(Ogre::Vector3::ZERO)
	, m_bGoingForward(false)
	, m_bGoingBack(false)
	, m_bGoingLeft(false)
	, m_bGoingRight(false)
	, m_bGoingUp(false)
	, m_bGoingDown(false)
	, m_bRollLeft(false)
	, m_bRollRight(false)
	, m_bFastMove(false)
	, m_fTurboBoost(2)
	, m_minX(minX)
	, m_maxX(maxX)
	, m_minZ(minZ)
	, m_maxZ(maxZ)
	, m_bPitchDown(false)
	, m_bPitchUp(false)
//	, m_CallBackFunction(_CallBackFunction)
//	, m_CallBackObject(_CallBackObject)
{
#ifdef _USE_ASSERTS_
	assert(minX < maxX && minZ < maxZ);
#endif
}


PlayerCamera::~PlayerCamera(void)
{
}

void PlayerCamera::manualStop() {
	m_bGoingForward = false;
	m_bGoingBack = false;
	m_bGoingLeft = false;
	m_bGoingRight = false;
	m_bGoingUp = false;
	m_bGoingDown = false;
	m_v3Velocity = Ogre::Vector3::ZERO;
}

bool PlayerCamera::frameRenderingQueued(const Ogre::FrameEvent& evt) {

	// build our acceleration vector based on keyboard input composite
	Ogre::Vector3 accel = Ogre::Vector3::ZERO;
	//if (m_bGoingRight) accel += m_pCamera->getRight();
	//if (m_bGoingLeft) accel -= m_pCamera->getRight();
	//if (m_bGoingUp) accel += m_pCamera->getUp();
	//if (m_bGoingDown) accel -= m_pCamera->getUp();

	if (m_bGoingRight) accel += m_pCamera->getRight();
	if (m_bGoingLeft) accel -= m_pCamera->getRight();
	if (m_bGoingUp) accel += m_pCamera->getUp();
	if (m_bGoingDown) accel -= m_pCamera->getUp();

	accel.y = 0; // двигаемся только в плоскости XZ, если не указано обратное

	Ogre::Vector3 cameraPos = m_pCamera->getPosition();

	if (cameraPos.x > m_maxX ) {
		if (accel.x > 0) {
			accel.x = 0;
			m_v3Velocity.x = 0;
		}
	} 
	else if (cameraPos.x < m_minX) {
		if (accel.x < 0) {
			accel.x = 0;
			m_v3Velocity.x = 0;
		}
	}
	if (cameraPos.z > m_maxZ) {
		if (accel.z > 0) {
			accel.z = 0;
			m_v3Velocity.z = 0;
		}
	}
	else if (cameraPos.z < m_minZ) {
		if (accel.z < 0) {
			accel.z = 0;
			m_v3Velocity.z = 0;
		}
	}

	if (m_bGoingForward) accel += m_pCamera->getDirection();
	if (m_bGoingBack) accel -= m_pCamera->getDirection();

	if (m_bPitchUp) m_pCamera->pitch(Ogre::Radian(0.3f * (m_bFastMove?2:1) * evt.timeSinceLastFrame));
	if (m_bPitchDown) m_pCamera->pitch(Ogre::Radian(-0.3f * (m_bFastMove?2:1) * evt.timeSinceLastFrame));

	if (m_bRollLeft) 
		m_pCamera->yaw(Ogre::Radian(0.3f * (m_bFastMove?2:1) * evt.timeSinceLastFrame));
	if (m_bRollRight) 
		m_pCamera->yaw(Ogre::Radian(-0.3f * (m_bFastMove?2:1) * evt.timeSinceLastFrame));	

	// if accelerating, try to reach top speed in a certain time
	Ogre::Real topSpeed = m_bFastMove ? m_fTopSpeed * m_fTurboBoost : m_fTopSpeed;
	if (accel.squaredLength() != 0)
	{
		accel.normalise();
		m_v3Velocity += accel * topSpeed * evt.timeSinceLastFrame * 10;
	}
	// if not accelerating, try to stop in a certain time
	else m_v3Velocity -= m_v3Velocity * evt.timeSinceLastFrame * 10;

	Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();

	// keep camera velocity below top speed and above epsilon
	if (m_v3Velocity.squaredLength() > topSpeed * topSpeed)
	{
		m_v3Velocity.normalise();
		m_v3Velocity *= topSpeed;
	}
	else if (m_v3Velocity.squaredLength() < tooSmall * tooSmall) {
		m_v3Velocity = Ogre::Vector3::ZERO;
	}

	if (m_v3Velocity != Ogre::Vector3::ZERO) {
		m_pCamera->move(m_v3Velocity * evt.timeSinceLastFrame);
	}

	//if (m_bPitchUp || m_bPitchDown || m_bRollLeft || m_bRollRight || m_v3Velocity != Ogre::Vector3::ZERO) {
	//	m_CallBackObject->m_CallBackFunction();
	//}

	return true;
}

void PlayerCamera::injectKeyDown(const OIS::KeyEvent& evt) {
	//if (evt.key == OIS::KC_W || evt.key == OIS::KC_UP) mGoingForward = true;
	//else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN) mGoingBack = true;
	//else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT) mGoingLeft = true;
	//else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT) mGoingRight = true;
	//else if (evt.key == OIS::KC_PGUP) mGoingUp = true;
	//else if (evt.key == OIS::KC_PGDOWN) mGoingDown = true;
	//else if (evt.key == OIS::KC_LSHIFT) mFastMove = true;
	switch (evt.key) {
	case OIS::KC_W:
	case OIS::KC_UP:
		{
			m_bGoingUp = true;
		} break;
	case OIS::KC_A:
	case OIS::KC_LEFT:
		{
			m_bGoingLeft = true;
		} break;
	case OIS::KC_S:
	case OIS::KC_DOWN:
		{
			m_bGoingDown = true;
		} break;
	case OIS::KC_D:
	case OIS::KC_RIGHT:
		{
			m_bGoingRight = true;
		} break;
	case OIS::KC_LSHIFT:
		{
			m_bFastMove = true;
		} break;
	case OIS::KC_I:
		{
			m_bPitchDown = false;
			m_bPitchUp = true;
		} break;
	case OIS::KC_K:
		{
			m_bPitchDown = true;
			m_bPitchUp = false;
		} break;
	case OIS::KC_J:
		{
			m_bRollLeft = true;
			m_bRollRight = false;
		} break;
	case OIS::KC_L:
		{
			m_bRollLeft = false;
			m_bRollRight = true;
		} break;
	case OIS::KC_Y:
		{
			m_bGoingForward = true;
		} break;
	case OIS::KC_H:
		{
			m_bGoingBack = true;
		} break;
	default:
		break;
	}
}

void PlayerCamera::injectKeyUp(const OIS::KeyEvent& evt) {
	switch (evt.key) {
	case OIS::KC_W:
	case OIS::KC_UP:
		{
			m_bGoingUp = false;
		} break;
	case OIS::KC_A:
	case OIS::KC_LEFT:
		{
			m_bGoingLeft = false;
		} break;
	case OIS::KC_S:
	case OIS::KC_DOWN:
		{
			m_bGoingDown = false;
		} break;
	case OIS::KC_D:
	case OIS::KC_RIGHT:
		{
			m_bGoingRight = false;
		} break;
	case OIS::KC_LSHIFT:
		{
			m_bFastMove = false;
		} break;
	case OIS::KC_I:
		{
			m_bPitchUp = false;
		} break;
	case OIS::KC_K:
		{
			m_bPitchDown = false;
		} break;
	case OIS::KC_J:
		{
			m_bRollLeft = false;
		} break;
	case OIS::KC_L:
		{
			m_bRollRight = false;
		} break;
	case OIS::KC_Y:
		{
			m_bGoingForward = false;
		} break;
	case OIS::KC_H:
		{
			m_bGoingBack = false;
		} break;
	default:
		break;
	}
}