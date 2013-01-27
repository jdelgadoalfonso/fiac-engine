/*
 * LowLevelOgre.h
 *
 *  Created on: 01/06/2012
 *      Author: jose
 */

#ifndef LOWLEVELOGRE_H_
#define LOWLEVELOGRE_H_

#include <OGRE/OgreRoot.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreWindowEventUtilities.h>
#include <OGRE/OgrePlugin.h>


class LowLevelOgre {
public:
	LowLevelOgre(void);
	virtual ~LowLevelOgre(void);
	bool go(void);
protected:
	Ogre::Root *mRoot;
	Ogre::Camera* mCamera;
	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mWindow;
};

#endif /* LOWLEVELOGRE_H_ */
