/*
 -----------------------------------------------------------------------------
    This file is part of the Dusk Engine.
    Copyright (C) 2009, 2010 thoronador

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

#include "Landscape.h"
#include "DuskConstants.h"
#include <iostream>
#include <sstream>
#ifndef NO_OGRE_IN_LANDSCAPE
  #include <OgreMath.h>
#endif

namespace Dusk
{

  const std::string Landscape::cLandNodeName = "LandscapeNode";
  const unsigned int Landscape::cMaxLandRecords = 2500; //50 x 50 squares should do

  const float LandscapeRecord::cDefaultStride = 5.00;
  const float LandscapeRecord::cMinScale = 0.01;
  const std::string LandscapeRecord::cLandscapeNamePrefix = "Landscape";

unsigned int GenerateUniqueID()
{
  static unsigned int m_genID = 0;
  return m_genID++;
}

//the Landscape record class
// ++++
// ++ Contains the (height and colour) data of the landscape chunks.
// ++ However, it should be created and managed by the Landscape class and not
// ++ created or manipulated directly by the application.
// ++++
LandscapeRecord::LandscapeRecord()
{
  m_Loaded = false;
  #ifndef NO_OGRE_IN_LANDSCAPE
  m_OgreObject = NULL;
  #endif
  m_RecordID = GenerateUniqueID();
  m_OffsetX = 0.0;
  m_OffsetY = 0.0;
  m_Stride = cDefaultStride;
  m_Highest = 0.0;
  m_Lowest = 0.0;
}

LandscapeRecord::~LandscapeRecord()
{
  //empty destructor
  #ifndef NO_OGRE_IN_LANDSCAPE
  if (m_OgreObject != NULL)
  {
    disable();
  }
  #endif
}

float LandscapeRecord::getHighest() const
{
  return m_Highest;
}

float LandscapeRecord::getLowest() const
{
  return m_Lowest;
}

float LandscapeRecord::getOffsetX() const
{
  return m_OffsetX;
}

float LandscapeRecord::getOffsetY() const
{
  return m_OffsetY;
}

float LandscapeRecord::getStride() const
{
  return m_Stride;
}

bool LandscapeRecord::loadFromStream(std::ifstream &AStream)
{
  if (m_Loaded)
  {
    std::cout << "LandscapeRecord::LoadFromStream: ERROR: record already "
              << "contains data.\n";
    return false;
  }
  if (!AStream.good())
  {
    std::cout << "LandscapeRecord::LoadFromStream: ERROR: passed stream "
              << "argument contains error(s).\n";
    return false;
  }

  unsigned int Land;
  Land = 0;
  //read header "Land"
  AStream.read((char*) &Land, sizeof(unsigned int));
  if (Land!=cHeaderLand)
  {
    std::cout << "LandscapeRecord::LoadFromStream: Stream contains invalid "
              << "Landscape record header.\n";
    return false;
  }
  //read offsets
  AStream.read((char*) &m_OffsetX, sizeof(float));
  AStream.read((char*) &m_OffsetY, sizeof(float));
  //stride
  AStream.read((char*) &m_Stride, sizeof(float));
  if (!AStream.good())
  {
    std::cout << "LandscapeRecord::LoadFromStream: ERROR: Stream seems to "
              << "have invalid Land record data.\n";
    return false;
  }
  if (m_Stride <=0.0f)
  {
    std::cout << "LandscapeRecord::LoadFromStream: Stream contains an invalid "
              << "stride value of "<< m_Stride <<". Setting default value. Exit.\n";
    m_Stride = cDefaultStride;
    return false;
  }//if

  //read the height data
  AStream.read((char*) &Height[0][0], 65*65*sizeof(float));
  if (!AStream.good())
  {
    std::cout << "LandscapeRecord::LoadFromStream: ERROR: Stream seems to have"
              << " invalid Land record height data.\n";
    return false;
  }

  //colour data
  AStream.read((char*) &Colour[0][0][0], 65*65*3);
  if (!AStream.good())
  {
    std::cout << "LandscapeRecord::LoadFromStream: ERROR: Stream seems to "
              << "have invalid Land record colour data.\n";
    return false;
  }

  //search Highest and Lowest values
  unsigned int i, j;

  m_Highest= Height[0][0];
  m_Lowest = Height[0][0];
  for (i=0; i<65; i++)
  {
    for (j=0; j<65; j++)
    {
      if (Height[i][j]>m_Highest)
      {
        m_Highest = Height[i][j];
      }
      else if (Height[i][j]<m_Lowest)
      {
        m_Lowest = Height[i][j];
      }
    }//for j
  }//for i
  m_Loaded = true;
  return true;
}//LoadFromStream

bool LandscapeRecord::saveToStream(std::ofstream &AStream) const
{
  if (!m_Loaded)
  {
    std::cout << "LandscapeRecord::SaveToStream: ERROR: record is not loaded.\n";
    return false;
  }
  if (!AStream.good())
  {
    std::cout << "LandscapeRecord::SaveToStream: ERROR: passed stream argument"
              << " contains error(s).\n";
    return false;
  }
  //write header "Land"
  AStream.write((char*) &cHeaderLand, sizeof(unsigned int));
  //write offsets
  AStream.write((char*) &m_OffsetX, sizeof(float));
  AStream.write((char*) &m_OffsetY, sizeof(float));
  //stride
  AStream.write((char*) &m_Stride, sizeof(float));
  //height data
  AStream.write((char*) &Height[0][0], 65*65*sizeof(float));
  //colour data
  AStream.write((char*) &Colour[0][0][0], 65*65*3);
  if (!AStream.good())
  {
    std::cout << "LandscapeRecord::SaveToStream: Error while writing record to"
              << " stream.\n";
    return false;
  }//if
  return true;
}//SaveToStream

bool LandscapeRecord::isLoaded() const
{
  return m_Loaded;
}

void LandscapeRecord::setLoadedState(const bool value)
{
  m_Loaded = value;
}

bool LandscapeRecord::shift(const float delta)
{
  unsigned int i,j;

  for (i=0; i<65; i++)
  {
    for (j=0; j<65; j++)
    {
      Height[i][j] = Height[i][j]+delta;
    }
  }
  m_Highest = m_Highest+delta;
  m_Lowest = m_Lowest+delta;
  #ifndef NO_OGRE_IN_LANDSCAPE
  if (isEnabled())
  {
    Landscape::getSingleton().requestUpdate(this);
  }
  #endif
  return true;
}

bool LandscapeRecord::scale(const float factor)
{
  if (!m_Loaded || factor<cMinScale)
  {
    std::cout << "LandscapeRecord::Scale: ERROR: Record is not loaded yet or "
              << "scaling factor ("<<factor<<") is invalid.\n";
    return false;
  }
  unsigned int i,j;

  for (i=0; i<65; i++)
  {
    for (j=0; j<65; j++)
    {
      Height[i][j] = Height[i][j]*factor;
    }
  }
  m_Highest = m_Highest*factor;
  m_Lowest = m_Lowest*factor;
  #ifndef NO_OGRE_IN_LANDSCAPE
  if (isEnabled())
  {
    Landscape::getSingleton().requestUpdate(this);
  }
  #endif
  return true;
}

bool LandscapeRecord::makePlain(const float value)
{
  unsigned int i,j;

  for (i=0; i<65; i++)
  {
    for (j=0; j<65; j++)
    {
      Height[i][j] = value;
    }
  }
  m_Highest = value;
  m_Lowest = value;
  setLoadedState(true);
  #ifndef NO_OGRE_IN_LANDSCAPE
  if (isEnabled())
  {
    Landscape::getSingleton().requestUpdate(this);
  }
  #endif
  return true;
}

bool LandscapeRecord::isPlain() const
{
  return m_Highest==m_Lowest;
}

bool LandscapeRecord::generateByFunction( float (*func) (const float x, const float z))
{
  if (func==NULL)
  {
    std::cout << "LandscapeRecord::GenerateByFunction: ERROR: pointer to "
              << "function is NULL!\n";
    return false;
  }

  unsigned int i,j;

  m_Highest = -(1.0f/0.0f); //negative infinity
  m_Lowest = 1.0f/0.0f; //positive infinity
  for (i=0; i<65; ++i)
  {
    for (j=0; j<65; ++j)
    {
      Height[i][j] = func((float)i/64.0f, (float)j/64.0f);
      if (Height[i][j]>m_Highest)
      {
        m_Highest = Height[i][j];
      }
      else if (Height[i][j]<m_Lowest)
      {
        m_Lowest = Height[i][j];
      }
    }//for j
  }//for i
  setLoadedState(true);
  #ifndef NO_OGRE_IN_LANDSCAPE
  if (isEnabled())
  {
    Landscape::getSingleton().requestUpdate(this);
  }
  #endif
  return true;
}//function

bool LandscapeRecord::colourByFunction(ColourData (*func) (const float x, const float z))
{
  if (func==NULL)
  {
    std::cout << "LandscapeRecord::ColourByFunction: ERROR: pointer to "
              << "function is NULL!\n";
    return false;
  }

  unsigned int i,j;
  for (i=0; i<65; ++i)
  {
    for (j=0; j<65; ++j)
    {
      const ColourData cd = func((float)i/64.0f, (float)j/64.0f);
      Colour[i][j][0] = cd.red;
      Colour[i][j][1] = cd.green;
      Colour[i][j][2] = cd.blue;
    }//for (inner)
  }//for
  #ifndef NO_OGRE_IN_LANDSCAPE
  if (isEnabled())
  {
    Landscape::getSingleton().requestUpdate(this);
  }
  #endif
  return true;
}

void LandscapeRecord::moveTo(const float Offset_X, const float Offset_Y)
{
  m_OffsetX = Offset_X;
  m_OffsetY = Offset_Y;
  #ifndef NO_OGRE_IN_LANDSCAPE
  if (isEnabled())
  {
    Landscape::getSingleton().requestUpdate(this);
  }
  #endif
}

bool LandscapeRecord::terraform(const float x, const float z, const float delta)
{
  if (x>=m_OffsetX && x<=m_OffsetX+64*m_Stride
     && z>=m_OffsetY && z<=m_OffsetY+64*m_Stride)
  {
    unsigned int x_idx, y_idx;
    x_idx = (unsigned int)((x-m_OffsetX)/m_Stride);
    y_idx = (unsigned int)((z-m_OffsetY)/m_Stride);
    Height[x_idx][y_idx] += delta;
    //adjust highest/ lowest values, if neccessary
    if (Height[x_idx][y_idx] > m_Highest)
    {
      m_Highest = Height[x_idx][y_idx];
    }
    else if (Height[x_idx][y_idx] < m_Lowest)
    {
      m_Lowest = Height[x_idx][y_idx];
    }
    #ifndef NO_OGRE_IN_LANDSCAPE
    if (isEnabled())
    {
      Landscape::getSingleton().requestUpdate(this);
    }
    #endif
    return true;
  }
  return false;
}

bool LandscapeRecord::setColour(const float x, const float z, const unsigned char r,const unsigned char g, const unsigned char b)
{
  if (x>=m_OffsetX && x<=m_OffsetX+64*m_Stride
     && z>=m_OffsetY && z<=m_OffsetY+64*m_Stride)
  {
    unsigned int x_idx, y_idx;
    x_idx = (unsigned int)((x-m_OffsetX)/m_Stride);
    y_idx = (unsigned int)((z-m_OffsetY)/m_Stride);
    std::cout << "DEBUG: LandscapeRecord::SetColour(): x_idx="<<x_idx<<", y_idx="<<y_idx<<"\n";
    Colour[x_idx][y_idx][0] = r;
    Colour[x_idx][y_idx][1] = g;
    Colour[x_idx][y_idx][2] = b;
    #ifndef NO_OGRE_IN_LANDSCAPE
    if (isEnabled())
    {
      Landscape::getSingleton().requestUpdate(this);
    }
    #endif
    return true;
  }
  std::cout << "DEBUG: LandscapeRecord::SetColour(): call with x or z value out of range.\n";
  return false;
}

bool LandscapeRecord::setStride(const float new_stride)
{
  if (new_stride>0.0f)
  {
    if (new_stride!=m_Stride)
    {
      m_Stride = new_stride;
      #ifndef NO_OGRE_IN_LANDSCAPE
      if (isEnabled())
      {
        Landscape::getSingleton().requestUpdate(this);
      }
      #endif
    }
    return true;
  }
  return false;
}

#ifndef NO_OGRE_IN_LANDSCAPE
bool LandscapeRecord::enable(Ogre::SceneManager * scm, const bool WireFrame)
{
  if (!m_Loaded)
  {
    std::cout << "LandscapeRecord::Enable: ERROR: Record has no valid data (yet).\n";
    return false;
  }

  if (m_OgreObject != NULL)
  {
    std::cout << "LandscapeRecord::Enable: Hint: Record is already enabled.\n";
    return true;
  }

  if (scm==NULL)
  {
    std::cout << "LandscapeRecord::Enable: ERROR: Got NULL for scene manager.\n";
    return false;
  }

  //get own scene node for landscape
  if (!scm->hasSceneNode(Landscape::cLandNodeName))
  {
    std::cout << "LandscapeRecord::Enable: ERROR: LandscapeNode does not exist.\n";
    return false;
  }
  Ogre::SceneNode * landnode;
  landnode = scm->getSceneNode(Landscape::cLandNodeName);

  std::stringstream convert;
  convert << getID();

  m_OgreObject = scm->createManualObject(cLandscapeNamePrefix+convert.str());
  m_OgreObject->estimateVertexCount(65*65);
  if (WireFrame)
  {
    m_OgreObject->estimateIndexCount(64*64*4);
  }
  else
  {
    m_OgreObject->estimateIndexCount(64*64*6);
  }
  m_OgreObject->setDynamic(false);
  unsigned int j, k;
  if (WireFrame)
  { //wire frame model
    m_OgreObject->begin("Landscape/VertexColour", Ogre::RenderOperation::OT_LINE_LIST);
    //vertices
    for (j=0; j<65; ++j)
    {
      for (k=0; k<65; ++k)
      {
        m_OgreObject->position(m_OffsetX+m_Stride*j,
                               Height[j][k],
                               m_OffsetY+m_Stride*k);
        m_OgreObject->colour(0.0f, 1.0f, 0.0f);
      }//for k
    }//for j

    //lines in one direction
    for(j=0; j<64; ++j)
    {
      for(k=0; k<65; ++k)
      {
        //line: [j][k] to [j+1][k]
        m_OgreObject->index(j*65+k);
        m_OgreObject->index((j+1)*65+k);
      }//for
    }//for

    //lines in other direction
    for(j=0; j<65; ++j)
    {
      for(k=0; k<64; ++k)
      {
        //line: [j][k] to [j][k+1]
        m_OgreObject->index(j*65+k);
        m_OgreObject->index(j*65+k+1);
      }//for
    }//for
  }//if WireFrame
  else
  { //"solid" landscape
    m_OgreObject->begin("Landscape/VertexColour", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    //vertices
    for (j=0; j<65; ++j)
    {
      for (k=0; k<65; ++k)
      {
        m_OgreObject->position(m_OffsetX+m_Stride*j,
                               Height[j][k],
                               m_OffsetY+m_Stride*k);
        m_OgreObject->colour(Colour[j][k][0]/255.0f,
                             Colour[j][k][1]/255.0f,
                             Colour[j][k][2]/255.0f);
      }//for k
    }//for j

    //triangles
    for (j=0; j<64; ++j)
    {
      for (k=0; k<64; ++k)
      {
        //first triangle: [j][k], [j][k+1], [j+1][k]
        m_OgreObject->index(j*65+k);
        m_OgreObject->index(j*65+k+1);
        m_OgreObject->index((j+1)*65+k);
        //second triangle: [j+1][k], [j][k+1], [j+1][k+1]
        m_OgreObject->index((j+1)*65+k);
        m_OgreObject->index(j*65+k+1);
        m_OgreObject->index((j+1)*65+k+1);
      }//for k
    }//for j
  }//if not wire frame
  m_OgreObject->end();
  landnode->attachObject(m_OgreObject);
  return true;
}

bool LandscapeRecord::disable()
{
  if (m_OgreObject == NULL)
  {
    return true;
  }

  Ogre::SceneManager * scm;
  scm = m_OgreObject->getParentSceneNode()->getCreator();
  if (scm==NULL)
  {
    std::cout << "LandscapeRecord::Disable: ERROR: Got NULL for "
              << "scene manager.\n";
    return false;
  }

  //get scene node for landscape
  if (!scm->hasSceneNode(Landscape::cLandNodeName))
  {
    std::cout << "LandscapeRecord::Disable: ERROR: LandscapeNode does not exist.\n";
    return false;
  }
  Ogre::SceneNode * landnode;
  landnode = scm->getSceneNode(Landscape::cLandNodeName);
  landnode->detachObject(m_OgreObject);
  scm->destroyManualObject(m_OgreObject);
  m_OgreObject = NULL;
  return true;
}

bool LandscapeRecord::update(const bool WireFrame)
{
  if (m_OgreObject == NULL)
  {
    return true;
  }
  Ogre::SceneManager* scm = m_OgreObject->getParentSceneNode()->getCreator();
  if (scm==NULL)
  {
    std::cout << "LandscapeRecord::Update: ERROR: SceneManager is NULL.\n";
    return false;
  }
  if (!disable())
  {
    std::cout << "LandscapeRecord::Update: ERROR: Could not remove record.\n";
    return false;
  }
  if (!enable(scm, WireFrame))
  {
    std::cout << "LandscapeRecord::Update: ERROR: Could not enable record.\n";
    return false;
  }
  return true;
}

bool LandscapeRecord::isEnabled() const
{
  return (m_OgreObject != NULL);
}

bool LandscapeRecord::isLandscapeRecordName(const std::string& val)
{
  if (val.length()<=cLandscapeNamePrefix.length())
  {
    return false;
  }
  if (val.substr(0,cLandscapeNamePrefix.length())!=cLandscapeNamePrefix)
  {
    return false;
  }
  std::stringstream s_str(val.substr(cLandscapeNamePrefix.length()));
  int value = -1;
  if (!(s_str>>value))
  {
    return false;
  }
  return (value>=0);
  //return (StringToInt(val.substr(cLandscapeNamePrefix.length()), -1)>=0);
}

bool LandscapeRecord::isHitByRay(const Ogre::Ray& ray, Ogre::Vector3& HitPoint) const
{
  Ogre::Real minDistance = -1.0f;
  unsigned int i, j;
  for (i=0; i<64; i=i+1)
  {
    for (j=0; j<64; j=j+1)
    {
      //first triangle: [i][j], [i][j+1], [i+1][j]
      std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray,
                       getPositionOfIndex(i,j), getPositionOfIndex(i,j+1),
                       getPositionOfIndex(i+1,j), true, false);
      if (hit.first)
      {
        if ((hit.second < minDistance) or (minDistance<0.0f))
        { //new closest hit point
          minDistance = hit.second;
        }
      }
      //second triangle: [i+1][j], [i][j+1], [i+1][j+1]
      hit = Ogre::Math::intersects(ray, getPositionOfIndex(i+1,j),
                       getPositionOfIndex(i,j+1), getPositionOfIndex(i+1,j+1),
                       true, false);
      if (hit.first)
      {
        if ((hit.second < minDistance) or (minDistance<0.0f))
        { //new closest hit point
          minDistance = hit.second;
        }
      }
    }//for j
  }//for i
  //get return values
  if (minDistance > -0.5f)
  {
    HitPoint = ray.getPoint(minDistance);
    return true;
  }
  return false;
}

const Ogre::Vector3 LandscapeRecord::getPositionOfIndex(const unsigned int i, const unsigned int j) const
{
  if (i<65 and j<65)
  {
    return Ogre::Vector3(m_OffsetX+i*m_Stride, Height[i][j], m_OffsetY+j*m_Stride);
  }
  return Ogre::Vector3::ZERO;
}

#endif //ifndef NO_OGRE_IN_LANDSCAPE

unsigned int LandscapeRecord::getID() const
{
  return m_RecordID;
}

//the main Landscape class
// ++++
// ++ Basically it will be a sort of manager for all the individual landscape
// ++ records and the point through which other classes access the landscape
// ++ data.
// ++++
Landscape::Landscape()
{
  m_RecordList = NULL;
  m_numRec = 0;
  m_Capacity = 0;
  #ifndef NO_OGRE_IN_LANDSCAPE
  m_RecordsForUpdate.clear();
  #endif
}

Landscape::~Landscape()
{
  //empty
  if (m_RecordList != NULL)
  {
    int i = 0;
    for (i=m_numRec-1; i>=0; i=i-1)
    {
      if (m_RecordList[i]!=NULL)
      {
        delete m_RecordList[i];
        m_RecordList[i] = NULL;
      }
    }//for
    delete [] m_RecordList;
    m_RecordList = NULL;
  }//if
}//destructor

Landscape& Landscape::getSingleton()
{
    static Landscape Instance;
    return Instance;
}

bool Landscape::loadFromFile(const std::string& FileName)
{
  if (m_numRec!=0)
  {
    std::cout << "Landscape::LoadFromFile: Landscape data is already present.\n";
    return false;
  }

  unsigned int numRecords, i;
  unsigned int Header;
  std::ifstream input;

  input.open(FileName.c_str(), std::ios::in | std::ios::binary);
  if(!input)
  {
    std::cout << "Landscape::LoadFromFile: Could not open file \""<<FileName
              << "\" for reading in binary mode.\n";
    return false;
  }//if

  //read header "Dusk"
  Header = 0;
  input.read((char*) &Header, sizeof(unsigned int));
  if (Header!=cHeaderDusk)
  {
    std::cout << "Landscape::LoadFromFile: File \""<<FileName<< "\" has "
              << "invalid header.\n";
    input.close();
    return false;
  }
  //read total number of records in file
  input.read((char*) &numRecords, sizeof(unsigned int));
  if (!input.good())
  {
    std::cout << "Landscape::LoadFromFile: File \""<<FileName<< "\" has "
              << "invalid, (short?) header.\n";
    input.close();
    return false;
  }

  if (numRecords>cMaxLandRecords)
  {
    std::cout << "Landscape::LoadFromFile: File \""<<FileName<< "\" has "
              << "more than "<<cMaxLandRecords<<" records.\n";
    input.close();
    return false;
  }

  //m_RecordList = new LandscapeRecord[numRecords];
  //m_numRec = numRecords;
  changeListSize(numRecords);

  //read loop
  for (i=0; i<numRecords; i=i+1)
  {
    LandscapeRecord* tr = createRecord();
    if (!tr->loadFromStream(input))
    {
      std::cout << "Landscape::LoadFromFile: File \""<<FileName<< "\" has "
                << "invalid record data. Clearing loaded data.\n";
      //delete[] m_RecordList;
      //m_RecordList= NULL;
      //m_numRec = 0;
      changeListSize(0);
      input.close();
      return false;
    }
  }//for
  input.close();
  return true;
}

bool Landscape::saveToFile(const std::string& FileName) const
{
  if (m_numRec==0)
  {
    std::cout << "Landscape::SaveToFile: No Landscape data is present.\n";
    return false;
  }

  std::ofstream output;

  output.open(FileName.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
  if(!output)
  {
    std::cout << "Landscape::SaveToFile: Could not open file \""<<FileName
              << "\" for writing in binary mode.\n";
    return false;
  }//if

  //write header "Dusk"
  output.write((char*) &cHeaderDusk, sizeof(unsigned int));
  output.write((char*) &m_numRec, sizeof(unsigned int));

  unsigned int i;

  for (i=0; i<m_numRec; i=i+1)
  {
    if (!m_RecordList[i]->saveToStream(output))
    {
      std::cout << "Landscape::SaveToFile: Error while writing record "<<i+1
                << " to file \"" <<FileName<<"\".\n";
      output.close();
      return false;
    }
  }//for
  output.close();
  return true;
}//SaveToFile

bool Landscape::saveAllToStream(std::ofstream& AStream) const
{
  if (!AStream.good())
  {
    std::cout << "Landscape::SaveAllToStream: ERROR: passed stream argument"
              << " contains error(s).\n";
    return false;
  }
  unsigned int i;
  for (i=0; i<m_numRec; i=i+1)
  {
    if (!m_RecordList[i]->saveToStream(AStream))
    {
      std::cout << "Landscape::SaveAllToStream: Error while saving record "<<i+1
                << " to stream.\n";
      return false;
    }
  }//for
  return AStream.good();
}

void Landscape::changeListSize(const unsigned int new_size)
{
  LandscapeRecord ** new_list;
  LandscapeRecord ** temp_list;
  unsigned int i, copy_count;

  //allocate new list
  new_list = new LandscapeRecord*[new_size];
  //copy existing pointers
  copy_count = m_numRec;
  if (new_size<m_numRec)
  {
    copy_count = new_size;
    //delete unwanted records
    for (i=new_size; i<m_numRec; ++i)
    {
      delete m_RecordList[i];
    }
  }//if

  for (i=0; i<m_numRec; ++i)
  {
    new_list[i] = m_RecordList[i];
  }//for
  //nullify new pointers
  for (i=m_numRec; i<new_size; ++i)
  {
    new_list[i] = NULL;
  }//for
  temp_list = m_RecordList;
  m_RecordList = new_list;
  if (new_size<m_numRec)
  {
    m_numRec = new_size;
  }//if
  delete [] temp_list;
  m_Capacity = new_size;
}//ChangeListSize

LandscapeRecord* Landscape::createRecord()
{
  //check for insufficient list length
  if (m_numRec == m_Capacity)
  {
    //allocate new memory
    if (m_Capacity<500)
    {
      changeListSize(m_Capacity*2+1);
    }//if
    else
    {
      changeListSize(m_Capacity+100);
    }
  }//if

  m_RecordList[m_numRec] = new LandscapeRecord;
  m_numRec = m_numRec +1;
  return m_RecordList[m_numRec-1];
}

void Landscape::destroyRecord(const LandscapeRecord* recPtr)
{
  //far from optimal :(
  if (recPtr==NULL)
  {
    return;
  }

  unsigned int i;

  for (i=0; i<m_numRec; i++)
  {
    if (recPtr==m_RecordList[i])
    {
      delete m_RecordList[i];
      //fill space with last record
      m_RecordList[i] = m_RecordList[m_numRec-1];
      m_RecordList[m_numRec-1] = NULL;
      m_numRec = m_numRec-1;
      return;
    }//if
  }//for
}//DestroyRecord

unsigned int Landscape::getNumberOfRecordsAvailable()
{
  return m_numRec;
}

//gets pointer to the record-th record in list
//  ---- unsafe, because position of records might change due to deleting and
//       adding records
LandscapeRecord* Landscape::getRecordByPosition(const unsigned int record)
{
  if (record>=m_numRec)
  {
    return NULL;
  }
  return m_RecordList[record];
}

//get pointer to first record which covers position (x,-inf,z)
LandscapeRecord* Landscape::getRecordAtXZ(const float x, const float z) const
{
  unsigned int i;
  for(i=0; i<m_numRec; i++)
  {
    if ((x>=m_RecordList[i]->getOffsetX()) && (x<=m_RecordList[i]->getOffsetX()+64*LandscapeRecord::cDefaultStride)
       &&(z>=m_RecordList[i]->getOffsetY()) && (z<=m_RecordList[i]->getOffsetY()+64*LandscapeRecord::cDefaultStride))
    {
      return m_RecordList[i];
    }//if
  }//for
  return NULL;
}

//gets pointer to record with given ID
//  ---- safer than GetRecordByPosition(), but also slower
LandscapeRecord* Landscape::getRecordByID(const unsigned int recordID)
{
  if (m_numRec==0)
  {
    return NULL;
  }

  unsigned int i;

  for (i=0; i<m_numRec; i++)
  {
    if (m_RecordList[i]->getID() == recordID)
    {
      return m_RecordList[i];
    }
  }//for
  return NULL;
}

void Landscape::clearAllRecords()
{
  changeListSize(0);
}

#ifndef NO_OGRE_IN_LANDSCAPE
bool Landscape::sendToEngine(Ogre::SceneManager * scm, const bool WireFrame)
{
  if (m_numRec==0)
  {
    return false;
  }

  if (scm==NULL)
  {
    std::cout << "Landscape::SendToEngine: ERROR: Got NULL for scene manager.\n";
    return false;
  }

  if (!scm->hasSceneNode(cLandNodeName))
  {
    //create own scene node for landscape
    scm->getRootSceneNode()->createChildSceneNode(cLandNodeName, Ogre::Vector3(0.0, 0.0, 0.0));
  }

  unsigned int i;
  for (i=0; i<m_numRec; i++)
  {
    m_RecordList[i]->enable(scm, WireFrame);
  }//for
  return true;
}

bool Landscape::removeFromEngine(Ogre::SceneManager * scm)
{
  if(m_numRec == 0)
  {
    return true; //nothing to remove, i.e. done ;)
  }

  if (scm==NULL)
  {
    std::cout << "Landscape::RemoveFromEngine: ERROR: Got NULL for scene manager.\n";
    return false;
  }
  //remove all records' objects, one by one
  unsigned int i;
  for (i=0; i<m_numRec; i++)
  {
    m_RecordList[i]->disable();
  }//for
  if (scm->hasSceneNode(cLandNodeName))
  {
    //remove previously create scene node for landscape
    scm->getRootSceneNode()->removeChild(cLandNodeName);
    //delete scene node
    scm->destroySceneNode(cLandNodeName);
  }
  return true;
}

void Landscape::requestUpdate(LandscapeRecord* who)
{
  if (who!=NULL)
  {
    if (m_RecordsForUpdate.empty())
    {
      m_RecordsForUpdate.push_back(who);
    }
    else if (m_RecordsForUpdate.back()!=who)
    {
      m_RecordsForUpdate.push_back(who);
    }
  }
}

bool Landscape::needsUpdate() const
{
  return (!(m_RecordsForUpdate.empty()));
}

unsigned int Landscape::updateRecords(const bool WireFrame)
{
  unsigned int count = 0;
  while (!m_RecordsForUpdate.empty())
  {
    if (m_RecordsForUpdate.back()->update(WireFrame))
    {
      count++;
    }
    m_RecordsForUpdate.pop_back();
  }
  return count;
}

#endif //ifndef NO_OGRE_IN_LANDSCAPE

float Landscape::getHeightAtPosition(const float x, const float y) const
{
  if (m_numRec == 0)
  {
    return 0.0;
  }
  unsigned int i, x_idx, y_idx;
  for(i=0; i<m_numRec; i++)
  {
    const float stride = m_RecordList[i]->getStride();
    if ((x>=m_RecordList[i]->getOffsetX()) && (x<=m_RecordList[i]->getOffsetX()+64*stride)
       &&(y>=m_RecordList[i]->getOffsetY()) && (y<=m_RecordList[i]->getOffsetY()+64*stride))
    {
      //got it
      //not implemented exactly yet, but at least we have a return value which
      // is somewhat near the real value
      x_idx = (unsigned int)((x-m_RecordList[i]->getOffsetX())/stride);
      y_idx = (unsigned int)((y-m_RecordList[i]->getOffsetY())/stride);

      if (x_idx==64 or y_idx==64)
      {
        return m_RecordList[i]->Height[x_idx][y_idx];
      }

      //interpolation to get better approximation of height between points
      float x_linear, y_linear; //linear factors
      float ip1, ip2; //height at interpolatoin points 1 and 2
      x_linear = (x-m_RecordList[i]->getOffsetX()-x_idx*stride)/stride;
      y_linear = (y-m_RecordList[i]->getOffsetY()-y_idx*stride)/stride;

      ip1 = (1.0-x_linear)*m_RecordList[i]->Height[x_idx][y_idx]
           +      x_linear*m_RecordList[i]->Height[x_idx+1][y_idx];
      ip2 = (1.0-x_linear)*m_RecordList[i]->Height[x_idx][y_idx+1]
           +      x_linear*m_RecordList[i]->Height[x_idx+1][y_idx+1];
      return (1.0-y_linear)*ip1 + y_linear*ip2;
    }//if
  }//for
  return 0.0;
}

} //namespace
