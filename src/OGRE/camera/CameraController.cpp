/*
 * CameraController.cpp
 *
 *  Created on: 26 Jan 2013
 *      Author: jose
 */

#include "CameraController.h"

namespace Ogre {

CameraController::CameraController(Ogre::Camera* cam)
: mCamera(0)
, mTarget(0)
, mOrbiting(false)
, mZooming(false)
, mTopSpeed(150)
, mVelocity(Ogre::Vector3::ZERO)
, mGoingForward(false)
, mGoingBack(false)
, mGoingLeft(false)
, mGoingRight(false)
, mGoingUp(false)
, mGoingDown(false)
, mFastMove(false) {

	setCamera(cam);
	setStyle(CS_FREELOOK);
}

CameraController::~CameraController() {

}

void CameraController::setCamera(Ogre::Camera* cam) {
	mCamera = cam;
}

Ogre::Camera* CameraController::getCamera() {
	return mCamera;
}

void CameraController::setTarget(Ogre::SceneNode* target) {
	if (mStyle == CS_ORBIT) {
		mTarget = target ? target : mCamera->getSceneManager()->getRootSceneNode();
		setYawPitchDist(Ogre::Degree(0), Ogre::Degree(15), 150);
		mCamera->setAutoTracking(true, mTarget);
	}
}

Ogre::SceneNode* CameraController::getTarget() {
	return mTarget;
}

void CameraController::setYawPitchDist(
		Ogre::Radian yaw, Ogre::Radian pitch, Ogre::Real dist) {
	if (mStyle == CS_ORBIT) {
		mCamera->setPosition(mTarget->_getDerivedPosition());
		mCamera->setOrientation(mTarget->_getDerivedOrientation());
		mCamera->yaw(yaw);
		mCamera->pitch(-pitch);
		mCamera->moveRelative(Ogre::Vector3(0, 0, dist));
	}
}

void CameraController::setTopSpeed(Ogre::Real topSpeed) {
	mTopSpeed = topSpeed;
}

Ogre::Real CameraController::getTopSpeed() {
	return mTopSpeed;
}

void CameraController::setStyle(CameraStyle style) {
	if (mStyle != CS_ORBIT && style == CS_ORBIT) {
		mStyle = CS_ORBIT;
		setTarget(mTarget);
		mCamera->setFixedYawAxis(true);
	} else if (mStyle != CS_FREELOOK && style == CS_FREELOOK) {
		mStyle = CS_FREELOOK;
		mCamera->setAutoTracking(false);
		mCamera->setFixedYawAxis(true);
	} else if (mStyle != CS_MANUAL && style == CS_MANUAL) {
		mStyle = CS_MANUAL;
		mCamera->setAutoTracking(false);
		mCamera->setFixedYawAxis(true);
	}
}

CameraStyle CameraController::getStyle() {
	return mStyle;
}

void CameraController::manualStop()
{
	if (mStyle == CS_FREELOOK)
	{
		mGoingForward = false;
		mGoingBack = false;
		mGoingLeft = false;
		mGoingRight = false;
		mGoingUp = false;
		mGoingDown = false;
		mVelocity = Ogre::Vector3::ZERO;
	}
}

bool CameraController::frameRenderingQueued(const Ogre::FrameEvent& evt) {
	if (mStyle == CS_FREELOOK) {
		// build our acceleration vector based on keyboard input composite
		Ogre::Vector3 accel = Ogre::Vector3::ZERO;
		if (mGoingForward) accel += mCamera->getDirection();
		if (mGoingBack) accel -= mCamera->getDirection();
		if (mGoingRight) accel += mCamera->getRight();
		if (mGoingLeft) accel -= mCamera->getRight();
		if (mGoingUp) accel += mCamera->getUp();
		if (mGoingDown) accel -= mCamera->getUp();

		// if accelerating, try to reach top speed in a certain time
		Ogre::Real topSpeed = mFastMove ? mTopSpeed * 20 : mTopSpeed;
		if (accel.squaredLength() != 0) {
			accel.normalise();
			mVelocity += accel * topSpeed * evt.timeSinceLastFrame * 10;
		}
		// if not accelerating, try to stop in a certain time
		else mVelocity -= mVelocity * evt.timeSinceLastFrame * 10;

		// keep camera velocity below top speed and above zero
		if (mVelocity.squaredLength() > topSpeed * topSpeed) {
			mVelocity.normalise();
			mVelocity *= topSpeed;
		}
		else if (mVelocity.squaredLength() < 0.1) mVelocity = Ogre::Vector3::ZERO;

		if (mVelocity != Ogre::Vector3::ZERO) mCamera->move(mVelocity * evt.timeSinceLastFrame);
	}

	return true;
}

void CameraController::injectKeyDown(const OIS::KeyEvent& evt) {
	if (mStyle == CS_FREELOOK) {
		if (evt.key == OIS::KC_W || evt.key == OIS::KC_UP) mGoingForward = true;
		else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN) mGoingBack = true;
		else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT) mGoingLeft = true;
		else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT) mGoingRight = true;
		else if (evt.key == OIS::KC_PGUP) mGoingUp = true;
		else if (evt.key == OIS::KC_PGDOWN) mGoingDown = true;
		else if (evt.key == OIS::KC_LSHIFT) mFastMove = true;
	}
}

