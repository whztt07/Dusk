/*
 -----------------------------------------------------------------------------
    This file is part of the Dusk Editor.
    Copyright (C) 2009, 2010 thoronador

    The Dusk Editor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The Dusk Editor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the Dusk Editor.  If not, see <http://www.gnu.org/licenses/>.
 -----------------------------------------------------------------------------
*/

#include "EditorFrameListener.h"
#include "EditorCamera.h"
#include "../Engine/Landscape.h"

namespace Dusk
{


CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
    switch (buttonID)
    {
    case OIS::MB_Left:
        return CEGUI::LeftButton;

    case OIS::MB_Right:
        return CEGUI::RightButton;

    case OIS::MB_Middle:
        return CEGUI::MiddleButton;

    default:
        return CEGUI::LeftButton;
    }
}


bool EditorFrameListener::frameStarted(const Ogre::FrameEvent &evt)
{
  mKeyboard->capture();
  mMouse->capture();

  return mContinue && !mKeyboard->isKeyDown(OIS::KC_ESCAPE);
}

// MouseListener
bool EditorFrameListener::mouseMoved(const OIS::MouseEvent &arg)
{
  if ( m_Mode == EM_Movement && arg.state.buttonDown(OIS::MB_Right))
  {
    EditorCamera::getSingleton().relativeRotation( arg.state.X.rel * 180.0f/800.0f);
    return true;
  }
  CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
  return true;
}

bool EditorFrameListener::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));
  return true;
}

bool EditorFrameListener::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
  return true;
}

// KeyListener
bool EditorFrameListener::keyPressed(const OIS::KeyEvent &arg)
{
  if (m_Mode == EM_Movement)
  {
    switch(arg.key)
    {
      case OIS::KC_W:
      case OIS::KC_UP:
           EditorCamera::getSingleton().setTranslationVector(25*Ogre::Vector3::NEGATIVE_UNIT_Z);
           return true;
           break;
      case OIS::KC_S:
      case OIS::KC_DOWN:
           EditorCamera::getSingleton().setTranslationVector(25*Ogre::Vector3::UNIT_Z);
           return true;
           break;
      case OIS::KC_A:
      case OIS::KC_LEFT:
           EditorCamera::getSingleton().setTranslationVector(25*Ogre::Vector3::NEGATIVE_UNIT_X);
           return true;
           break;
      case OIS::KC_D:
      case OIS::KC_RIGHT:
           EditorCamera::getSingleton().setTranslationVector(25*Ogre::Vector3::UNIT_X);
           return true;
           break;
      case OIS::KC_Q:
           EditorCamera::getSingleton().setRotationSpeed(60.0f);
           return true;
           break;
      case OIS::KC_E:
           EditorCamera::getSingleton().setRotationSpeed(-60.0f);
           return true;
           break;
      case OIS::KC_LSHIFT:
           EditorCamera::getSingleton().setTurboMode(true);
           return true;
           break;
      default: break;
    }//swi
  }//if

  CEGUI::System *sys = CEGUI::System::getSingletonPtr();
  sys->injectKeyDown(arg.key);
  sys->injectChar(arg.text);
  return true;
}

bool EditorFrameListener::keyReleased(const OIS::KeyEvent &arg)
{
  if (m_Mode == EM_Movement)
  {
    switch(arg.key)
    {
      case OIS::KC_W:
      case OIS::KC_UP:
      case OIS::KC_S:
      case OIS::KC_DOWN:
      case OIS::KC_A:
      case OIS::KC_LEFT:
      case OIS::KC_D:
      case OIS::KC_RIGHT:
           EditorCamera::getSingleton().setTranslationVector(Ogre::Vector3::ZERO);
           return true;
           break;
      case OIS::KC_Q:
      case OIS::KC_E:
           EditorCamera::getSingleton().setRotationSpeed(0.0f);
           return true;
           break;
      case OIS::KC_LSHIFT:
           EditorCamera::getSingleton().setTurboMode(false);
           return true;
           break;
      default: break;
    }//swi
  }//if

  CEGUI::System::getSingleton().injectKeyUp(arg.key);
  return true;
}

bool EditorFrameListener::quit(const CEGUI::EventArgs &e)
{
  mContinue = false;
  return true;
}

