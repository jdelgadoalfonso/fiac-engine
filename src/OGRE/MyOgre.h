/*
 * myOgre.h
 *
 *  Created on: 27/05/2012
 *      Author: jose
 */

#ifndef MYOGRE_H_
#define MYOGRE_H_

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreConfigFile.h>

#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include "camera/CameraController.h"
#include "enviroment/Terrain.h"

class MyOgre : public Ogre::FrameListener,
			   public OIS::KeyListener,
			   public OIS::MouseListener,
			   public Ogre::WindowEventListener {
public:
	MyOgre(void);
	virtual ~MyOgre(void);
	bool createScene(void);

protected:
	Ogre::Root *mRoot;
	Ogre::Camera* mCamera;
	Ogre::CameraController* mCameraMan;
	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mWindow;
	Ogre::String mResourcesCfg;
	Ogre::String mPluginsCfg;
	Ogre::Light* mLight;
	Ogre::Entity* mPersonEntity;

	bool mExit;

	Ogre::Log *mLog;
	Ogre::String mTextureFilterOptions;

	//OgreBites::SdkTrayManager* mTrayMgr;
	//OgreBites::SdkCameraMan* mCameraMan;      // basic camera controller
	//OgreBites::ParamsPanel* mDetailsPanel;    // sample details panel
	bool mCursorWasVisible;                   // was cursor visible before dialog appeared
	bool mShutDown;

	// OIS Input devices
	OIS::InputManager* mInputManager;
	OIS::Mouse* mMouse;
	OIS::Keyboard* mKeyboard;

	// Frame Render Listener
	virtual bool frameRenderingQueued(const Ogre::FrameEvent&);

	// OIS::KeyListener
	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg );
	// OIS::MouseListener
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	// Ogre::WindowEventListener
	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);

private:
	bool initOGRE(void);
	bool initOIS(void);
};

#endif /* MYOGRE_H_ */
