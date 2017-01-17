#pragma once
#ifndef _TERRAIN_DECAL_H_
#define _TERRAIN_DECAL_H_

class TerrainDecal
{
protected:
	Ogre::Vector3 mPos;            // center
	Ogre::Vector2 mSize;        // size of decal

	std::string mTexture;        // texture to apply

	Ogre::SceneNode* mNode;        // the projection node
	Ogre::Frustum* mFrustum;    // the projection frustum

	Ogre::SceneManager* mSceneManager;    // pointer to PLSM2

	bool mVisible;                // is the decal visible/active or not?
	bool mInitialized;

	// info about materials that are receiving the decal
	std::map<std::string,Ogre::Pass*> mTargets;            // passes mapped by material names

	bool isPosOnTerrain(Ogre::Vector3 pos);

	std::string getMaterialAtPosition(Ogre::Vector3 pos);

	void addMaterial(std::string matName);

	std::map<std::string,Ogre::Pass*>::iterator removeMaterial(std::string matName);

public:
	TerrainDecal();
	~TerrainDecal();

	void setColour(Ogre::ColourValue);
	void init(Ogre::SceneManager*, Ogre::Vector2 Size, std::string Texture);
	bool initialized() const { return mInitialized; }
	void show();
	void hide();
	void updatePosition(Ogre::Vector3 Position);
	Ogre::Vector3 getPosition() const;
	void updateSize(Ogre::Vector2 Size);
	bool isVisible() const { return mVisible; }
};

#endif