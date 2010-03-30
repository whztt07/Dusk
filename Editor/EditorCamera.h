#ifndef EDITORCAMERA_H
#define EDITORCAMERA_H

#include <OgreVector3.h>
#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreFrameListener.h>

namespace Dusk
{

  class EditorCamera
  {
    public:
      virtual ~EditorCamera();
      static EditorCamera& GetSingleton();
      void setupCamera(Ogre::SceneManager* scm);
      Ogre::Camera* getOgreCamera();
      void setPosition(const Ogre::Vector3& position);
      Ogre::Vector3 getPosition(void);
      Ogre::Quaternion getOrientation(void);
      void setZoomDistance(const float distance);
      float getZoomDistance(void);
      void resetToOrigin(void);
      void setRotationSpeed(const float degrees);
      float getRotationSpeed(void);
      void relativeRotation(const float degrees);
      void setTranslationVector(const Ogre::Vector3& velocity);
      Ogre::Vector3 getTranslationVector(void);
      void setTurboMode(const bool value);

      void processMovement(const Ogre::FrameEvent& evt);
    private:
      EditorCamera();
      EditorCamera(const EditorCamera& op){}
      Ogre::Camera* cam;
      Ogre::SceneNode* m_primaryCameraNode;
      Ogre::SceneNode* m_secondaryCameraNode;
      Ogre::Vector3 translation_per_second;
      bool m_Turbo;
      float rotation_per_second;
  }; //class

}//namespace

#endif // EDITORCAMERA_H
