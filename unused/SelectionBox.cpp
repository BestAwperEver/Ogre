#include "SelectionBox.h"
 
SelectionBox::SelectionBox(const Ogre::String& name)
	: Ogre::ManualObject(name)
	, m_v2Start(0,0)
	, m_v2Stop(0,0)
	, m_bSelecting(false)
{

	Ogre::MaterialPtr matptr = Ogre::MaterialManager::getSingleton().create("BaseColoured","General");
	matptr->load();
	matptr->getBestTechnique()->getPass(0)->setVertexColourTracking(Ogre::TVC_DIFFUSE);
	matptr->getBestTechnique()->getPass(0)->setLightingEnabled(false); 
	matptr->setDiffuse(0,1,0,0.8);
	matptr->setDepthCheckEnabled(false);

	setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY); // when using this, ensure Depth Check is Off in the material
	setUseIdentityProjection(true);
	setUseIdentityView(true);
	setQueryFlags(0);

	/*
	+-450 1116 -98
	*/
}
 
SelectionBox::~SelectionBox()
{
}
 
void SelectionBox::setCorners(float left, float top, float right, float bottom)
{
	left = left * 2 - 1;
	right = right * 2 - 1;
	top = 1 - top * 2;
	bottom = 1 - bottom * 2;
	clear();
//	Ogre::Rectangle2D rect("olol");

	begin("BaseColoured", Ogre::RenderOperation::OT_LINE_STRIP);
			colour(0,1,0);
			position(left, top, -1);
			position(right, top, -1);
			position(right, bottom, -1);
			position(left, bottom, -1);
			position(left, top, -1);
	end();
	setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
}
 
void SelectionBox::setCorners(const Ogre::Vector2& topLeft, const Ogre::Vector2& bottomRight)
{
	setCorners(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
}

void SelectionBox::startSelection(Ogre::Vector2 from) {
	m_v2Start = from;
	m_v2Stop = from;
 
	m_bSelecting = true;

	clear();
	setVisible(true);
}

void SelectionBox::update(Ogre::Vector2 coord) {
	m_v2Stop = coord;
	setCorners(m_v2Start, m_v2Stop);
}

void SelectionBox::finishSelection() {
	m_bSelecting = false;
	setVisible(false);
}