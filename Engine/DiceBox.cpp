/*
 -----------------------------------------------------------------------------
    This file is part of the Dusk Engine.
    Copyright (C) 2009, 2010, 2011 thoronador

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

#include "DiceBox.h"
#include <ctime>
#include <cstdlib>

namespace Dusk
{

DiceBox::DiceBox()
{
  srand(time(NULL));
}

DiceBox::~DiceBox()
{
  //empty
}

DiceBox& DiceBox::getSingleton()
{
  static DiceBox Instance;
  return Instance;
}

unsigned int DiceBox::d4()
{
  const float res = rand()/float(RAND_MAX);
  return 1+ int(4*res);
}

unsigned int DiceBox::d6()
{
  const float res = rand()/float(RAND_MAX);
  return 1+ int(6*res);
}

unsigned int DiceBox::d8()
{
  const float res = rand()/float(RAND_MAX);
  return 1+ int(8*res);
}

unsigned int DiceBox::d10()
{
  const float res = rand()/float(RAND_MAX);
  return 1+ int(10*res);
}

unsigned int DiceBox::d20()
{
  const float res = rand()/float(RAND_MAX);
  return 1+ int(20*res);
}

unsigned int DiceBox::d4(const unsigned short int n)
{
  unsigned int result = 0;
  unsigned int i;
  for (i=0; i<n; i=i+1)
  {
    result += d4();
  }
  return result;
}

unsigned int DiceBox::d6(const unsigned short int n)
{
  unsigned int result = 0;
  unsigned int i;
  for (i=0; i<n; i=i+1)
  {
    result += d6();
  }
  return result;
}

unsigned int DiceBox::d8(const unsigned short int n)
{
  unsigned int result = 0;
  unsigned int i;
  for (i=0; i<n; i=i+1)
  {
    result += d8();
  }
  return result;
}

unsigned int DiceBox::d10(const unsigned short int n)
{
  unsigned int result = 0;
  unsigned int i;
  for (i=0; i<n; i=i+1)
  {
    result += d10();
  }
  return result;
}

unsigned int DiceBox::d20(const unsigned short int n)
{
  unsigned int result = 0;
  unsigned int i;
  for (i=0; i<n; i=i+1)
  {
    result += d20();
  }
  return result;
}

float DiceBox::random()
{
  return rand()/float(RAND_MAX);
}

} //namespace
