/*
 * Entidades.h
 *
 *  Created on: 22 Jul 2012
 *      Author: jose
 */

#ifndef ENTIDADES_H_
#define ENTIDADES_H_

#include <stddef.h>
#include <OGRE/OgreRoot.h>

class Entidades {
public:
	Entidades();
	virtual ~Entidades();

	Ogre::String getEscenaNodeName() const;
	void setEscenaNodeName(Ogre::String);
	Ogre::String getMesh() const;
	void setMesh(Ogre::String);
	Ogre::String getName() const;
	void setName(Ogre::String);
	Ogre::Vector3 *getPosition() const;
	void setPosition(Ogre::Vector3*);

private:
	Ogre::String mMesh;
	Ogre::String mName;
	Ogre::String mEscenaNodeName;
	Ogre::Vector3 *mPosition;
};

#endif /* ENTIDADES_H_ */
