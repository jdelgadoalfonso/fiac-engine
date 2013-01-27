/*
 * myOgre.cpp
 *
 *  Created on: 27/05/2012
 *      Author: jose
 */

#include "MyOgre.h"

MyOgre::MyOgre(void)
: mRoot(0),
  mCamera(0),
  mSceneMgr(0),
  mWindow(0),
  mResourcesCfg(Ogre::StringUtil::BLANK),
  mPluginsCfg(Ogre::StringUtil::BLANK),
  mTextureFilterOptions("N"),
  mCursorWasVisible(false),
  mShutDown(false),
  mInputManager(0),
  mMouse(0),
  mKeyboard(0) {

	mExit = (!initOGRE() || !initOIS());
}

MyOgre::~MyOgre(void) {
//	if (mTrayMgr) delete mTrayMgr;

	mRoot->destroySceneManager(mSceneMgr);

	//Remove ourself as a Window listener
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
	delete mRoot;
}

bool MyOgre::initOGRE(void) {
#ifdef _DEBUG
	mResourcesCfg = "conf/resources_d.cfg";
	mPluginsCfg = "conf/plugins_d.cfg";
#else
	mResourcesCfg = "conf/resources2.cfg";
	mPluginsCfg = "conf/plugins.cfg";
#endif

	// construct Ogre::Root
	mRoot = new Ogre::Root(mPluginsCfg);

	// Take the log of OGRE
	mLog = Ogre::LogManager::getSingleton().getDefaultLog();

	//---------------------------------------
	// setup resources
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load(mResourcesCfg);

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements()) {
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i) {
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
					archName, typeName, secName);
		}
	}
	//-------------------------------------------------------------------------
	// configure
	// Show the configuration dialog and initialise the system
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg
	if(mRoot->restoreConfig() || mRoot->showConfigDialog()) {
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window
		// by passing 'true'
		mWindow = mRoot->initialise(true, "myOgre Render Window");
	} else {
		return false;
	}
	return true;
}

bool MyOgre::initOIS(void) {

	//-------------------------------------------------------------------------
	//create FrameListener
	Ogre::LogManager::getSingletonPtr()->
			logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem( pl );

	mKeyboard = static_cast<OIS::Keyboard*>
	(mInputManager->createInputObject(OIS::OISKeyboard, true));
	mMouse = static_cast<OIS::Mouse*>
	(mInputManager->createInputObject(OIS::OISMouse, true));

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	//Set initial mouse clipping size
	windowResized(mWindow);

	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	return true;
}

bool MyOgre::createScene(void) {
	//-------------------------------------------------------------------------
	// choose scenemanager
	// Get the SceneManager, in this case a generic one
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
	//-------------------------------------------------------------------------
	// create camera
	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");

	// Position it at 500 in Z direction
	mCamera->setPosition(Ogre::Vector3(0,0,80));
	// Look back along -Z
	mCamera->lookAt(Ogre::Vector3(0,0,-300));
	mCamera->setNearClipDistance(5);

	// create a default camera controller
	mCameraMan = new Ogre::CameraController(mCamera);
	//-------------------------------------------------------------------------
	// create viewports
	// Create one viewport, entire window
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(
			Ogre::Real(vp->getActualWidth()) /
			Ogre::Real(vp->getActualHeight())
	);
	//-------------------------------------------------------------------------
	// Set default mipmap level (NB some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	//-------------------------------------------------------------------------
	// Create any resource listeners (for loading screens)
	//createResourceListener();
	//-------------------------------------------------------------------------
	// load resources
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	//-------------------------------------------------------------------------
	// Create the scene

	Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
	lightdir.normalise();

	mLight = mSceneMgr->createLight("tstLight");
	mLight->setType(Ogre::Light::LT_DIRECTIONAL);
	mLight->setDirection(lightdir);
	mLight->setDiffuseColour(Ogre::ColourValue::White);
	mLight->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));

	Terrain *terreno = new Terrain();
	terreno->configureTerrain(mLight, mSceneMgr);

	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

	mRoot->addFrameListener(this);
	//-------------------------------------------------------------------------
	mRoot->startRendering();

	return true;
}

bool MyOgre::frameRenderingQueued(const Ogre::FrameEvent& evt) {
	if (mWindow->isClosed()) {
		return false;
	}

	if (mShutDown) {
		return false;
	}

	mCameraMan->frameRenderingQueued(evt);

	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();

	return true;
}

bool MyOgre::keyPressed( const OIS::KeyEvent &arg ) {
	// cycle texture filtering mode
	if (arg.key == OIS::KC_T) {
		Ogre::String newVal;
		Ogre::TextureFilterOptions tfo;
		unsigned int aniso;

		switch (mTextureFilterOptions.at(0)) {
		case 'B':
			newVal = "Trilinear";
			tfo = Ogre::TFO_TRILINEAR;
			aniso = 1;
			break;
		case 'T':
			newVal = "Anisotropic";
			tfo = Ogre::TFO_ANISOTROPIC;
			aniso = 8;
			break;
		case 'N':
			newVal = "None";
			tfo = Ogre::TFO_NONE;
			aniso = 1;
			break;
		default:
			newVal = "Bilinear";
			tfo = Ogre::TFO_BILINEAR;
			aniso = 1;
			break;
		}

		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
		Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
		mTextureFilterOptions.assign(newVal);
	} else if (arg.key == OIS::KC_R) {  // cycle polygon rendering mode
		Ogre::String newVal;
		Ogre::PolygonMode pm;

		switch (mCamera->getPolygonMode()) {
		case Ogre::PM_SOLID:
			newVal = "Wireframe";
			pm = Ogre::PM_WIREFRAME;
			break;
		case Ogre::PM_WIREFRAME:
			newVal = "Points";
			pm = Ogre::PM_POINTS;
			break;
		default:
			newVal = "Solid";
			pm = Ogre::PM_SOLID;
			break;
		}

		mCamera->setPolygonMode(pm);
	} else if(arg.key == OIS::KC_F5) {   // refresh all textures
		Ogre::TextureManager::getSingleton().reloadAll();
	} else if (arg.key == OIS::KC_SYSRQ) {  // take a screenshot
		mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
	} else if (arg.key == OIS::KC_ESCAPE) {
		mShutDown = true;
	}

	mCameraMan->injectKeyDown(arg);
	return true;
}

bool MyOgre::keyReleased( const OIS::KeyEvent &arg ) {
	mCameraMan->injectKeyUp(arg);
	return true;
}

bool MyOgre::mouseMoved( const OIS::MouseEvent &arg ) {
	mCameraMan->injectMouseMove(arg);
	return true;
}

bool MyOgre::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) {
	mCameraMan->injectMouseDown(arg, id);
	return true;
}

bool MyOgre::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) {
	mCameraMan->injectMouseUp(arg, id);
	return true;
}

// Adjust mouse clipping area
void MyOgre::windowResized(Ogre::RenderWindow* rw) {
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

// Unattach OIS before window shutdown (very important under Linux)
void MyOgre::windowClosed(Ogre::RenderWindow* rw) {
	// Only close for window that created OIS (the main window in these demos)
	if( rw == mWindow && mInputManager) {
		mInputManager->destroyInputObject( mMouse );
		mInputManager->destroyInputObject( mKeyboard );

		OIS::InputManager::destroyInputSystem(mInputManager);
		mInputManager = 0;
	}
}
