/*---------------------------------------------------------------------------
 Author:  thoronador
 Date:    2010-05-15
 Purpose: LuaIncludes
          Summarizes all #includes specific to Lua, so we can simply do a
            #include "LuaIncludes.h"
          instead of including all the Lua headers every time a file needs
          stuff from Lua.
          If you don't see why this is a good idea, read the comments below.

 History:
     - 2010-02-08 (rev 168) - initial version (by thoronador)
     - 2010-02-09 (rev 169) - should now work on Windows, too, although we use
                              Lua 5.1 and not Lua 5.0 for Windows
     - 2010-05-15 (rev 204) - Dusk-specific defines added; comments updated

 ToDo list:
     - ???

 Bugs/ Known Issues:
     - Will most likely not compile /work with Lua versions lower than 5.0.
     - Almost untested. If you find any bugs, tell us please and we will try to
       fix them.
 --------------------------------------------------------------------------*/

#ifndef LUAINCLUDES_H
#define LUAINCLUDES_H

extern "C" {
  #if defined(_WIN32)
    /* We use Lua 5.1 on Windows platforms, since 5.0 does not seem to be
       available for download on SF any more.
    */
    #include "lua51/lua.h"
    #include "lua51/lualib.h"
    #include "lua51/lauxlib.h"
    #define DUSK_LUA51
    /*compatibility macros (not completely happy with them)*/
    #define luaopen_loadlib(L)  luaopen_package(L)
    #define lua_dofile luaL_dofile
    #define lua_dostring luaL_dostring
  #elif defined(__linux__) || defined(linux)
    /* We use Lua 5.0 on Linux platforms.
       If you want to use a different Lua version, or use a different OS, you
       will have to change the following includes.
       These includes work on Debian 5.0 ("lenny") with package liblualib50-dev
       and its dependencies installed, but they might need to be changed for
       other systems.
       (The main reason for the use of Lua 5.0 instead of Lua 5.1 is the Editor,
       which depends on CEGUI, and the current CEGUI package depends on Lua 5.0.
       This way we avoid to force the user to have two different Lua versions
       installed at the same time.)
    */
    #include <lua50/lua.h>
    #include <lua50/lualib.h>
    #include <lua50/lauxlib.h>
    #define DUSK_LUA50
  #else
    #error "LuaIncludes could not detect a known operating system!"
  #endif
}

#endif // LUAINCLUDES_H