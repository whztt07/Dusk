/*
 -----------------------------------------------------------------------------
    This file is part of the Dusk Engine.
    Copyright (C) 2008, 2009, 2010 walljumper, thoronador

    The Dusk Engine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The Dusk Engine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the Dusk Engine.  If not, see <http://www.gnu.org/licenses/>.
 -----------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
 Authors: walljumper, thoronador
 Date:    2010-05-06
 Purpose: Camera Singleton class
          Manages the game's camera.

 History:
     - 2008-02-08 (rev 34)  - initial version (by walljumper)
     - 2008-03-22 (rev 51)  - movement/ translation functions added,
                            - frame listener calls move()
     - 2008-03-25 (rev 55)  - move() and translate() actually implemented
     - 2009-06-21 (rev 98)  - rotate() added, camera rotation implemented
                              (by thoronador)
     - 2009-06-30 (rev 100) - camera adjusts its position to move along
                              landscape, if landscape data is present
     - 2009-12-04 (rev 138) - zoom functionality added
     - 2009-12-31 (rev 147) - documentation update
     - 2010-03-24 (rev 186) - singleton pattern; jump() added
     - 2010-03-27 (rev 188) - adjustments for Player class
     - 2010-05-06 (rev 197) - setupDone() added
     - 2010-05-21 (rev 206) - adjustments for player movement
     - 2010-05-27 (rev 208) - adjustments for player's animations
     - 2010-08-16 (rev 229) - fix in destructor to prevent possible segfault
     - 2010-08-27 (rev 236) - cAboveGroundLevel made public
                            - jump() and translate()removed, because Player class
                              will set the camera position instead
     - 2010-08-28 (rev 237) - rotateOnceX() and rotateOnceY() added

 ToDo list:
     - adjust value of const. gravity in move() and initial value of
       m_JumpVelocity in jump() to get more realistic jump height and
       duration (i.e. "realistic" as in "looks real")
     - ???
 Bugs:
     - No known bugs. If you find one (or more), then tell us please.
 --------------------------------------------------------------------------*/

#ifndef CAMERA_H
#define CAMERA_H

#include <Ogre.h>

namespace Dusk
{
    class Camera
    {
        public:
            /* singleton access method */
            static Camera& getSingleton();

            /* destructor */
            virtual ~Camera();

            /* initializes Camera for use by application

               remarks:
                   Has to be called once before the Camera is used.

               parameters:
                   scn - pointer to Ogre SceneManager which shall be used to
                         create camera and camera nodes. This pointer MUST NOT
                         be NULL, or the function will throw exceptions.
             */
            void setupCamera(Ogre::SceneManager* scn);

            /* returns true, if the camera has been set up, usually by calling
               the setupCamera() method
            */
            bool setupDone() const;

            /* retrieves the pointer to the Ogre::Camera object which is
               internally used*/
            Ogre::Camera* getOgreCamera() const;

            /* sets the position of the camera

               remarks:
                   The vector does not neccessarily represent the actual
                   position of the camera object but only the position of the
                   camera before zoom is applied.
            */
            void setPosition(const Ogre::Vector3& position);

            /* Tells the camera where to look at. */
            void lookAt(const Ogre::Vector3& direction);

            /* Performs movement of the camera, i.e. translation and rotation,
               based on the time given in evt.timeSinceLastFrame. */
            void move(const Ogre::FrameEvent& evt);

            /* Adds(!) to the internally used rotation value.

               remarks:
                   The value is interpreted as degrees, not as radians.
            */
            void rotate(const float rotation);

            /* rotates the camera left/right immediately

                parameters:
                   delta - amount of degrees the camera will be turned up/down
            */
            void rotateOnceX(const float delta);

            /* rotates the camera up/down immediately

               parameters:
                   delta - amount of degrees the camera will be turned up/down

               remarks:
                   If the camera's total rotation exceeds 90� in either
                   directions, the rotation will be reset to 90� or -90�
                   respectively.
            */
            void rotateOnceY(const float delta);

            /* Sets the zoom distance.

               remarks:
                   The distance is interpreted in world units behind the camera
                   position specified with setPosition(), i.e. greater values
                   will get the viewpoint further away from the camera position,
                   while a value of zero means you are exactly at the camera
                   position.
                   Note that the distance will be capped if outside of the range
                   [cMinimumZoom,cMaximumZoom].
            */
            void setZoom(const float distance);

            /* Returns the current zoom distance. */
            float getZoom() const;

            /* constants to limit zoom range in setZoom() */
            static const float cMinimumZoom;
            static const float cMaximumZoom;

            /* constant to indicate change in zoom for one keystroke/ one turn
               of mouse wheel (recommended)*/
            static const float cRecommendedZoomStep;

            /* default distance between camera and ground/ landscape */
            static const float cAboveGroundLevel;
        private:
            /* constructor

               parameters:
                   scn - pointer to an Ogre SceneManager
                         If this pointer is not NULL, the given SceneManager
                         is used to set up the camera right after creation.
                         Also see setupCamera().
            */
            Camera(Ogre::SceneManager* scn=NULL);

            /* copy constructor - empty, because it's a singleton class */
            Camera(const Camera& op) {}
            Ogre::Camera* m_Camera;
            Ogre::SceneNode* m_Primary;
            Ogre::SceneNode* m_Secondary;
            float m_RotationPerSecond;
    };
}

#endif // CAMERA_H
