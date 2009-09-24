#include "Landscape.h"
#include "DuskConstants.h"
#include <iostream>
#include <sstream>

namespace Dusk
{

  const std::string Landscape::cLandNodeName = "LandscapeNode";
  const unsigned int Landscape::cMaxLandRecords = 2500; //50 x 50 squares should do

  const float LandscapeRecord::cDefaultStride = 5.00;
  const float LandscapeRecord::cMinScale = 0.01;

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
  m_scm = NULL;
  #endif
  m_RecordID = GenerateUniqueID();
  m_OffsetX = 0.0;
  m_OffsetY = 0.0;
  Stride = cDefaultStride;
  m_Highest = 0.0;
  m_Lowest = 0.0;
}

LandscapeRecord::~LandscapeRecord()
{
  //empty destructor
  #ifndef NO_OGRE_IN_LANDSCAPE
  if (m_OgreObject != NULL)
  {
    RemoveDataFromEngine();
  }
  #endif
}

float LandscapeRecord::Highest() const
{
  return m_Highest;
}

float LandscapeRecord::Lowest() const
{
  return m_Lowest;
}

float LandscapeRecord::OffsetX() const
{
  return m_OffsetX;
}

float LandscapeRecord::OffsetY() const
{
  return m_OffsetY;
}

bool LandscapeRecord::LoadFromStream(std::ifstream &AStream)
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
  AStream.read((char*) &Stride, sizeof(float));
  if (!AStream.good())
  {
    std::cout << "LandscapeRecord::LoadFromStream: ERROR: Stream seems to "
              << "have invalid Land record data.\n";
    return false;
  }
  if (Stride <=0.0f)
  {
    std::cout << "LandscapeRecord::LoadFromStream: Stream contains an invalid "
              << "stride value of "<< Stride <<". Setting default value. Exit.\n";
    Stride = cDefaultStride;
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

bool LandscapeRecord::SaveToStream(std::ofstream &AStream) const
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
  AStream.write((char*) &Stride, sizeof(float));
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

bool LandscapeRecord::IsLoaded() const
{
  return m_Loaded;
}

void LandscapeRecord::SetLoadedState(const bool value)
{
  m_Loaded = value;
}

bool LandscapeRecord::Shift(const float delta)
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
  return true;
}

bool LandscapeRecord::Scale(const float factor)
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
  return true;
}

bool LandscapeRecord::MakePlain(const float value)
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
  SetLoadedState(true);
  return true;
}

bool LandscapeRecord::IsPlain() const
{
  return m_Highest==m_Lowest;
}

bool LandscapeRecord::GenerateByFunction( float (*func) (const float x, const float z))
{
  unsigned int i,j;

  if (func==NULL)
  {
    std::cout << "LandscapeRecord::GenerateByFunction: ERROR: pointer to "
              << "function is NULL!\n";
    return false;
  }

  m_Highest = 0.0;
  m_Lowest = 0.0;
  for (i=0; i<65; i++)
  {
    for (j=0; j<65; j++)
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
  SetLoadedState(true);
  return true;
}//function

void LandscapeRecord::MoveTo(const float Offset_X, const float Offset_Y)
{
  m_OffsetX = Offset_X;
  m_OffsetY = Offset_Y;
}

#ifndef NO_OGRE_IN_LANDSCAPE
bool LandscapeRecord::SendDataToEngine(Ogre::SceneManager * scm)
{
  if (!m_Loaded)
  {
    std::cout << "LandscapeRecord::SendDataToEngine: ERROR: Record has no valid data (yet).\n";
    return false;
  }

  if (scm==NULL)
  {
    std::cout << "LandscapeRecord::SendDataToEngine: ERROR: Got NULL for scene manager.\n";
    return false;
  }

  //get own scene node for landscape
  if (!scm->hasSceneNode(Landscape::cLandNodeName))
  {
    std::cout << "LandscapeRecord::SendDataToEngine: ERROR: LandscapeNode does not exist.\n";
    return false;
  }
  Ogre::SceneNode * landnode;
  landnode = scm->getSceneNode(Landscape::cLandNodeName);

  std::stringstream convert;
  convert << GetID();

  m_OgreObject = scm->createManualObject("Landscape"+convert.str());
  m_OgreObject->estimateVertexCount(65*65);
  m_OgreObject->estimateIndexCount(64*64*6);
  m_OgreObject->setDynamic(false);
  m_OgreObject->begin("Landscape/Green", Ogre::RenderOperation::OT_TRIANGLE_LIST);
  unsigned int j, k;
  //vectors
  for (j=0; j<65; j++)
  {
    for (k=0; k<65; k++)
    {
      m_OgreObject->position(m_OffsetX+cDefaultStride*j,
                             Height[j][k],
                             m_OffsetY+cDefaultStride*k);
      m_OgreObject->colour(Colour[j][k][0]/255.0f,
                           Colour[j][k][1]/255.0f,
                           Colour[j][k][2]/255.0f);
    }//for k
  }//for j

  //triangles
  for (j=0; j<64; j++)
  {
    for (k=0; k<64; k++)
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
  m_OgreObject->end();
  landnode->attachObject(m_OgreObject);
  m_scm = scm;
  return true;
}

bool LandscapeRecord::RemoveDataFromEngine()
{
  if (m_OgreObject == NULL)
  {
    return true;
  }

  Ogre::SceneManager * scm;
  scm = m_OgreObject->getParentSceneNode()->getCreator();
  if (scm==NULL)
  {
    std::cout << "LandscapeRecord::RemoveDataFromEngine: ERROR: Got NULL for "
              << "scene manager.\n";
    return false;
  }

  //get scene node for landscape
  if (!scm->hasSceneNode(Landscape::cLandNodeName))
  {
    std::cout << "LandscapeRecord::RemoveDataFromEngine: ERROR: LandscapeNode does not exist.\n";
    return false;
  }
  Ogre::SceneNode * landnode;
  landnode = scm->getSceneNode(Landscape::cLandNodeName);
  landnode->detachObject(m_OgreObject);
  scm->destroyManualObject(m_OgreObject);
  m_OgreObject = NULL;
  return true;
}
#endif //ifndef NO_OGRE_IN_LANDSCAPE

unsigned int LandscapeRecord::GetID() const
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

Landscape& Landscape::GetSingleton()
{
    static Landscape Instance;
    return Instance;
}

bool Landscape::LoadFromFile(const std::string& FileName)
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
  ChangeListSize(numRecords);

  //read loop
  for (i=0; i<numRecords; i=i+1)
  {
    LandscapeRecord* tr = CreateRecord();
    if (!tr->LoadFromStream(input))
    {
      std::cout << "Landscape::LoadFromFile: File \""<<FileName<< "\" has "
                << "invalid record data. Clearing loaded data.\n";
      //delete[] m_RecordList;
      //m_RecordList= NULL;
      //m_numRec = 0;
      ChangeListSize(0);
      input.close();
      return false;
    }
  }//for
  input.close();
  return true;
}

bool Landscape::SaveToFile(const std::string& FileName)
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
    if (!m_RecordList[i]->SaveToStream(output))
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

void Landscape::ChangeListSize(const unsigned int new_size)
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
    for (i=new_size; i<m_numRec; i++)
    {
      delete m_RecordList[i];
    }
  }//if

  for (i=0; i<m_numRec; i++)
  {
    new_list[i] = m_RecordList[i];
  }//for
  //nullify new pointers
  for (i=m_numRec; i<new_size; i++)
  {
    new_list[i] = NULL;
  }//for
  temp_list = m_RecordList;
  m_RecordList = new_list;
  delete [] temp_list;
  m_Capacity = new_size;
}//ChangeListSize

