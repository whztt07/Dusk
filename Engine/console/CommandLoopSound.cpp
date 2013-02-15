/*
 -----------------------------------------------------------------------------
    This file is part of the Dusk Engine.
    Copyright (C) 2008, 2009, 2010 thoronador

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

#include "CommandLoopSound.h"
#include "../Sound.h"

namespace Dusk
{
  //constructor
  CommandLoopSound::CommandLoopSound(const std::string& NoiseID, const bool Looping)
  {
    m_Noise = NoiseID;
    m_DoLoop = Looping;
  }

  //destructor
  CommandLoopSound::~CommandLoopSound()
  {
    //empty
  }

  bool CommandLoopSound::execute(Dusk::Scene* scene, int count)
  {
    return Sound::get().loopNoise(m_Noise, m_DoLoop);
  }
}