//Adjust mouse clipping area
void EditorFrameListener::windowResized(Ogre::RenderWindow* rw)
{
  unsigned int width, height, depth;
  int left, top;
  rw->getMetrics(width, height, depth, left, top);

  const OIS::MouseState &ms = mMouse->getMouseState();
  ms.width = width;
  ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void EditorFrameListener::windowClosed(Ogre::RenderWindow* rw)
{
  //Only close for window that created OIS (the main window in these demos)
  if( rw == mWindow )
  {
    if( mInputManager )
    {
      mInputManager->destroyInputObject( mMouse );
      mInputManager->destroyInputObject( mKeyboard );
      mInputManager->destroyInputObject( mJoy );

      OIS::InputManager::destroyInputSystem(mInputManager);
      mInputManager = 0;
    }
  }
}

EditorFrameListener::~EditorFrameListener()
{
  //Remove ourself as a Window listener
  Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
  windowClosed(mWindow);
}

bool EditorFrameListener::processUnbufferedKeyInput(const Ogre::FrameEvent& evt)
{

  if(mKeyboard->isKeyDown(OIS::KC_A))
    mTranslateVector.x = -mMoveScale;	// Move camera left

  if(mKeyboard->isKeyDown(OIS::KC_D))
    mTranslateVector.x = mMoveScale;	// Move camera RIGHT

  if(mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W) )
    mTranslateVector.z = -mMoveScale;	// Move camera forward

  if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) )
    mTranslateVector.z = mMoveScale;	// Move camera backward

  if(mKeyboard->isKeyDown(OIS::KC_PGUP))
    mTranslateVector.y = mMoveScale;	// Move camera up

  if(mKeyboard->isKeyDown(OIS::KC_PGDOWN))
    mTranslateVector.y = -mMoveScale;	// Move camera down

  if(mKeyboard->isKeyDown(OIS::KC_RIGHT))
    mCamera->yaw(-mRotScale);

  if(mKeyboard->isKeyDown(OIS::KC_LEFT))
    mCamera->yaw(mRotScale);

  if( mKeyboard->isKeyDown(OIS::KC_ESCAPE) || mKeyboard->isKeyDown(OIS::KC_Q) )
    return false;

  if( mKeyboard->isKeyDown(OIS::KC_F) && mTimeUntilNextToggle <= 0 )
  {
	mStatsOn = !mStatsOn;
	showDebugOverlay(mStatsOn);
    mTimeUntilNextToggle = 1;
  }

  if( mKeyboard->isKeyDown(OIS::KC_T) && mTimeUntilNextToggle <= 0 )
  {
	switch(mFiltering)
	{
	  case Ogre::TFO_BILINEAR:
			mFiltering = Ogre::TFO_TRILINEAR;
			mAniso = 1;
			break;
      case Ogre::TFO_TRILINEAR:
            mFiltering = Ogre::TFO_ANISOTROPIC;
			mAniso = 8;
			break;
      case Ogre::TFO_ANISOTROPIC:
            mFiltering = Ogre::TFO_BILINEAR;
			mAniso = 1;
			break;
      default: break;
	}
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(mFiltering);
	Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);

	showDebugOverlay(mStatsOn);
    mTimeUntilNextToggle = 1;
  }

  if(mKeyboard->isKeyDown(OIS::KC_SYSRQ) && mTimeUntilNextToggle <= 0)
  {
	std::ostringstream ss;
	ss << "screenshot_" << ++mNumScreenShots << ".png";
	mWindow->writeContentsToFile(ss.str());
	mTimeUntilNextToggle = 0.5;
    mDebugText = "Saved: " + ss.str();
  }

  if(mKeyboard->isKeyDown(OIS::KC_R) && mTimeUntilNextToggle <=0)
  {
	mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
	switch(mSceneDetailIndex) {
	  case 0 : mCamera->setPolygonMode(Ogre::PM_SOLID); break;
      case 1 : mCamera->setPolygonMode(Ogre::PM_WIREFRAME); break;
      case 2 : mCamera->setPolygonMode(Ogre::PM_POINTS); break;
	}
	mTimeUntilNextToggle = 0.5;
  }

  static bool displayCameraDetails = false;
  if(mKeyboard->isKeyDown(OIS::KC_P) && mTimeUntilNextToggle <= 0)
  {
	displayCameraDetails = !displayCameraDetails;
	mTimeUntilNextToggle = 0.5;
	if (!displayCameraDetails)
	  mDebugText = "";
  }

  // Print camera details
  if(displayCameraDetails)
    mDebugText = "P: " + Ogre::StringConverter::toString(mCamera->getDerivedPosition()) +
                 " " + "O: " + Ogre::StringConverter::toString(mCamera->getDerivedOrientation());

  // Return true to continue rendering
  return true;
}

bool EditorFrameListener::processUnbufferedMouseInput(const Ogre::FrameEvent& evt)
{
  // Rotation factors, may not be used if the second mouse button is pressed
  // 2nd mouse button - slide, otherwise rotate
  const OIS::MouseState &ms = mMouse->getMouseState();
  if( ms.buttonDown( OIS::MB_Right ) )
  {
    mTranslateVector.x += ms.X.rel * 0.13;
	mTranslateVector.y -= ms.Y.rel * 0.13;
  }
  else
  {
	mRotX = Ogre::Degree(-ms.X.rel * 0.13);
	mRotY = Ogre::Degree(-ms.Y.rel * 0.13);
  }

  return true;
}

void EditorFrameListener::moveCamera()
{
  // Make all the changes to the camera
  // Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW
  //(e.g. airplane)
  mCamera->yaw(mRotX);
  mCamera->pitch(mRotY);
  mCamera->moveRelative(mTranslateVector);
}