LandscapeRecord* Landscape::CreateRecord()
{
  //check for insufficient list length
  if (m_numRec == m_Capacity)
  {
    //allocate new memory
    if (m_Capacity<500)
    {
      ChangeListSize(m_Capacity*2+1);
    }//if
    else
    {
      ChangeListSize(m_Capacity+100);
    }
  }//if

  m_RecordList[m_numRec] = new LandscapeRecord;
  m_numRec = m_numRec +1;
  return m_RecordList[m_numRec-1];
}

void Landscape::DestroyRecord(const LandscapeRecord* recPtr)
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

unsigned int Landscape::RecordsAvailable()
{
  return m_numRec;
}

//gets pointer to the record-th record in list
//  ---- unsafe, because position of records might change due to deleting and
//       adding records
LandscapeRecord* Landscape::GetRecordByPosition(const unsigned int record)
{
  if (record>=m_numRec)
  {
    return NULL;
  }
  return m_RecordList[record];
}

//gets pointer to record with given ID
//  ---- safer than GetRecordByPosition(), but also slower
LandscapeRecord* Landscape::GetRecordByID(const unsigned int recordID)
{
  if (m_numRec==0)
  {
    return NULL;
  }

  unsigned int i;

  for (i=0; i<m_numRec; i++)
  {
    if (m_RecordList[i]->GetID() == recordID)
    {
      return m_RecordList[i];
    }
  }//for
  return NULL;
}

void Landscape::ClearAllRecords()
{
  ChangeListSize(0);
}

#ifndef NO_OGRE_IN_LANDSCAPE
bool Landscape::SendToEngine(Ogre::SceneManager * scm)
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
    m_RecordList[i]->SendDataToEngine(scm);
  }//for
  return true;
}

bool Landscape::RemoveFromEngine(Ogre::SceneManager * scm)
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
    m_RecordList[i]->RemoveDataFromEngine();
  }//for
  if (scm->hasSceneNode(cLandNodeName))
  {
    //remove previously create scene node for landscape
    scm->getRootSceneNode()->removeChild(cLandNodeName);
  }
  return true;
}

#endif //ifndef NO_OGRE_IN_LANDSCAPE

float Landscape::GetHeightAtPosition(const float x, const float y) const
{
  if (m_numRec == 0)
  {
    return 0.0;
  }
  unsigned int i, x_idx, y_idx;
  for(i=0; i<m_numRec; i++)
  {
    if ((x>=m_RecordList[i]->OffsetX()) && (x<=m_RecordList[i]->OffsetX()+64*LandscapeRecord::cDefaultStride)
       &&(y>=m_RecordList[i]->OffsetY()) && (y<=m_RecordList[i]->OffsetY()+64*LandscapeRecord::cDefaultStride))
    {
      //got it
      //not implemented exactly yet, but at least we have a return value which
      // is somewhat near the real value
      x_idx = (x-m_RecordList[i]->OffsetX())/LandscapeRecord::cDefaultStride;
      y_idx = (y-m_RecordList[i]->OffsetY())/LandscapeRecord::cDefaultStride;

      if (x_idx==64 or y_idx==64)
      {
        return m_RecordList[i]->Height[x_idx][y_idx];
      }

      //interpolation to get better approximation of height between points
      float x_linear, y_linear; //linear factors
      float ip1, ip2; //height at interpolatoin points 1 and 2
      x_linear = (x-m_RecordList[i]->OffsetX()-x_idx*LandscapeRecord::cDefaultStride)/LandscapeRecord::cDefaultStride;
      y_linear = (y-m_RecordList[i]->OffsetY()-y_idx*LandscapeRecord::cDefaultStride)/LandscapeRecord::cDefaultStride;

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
