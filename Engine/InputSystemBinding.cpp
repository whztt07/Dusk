#include "InputSystemBinding.h"

#include "Console.h"
#include "API.h"

namespace Dusk{

InputSystemBinding::InputSystemBinding()
{
    myBindListPress[OIS::KC_ESCAPE] = Script("quit");
    myBindListPress[OIS::KC_W] = Script("move_forward");
    myBindListPress[OIS::KC_S] = Script("move_backward");
    myBindListPress[OIS::KC_A] = Script("step_left");
    myBindListPress[OIS::KC_D] = Script("step_right");
    myBindListPress[OIS::KC_Q] = Script("turn_left");
    myBindListPress[OIS::KC_E] = Script("turn_right");
    //myBindListPress[OIS::KC_SPACE] = Script("jump");

    myBindListRelease[OIS::KC_W] = Script("move_backward");
    myBindListRelease[OIS::KC_S] = Script("move_forward");
    myBindListRelease[OIS::KC_A] = Script("step_right");
    myBindListRelease[OIS::KC_D] = Script("step_left");
    myBindListRelease[OIS::KC_Q] = Script("turn_right");
    myBindListRelease[OIS::KC_E] = Script("turn_left");
}

InputSystemBinding::~InputSystemBinding()
{

}

bool InputSystemBinding::keyPressed (const OIS::KeyEvent &arg)
{
    if (arg.key == OIS::KC_GRAVE)
        InputSystem::toggleInput();
    else
    {
        //is it a PressBinding?
        std::map<OIS::KeyCode, Script>::iterator it = myBindListPress.find(arg.key);
        if (it != myBindListPress.end())
            Console::getInstance()->addScript(it->second);
        // is it a hold binding?
        it = myBindListHold.find(arg.key);
        if (it != myBindListHold.end())
            Console::getInstance()->addScript(it->second.getStartScript());
    }

    //temporary bindings to toggle rain, fog and snow
    switch (arg.key)
    {
      case OIS::KC_R: // R like rain
           Console::getInstance()->addScript(Script("toggle_rain"));
           break;
      case OIS::KC_F:
           Console::getInstance()->addScript(Script("toggle_fog"));
           break;
      case OIS::KC_V: // S like snow is already occupied for movement
           Console::getInstance()->addScript(Script("toggle_snow"));
           break;
      default: break;
    }//swi

    return true;
}

bool InputSystemBinding::keyReleased (const OIS::KeyEvent &arg)
{
    //is it a ReleaseBinding?
    std::map<OIS::KeyCode, Script>::iterator it = myBindListRelease.find(arg.key);
    if (it != myBindListRelease.end())
        Console::getInstance()->addScript(it->second);
    // is it a HoldBindinginding?
    it = myBindListHold.find(arg.key);
    if (it != myBindListHold.end())
        Console::getInstance()->addScript(it->second.getStopScript());

    return true;
}
InputSystemBinding& InputSystemBinding::get()
{
    static InputSystemBinding Instance;
    return Instance;
}

bool InputSystemBinding::mouseMoved( const OIS::MouseEvent &arg )
{
    //Movement of the mouse wheel is considered movement of the z-axis in OIS.
    if (arg.state.Z.rel>0)
    {
      Console::getInstance()->addScript(Script("ZoomIn"));
    }
    else if (arg.state.Z.rel<0)
    {
      Console::getInstance()->addScript(Script("ZoomOut"));
    }
    return true;
}

bool InputSystemBinding::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    return true;
}

bool InputSystemBinding::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    return true;
}

} // end namespace
