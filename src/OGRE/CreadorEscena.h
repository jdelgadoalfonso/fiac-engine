/*
 * CreadorEscena.h
 *
 *  Created on: 14 Jul 2012
 *      Author: jose
 */

#ifndef CREADORESCENA_H_
#define CREADORESCENA_H_

#include "MyOgre.h"
#include "objetos/Entidades.h"
#include "../threads/Mutex.h"

class CreadorEscena: public MyOgre, Mutex {
public:
	CreadorEscena();
	virtual ~CreadorEscena();

	bool sceneLoader(const char*, const char*, const char*);

private:
	std::map<Ogre::String, Ogre::AnimationState*> mAnimations;
	std::map<Ogre::String, Ogre::Entity*> mEntities;
	std::map<Ogre::String, Ogre::SceneNode*> mSceneNode;
	std::map<Ogre::String, Ogre::Light*> mLights;
	std::list<Entidades> *mQueue;

	bool mAddPerson;

	bool frameRenderingQueued(const Ogre::FrameEvent&);
	bool processQueue();
};

#endif /* CREADORESCENA_H_ */
