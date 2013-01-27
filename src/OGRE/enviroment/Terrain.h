/*
 * Terrain.h
 *
 *  Created on: 25 Jul 2012
 *      Author: jose
 */

#ifndef TERRAIN_H_
#define TERRAIN_H_

#include <OGRE/Terrain/OgreTerrain.h>
#include <OGRE/Terrain/OgreTerrainGroup.h>

class Terrain {
public:
	Terrain();
	virtual ~Terrain();
	void configureTerrain(Ogre::Light*, Ogre::SceneManager*);

private:
	Ogre::TerrainGlobalOptions* mTerrainGlobals;
	Ogre::TerrainGroup* mTerrainGroup;
	bool mTerrainsImported;
	void defineTerrain(long, long);
	void initBlendMaps(Ogre::Terrain*);
	void configureTerrainDefaults(Ogre::Light*, Ogre::SceneManager*);
	void getTerrainImage(bool, bool, Ogre::Image&);
};

#endif /* TERRAIN_H_ */