void EditorFrameListener::showDebugOverlay(bool show)
{
  if (mDebugOverlay)
  {
	if (show)
      mDebugOverlay->show();
	else
      mDebugOverlay->hide();
  }
}

bool EditorFrameListener::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
  if(mWindow->isClosed())	return false;

  mSpeedLimit = mMoveScale * evt.timeSinceLastFrame;

  //Need to capture/update each device
  mKeyboard->capture();
  mMouse->capture();
  if( mJoy ) mJoy->capture();

  bool buffJ = (mJoy) ? mJoy->buffered() : true;

  Ogre::Vector3 lastMotion = mTranslateVector;

  //Check if one of the devices is not buffered
  if( !mMouse->buffered() || !mKeyboard->buffered() || !buffJ )
  {
    // one of the input modes is immediate, so setup what is needed for immediate movement
	if (mTimeUntilNextToggle >= 0)
		mTimeUntilNextToggle -= evt.timeSinceLastFrame;

	// Move about 100 units per second
	mMoveScale = mMoveSpeed * evt.timeSinceLastFrame;
	// Take about 10 seconds for full rotation
	mRotScale = mRotateSpeed * evt.timeSinceLastFrame;

	mRotX = 0;
	mRotY = 0;
	mTranslateVector = Ogre::Vector3::ZERO;
  }

  //Check to see which device is not buffered, and handle it
  if( !mKeyboard->buffered() )
    if( processUnbufferedKeyInput(evt) == false )
      return false;
  if( !mMouse->buffered() )
    if( processUnbufferedMouseInput(evt) == false )
      return false;

  // ramp up / ramp down speed
  if (mTranslateVector == Ogre::Vector3::ZERO)
  {
    // decay (one third speed)
    mCurrentSpeed -= evt.timeSinceLastFrame * 0.3;
    mTranslateVector = lastMotion;
  }
  else
  {
    // ramp up
    mCurrentSpeed += evt.timeSinceLastFrame;
  }
  // Limit motion speed
  if (mCurrentSpeed > 1.0)
    mCurrentSpeed = 1.0;
  if (mCurrentSpeed < 0.0)
    mCurrentSpeed = 0.0;

  mTranslateVector *= mCurrentSpeed;

  if( !mMouse->buffered() || !mKeyboard->buffered() || !buffJ )
    moveCamera();

  return true;
}

bool EditorFrameListener::frameEnded(const Ogre::FrameEvent& evt)
{
  //move camera
  EditorCamera::getSingleton().processMovement(evt.timeSinceLastFrame);
  //check for landscape updates
  if (Landscape::getSingleton().needsUpdate())
  {
    std::cout << "Info: "<<Landscape::getSingleton().updateRecords(m_WireFrameLandscape)<< " landscape record(s) updated.\n";
  }
  return true;
}

void EditorFrameListener::setEditorMode(const EditorMode em)
{
  m_Mode = em;
  if (CEGUI::WindowManager::getSingleton().isWindowPresent("Editor/ModeIndicator"))
  {
    switch (m_Mode)
    {
        case EM_LandscapeUp:
             CEGUI::WindowManager::getSingleton().getWindow("Editor/ModeIndicator")->setText("Mode: Landscape (Up)");
             break;
        case EM_LandscapeDown:
             CEGUI::WindowManager::getSingleton().getWindow("Editor/ModeIndicator")->setText("Mode: Landscape (Down)");
             break;
        case EM_LandscapeColour:
             CEGUI::WindowManager::getSingleton().getWindow("Editor/ModeIndicator")->setText("Mode: Landscape (Colour)");
             break;
        case EM_Movement:
             CEGUI::WindowManager::getSingleton().getWindow("Editor/ModeIndicator")->setText("Mode: Movement");
             break;
        case EM_Lists:
             CEGUI::WindowManager::getSingleton().getWindow("Editor/ModeIndicator")->setText("Mode: Catalogue");
             break;
        default: //should never happen
             CEGUI::WindowManager::getSingleton().getWindow("Editor/ModeIndicator")->setText("Mode: unknown");
             break;
    }//switch
  }//if
}

EditorMode EditorFrameListener::getEditorMode(void)
{
  return m_Mode;
}

void EditorFrameListener::setWireFrame(const bool wf)
{
  if (wf!=m_WireFrameLandscape)
  {
    m_WireFrameLandscape = wf;
    Landscape::getSingleton().removeFromEngine(getAPI().getOgreSceneManager());
    Landscape::getSingleton().sendToEngine(getAPI().getOgreSceneManager(), wf);
  }
}

bool EditorFrameListener::IsKeyDown(const OIS::KeyCode kc)
{
  if (mKeyboard!=NULL)
  {
    return mKeyboard->isKeyDown(kc);
  }
  return false;
}

bool EditorFrameListener::IsMouseDown(const OIS::MouseButtonID mb)
{
  if (mMouse!=NULL)
  {
    return mMouse->getMouseState().buttonDown(mb);
  }
  return false;
}

}//namespace