void CameraController::injectKeyUp(const OIS::KeyEvent& evt) {
	if (mStyle == CS_FREELOOK) {
		if (evt.key == OIS::KC_W || evt.key == OIS::KC_UP) mGoingForward = false;
		else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN) mGoingBack = false;
		else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT) mGoingLeft = false;
		else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT) mGoingRight = false;
		else if (evt.key == OIS::KC_PGUP) mGoingUp = false;
		else if (evt.key == OIS::KC_PGDOWN) mGoingDown = false;
		else if (evt.key == OIS::KC_LSHIFT) mFastMove = false;
	}
}

void CameraController::injectMouseMove(const OIS::MouseEvent& evt) {
	if (mStyle == CS_ORBIT) {
		Ogre::Real dist = (mCamera->getPosition() - mTarget->_getDerivedPosition()).length();

		if (mOrbiting) {   // yaw around the target, and pitch locally
			mCamera->setPosition(mTarget->_getDerivedPosition());

			mCamera->yaw(Ogre::Degree(-evt.state.X.rel * 0.25f));
			mCamera->pitch(Ogre::Degree(-evt.state.Y.rel * 0.25f));

			mCamera->moveRelative(Ogre::Vector3(0, 0, dist));

			// don't let the camera go over the top or around the bottom of the target
		} else if (mZooming) {  // move the camera toward or away from the target
			// the further the camera is, the faster it moves
			mCamera->moveRelative(Ogre::Vector3(0, 0, evt.state.Y.rel * 0.004f * dist));
		} else if (evt.state.Z.rel != 0) { // move the camera toward or away from the target

			// the further the camera is, the faster it moves
			mCamera->moveRelative(Ogre::Vector3(0, 0, -evt.state.Z.rel * 0.0008f * dist));
		}
	} else if (mStyle == CS_FREELOOK) {
		mCamera->yaw(Ogre::Degree(-evt.state.X.rel * 0.15f));
		mCamera->pitch(Ogre::Degree(-evt.state.Y.rel * 0.15f));
	}
}

void CameraController::injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id) {
	if (mStyle == CS_ORBIT) {
		if (id == OIS::MB_Left) mOrbiting = true;
		else if (id == OIS::MB_Right) mZooming = true;
	}
}

void CameraController::injectMouseUp(const OIS::MouseEvent& evt, OIS::MouseButtonID id) {
	if (mStyle == CS_ORBIT) {
		if (id == OIS::MB_Left) mOrbiting = false;
		else if (id == OIS::MB_Right) mZooming = false;
	}
}

}
