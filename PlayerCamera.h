#pragma once
#ifndef _Player_Camera_h_
#define _Player_Camera_h_

#define SQRT_2 1.41f

//class myapp;

class PlayerCamera
{
//	void (myapp::*m_CallBackFunction)(void);
//	myapp* m_CallBackObject;

	Ogre::Camera* m_pCamera;
//	CameraStyle m_Style;
	bool m_bZooming;
	Ogre::Real m_fTopSpeed;
	Ogre::Real m_fTurboBoost;
	Ogre::Vector3 m_v3Velocity;

	bool m_bGoingForward;
	bool m_bGoingBack;

	bool m_bPitchUp;
	bool m_bPitchDown;

	bool m_bRollLeft;
	bool m_bRollRight;

	bool m_bGoingLeft;
	bool m_bGoingRight;
	bool m_bGoingUp;
	bool m_bGoingDown;
	bool m_bFastMove;

	Ogre::Real m_maxX;
	Ogre::Real m_minX;
	Ogre::Real m_maxZ;
	Ogre::Real m_minZ;

public:
	PlayerCamera(Ogre::Camera*, Ogre::Real maxX,
							Ogre::Real minX,
							Ogre::Real maxZ,
							Ogre::Real minZ);//,
							//void (myapp::*_callcackFunction)(void),
							//myapp* _callbackObject);
	~PlayerCamera(void);

	void setCamera(Ogre::Camera* cam)
	{
		m_pCamera = cam;
	}
	void setTopSpeed(Ogre::Real topSpeed)
	{
		m_fTopSpeed = topSpeed;
	}
	void setTurboBoost(Ogre::Real turboBoost) 
	{
		m_fTurboBoost = turboBoost;
	}

	Ogre::Camera*	getCamera()
	{
		return m_pCamera;
	}
	Ogre::Real		getTopSpeed()
	{
		return m_fTopSpeed;
	}
	Ogre::Real		getTurboBoost()
	{
		return m_fTurboBoost;
	}

	void manualStop();

	bool frameRenderingQueued(const Ogre::FrameEvent&);

	void injectKeyDown(const OIS::KeyEvent&);
	void injectKeyUp(const OIS::KeyEvent&);
};

#endif

