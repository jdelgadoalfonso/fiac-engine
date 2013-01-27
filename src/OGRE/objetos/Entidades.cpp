/*
 * Entidades.cpp
 *
 *  Created on: 22 Jul 2012
 *      Author: jose
 */

#include "Entidades.h"

Entidades::Entidades() {
	mPosition = NULL;
}

Entidades::~Entidades() {

}

Ogre::String Entidades::getEscenaNodeName() const {
	return mEscenaNodeName;
}

void Entidades::setEscenaNodeName(Ogre::String escenaNodeName) {
	mEscenaNodeName = escenaNodeName;
}

Ogre::String Entidades::getMesh() const {
	return mMesh;
}

void Entidades::setMesh(Ogre::String mesh) {
	mMesh = mesh;
}

Ogre::String Entidades::getName() const {
	return mName;
}

void Entidades::setName(Ogre::String name) {
	mName = name;
}

Ogre::Vector3 *Entidades::getPosition() const {
	return mPosition;
}

void Entidades::setPosition(Ogre::Vector3 *position) {
	mPosition = position;
}
