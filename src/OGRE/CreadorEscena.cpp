/*
 * CreadorEscena.cpp
 *
 *  Created on: 14 Jul 2012
 *      Author: jose
 */

#include "CreadorEscena.h"

CreadorEscena::CreadorEscena() {
	mAddPerson = false;
	mQueue = new std::list<Entidades>();
}

CreadorEscena::~CreadorEscena() {
	delete mQueue;
}

bool CreadorEscena::frameRenderingQueued(const Ogre::FrameEvent& evt) {
	if(mWindow->isClosed()) {
		return false;
	}

	if(mShutDown) {
		return false;
	}

	bloquearMutex();
	if (mAddPerson) {
		mAddPerson = false;
		processQueue();
	}
	desbloquearMutex();

	//Need to capture/update each device

	mKeyboard->capture();
	mMouse->capture();

	return true;
}

bool CreadorEscena::sceneLoader(
		const char* name, const char* mesh, const char* nodeName) {

	if(mWindow->isClosed() || mShutDown) {
		return false;
	}

	bloquearMutex();

	Entidades pEntidad;
	pEntidad.setEscenaNodeName(nodeName);
	pEntidad.setMesh(mesh);
	pEntidad.setName(name);
	mQueue->push_front(pEntidad);

	mAddPerson = true;
	desbloquearMutex();

	return true;
}

bool CreadorEscena::processQueue() {
	Entidades pEntidades = mQueue->back();

	mPersonEntity = mSceneMgr->createEntity(
			pEntidades.getName(),
			pEntidades.getMesh()
	);
	mPersonEntity->setCastShadows(true);

	Ogre::SceneNode *nodePerson = mSceneMgr->getRootSceneNode()->
			createChildSceneNode(pEntidades.getEscenaNodeName());
	nodePerson->attachObject(mPersonEntity);
	if (pEntidades.getPosition()) {
		nodePerson->setPosition(*pEntidades.getPosition());
	} else {
		nodePerson->setPosition(0, 5.75, 0);
	}

	mQueue->pop_back();
	return true;
}
