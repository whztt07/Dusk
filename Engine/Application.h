/*
 -----------------------------------------------------------------------------
    This file is part of the Dusk Engine.
    Copyright (C) 2007, 2008, 2009, 2010, 2011 ssj5000, walljumper, DaSteph, thoronador

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
 Authors: ssj5000, DaSteph, walljumper, thoronador
 Date:    2007-11-19
 Purpose: Application class
          Responsible for building up the engine

 History:
     - 2007-11-19 (rev 3)   - initial version (by ssj5000)
     - 2007-11-27 (rev 8)   - initialise() and go() extended (by DaSteph)
     - 2007-11-28 (rev 9)   - frame listener added (by walljumper)
     - 2007-11-28 (rev 10)  - }
     - 2007-11-29 (rev 11)  - } frame listener construction improved
     - 2007-12-29 (rev 14)  - }
     - 2007-12-29 (rev 15)  - changed Input from FrameListener to InputSystem (by DaSteph)
     - 2007-12-30 (rev 17)  - getFrameListener() and API added (by walljumper)
     - 2008-01-17 (rev 23)  - reference to API removed, it's now singleton
     - 2008-01-17 (rev 24)  - dispatcher used/included
     - 2008-01-24 (rev 26)  - }
     - 2008-01-24 (rev 27)  - } small changes
     - 2008-01-26 (rev 28)  - }
     - 2008-01-26 (rev 29)  - createScene() removed (it's now handled by Scene class)
     - 2008-01-30 (rev 33)  - camera is now created by Application
     - 2008-02-22 (rev 38)  - } sound system is
     - 2008-02-23 (rev 40)  - } initialized by Application
     - 2009-02-13 (rev 80)  - small change (by thoronador)
     - 2010-01-07 (rev 150) - notification about status of sound system added
     - 2010-03-12 (rev 181) - small adjustments
     - 2010-03-24 (rev 186) - adjustments for camera, which is now singleton
     - 2010-08-03 (rev 221) - minor optimization, documentation updated
     - 2010-11-09 (rev 249) - createScreenshot() added
     - 2010-11-10 (rev 250) - minor optimization
     - 2010-11-12 (rev 252) - screenshot can now be JPEG or PNG
     - 2010-11-12 (rev 253) - minor fix to properly display fog in screenshots
     - 2010-12-01 (rev 265) - use DuskLog/Messages class for logging
     - 2011-08-05 (rev 295) - info about sound device names added
     - 2011-08-05 (rev 296) - info about default sound device name added

 ToDo list:
     - ???

 Bugs:
     - No known bugs. If you find one (or more), then tell me please.
 --------------------------------------------------------------------------*/

#ifndef APPLICATION_H
#define APPLICATION_H
#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include "FrameListener.h"
#include <string>
namespace Dusk
{
  class API;

  class Application
  {
    public:
      Application();
      virtual ~Application();
      virtual void go(const std::string& pluginFileName);
      Dusk::FrameListener* getFrameListener();

      /* creates a screenshot of the render window and saves it as PNG to the
         current directory

         return value:
           Returns false, if an error occured.
      */
      bool createScreenshot() const;

    protected:
      virtual bool initialise(const std::string& pluginFileName);
      virtual bool initOgreCore();

      virtual void createSceneManager();
      virtual void createCamera();
      virtual void createViewports();
      virtual void createResourceListener();
      virtual void createFrameListener();

      virtual void addResourceLocations();
      virtual void initResources();

      virtual void destroyScene();

      Ogre::Root* m_Root;
      Ogre::Camera* m_Camera;
      Ogre::SceneManager* m_SceneManager;
      Ogre::RenderWindow* m_Window;
      Dusk::FrameListener* m_FrameListener;

      //enumeration value for screenshot format
      enum ScreenshotFormat { ssfJPEG, ssfPNG };

      ScreenshotFormat m_ScreenshotType;
  };//class Application

}//namespace

#endif // APPLICATION_H
