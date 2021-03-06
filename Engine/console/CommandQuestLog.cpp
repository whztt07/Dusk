/*
 -----------------------------------------------------------------------------
    This file is part of the Dusk Engine.
    Copyright (C) 2010, 2013  Thoronador

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -----------------------------------------------------------------------------
*/

#include "CommandQuestLog.h"
#include "../Menu.h"

namespace Dusk
{

CommandQuestLog::CommandQuestLog(const QuestLogOperation op)
: Command(),
  m_Operation(op)
{
}

CommandQuestLog::~CommandQuestLog()
{
  //empty
}

bool CommandQuestLog::execute(Dusk::Scene* scene, int count)
{
  switch (m_Operation)
  {
    case qloToggle:
         //toggles visibility of QuestLog menu
         Menu::getSingleton().showQuestLog(!Menu::getSingleton().isQuestLogVisible());
         break;
    case qloNext:
         Menu::getSingleton().nextQuestLogPage();
         break;
    case qloPrev:
         Menu::getSingleton().previousQuestLogPage();
         break;
    default: //should never happen
         throw 42;
  }//swi
  return true;
}

} //namespace
