/*---------------------------------------------------------------------------
 Author:  thoronador
 Date:    2010-01-30
 Purpose: ObjectBase Singleton class
          holds information about all distinct objects in the game

 History:
     - 2009-07-01 (rev 101) - initial version (by thoronador)
     - 2009-07-02 (rev 102) - checks for empty ID and mesh in addObject()
     - 2009-07-06 (rev 103) - possibility to work with error mesh added
     - 2009-07-13 (rev 104) - SaveToStream() added
     - 2009-07-15 (rev 105) - LoadFromStream() added
     - 2009-07-24 (rev 111) - GetFirst() and GetEnd() added
     - 2009-07-31 (rev 113) - deleteObject() added
     - 2009-09-24 (rev 131) - } small fixes
     - 2009-09-27 (rev 132) - }
     - 2010-01-30 (rev 161) - obsolete load/save functions removed
                            - documentation updated

 ToDo list:
     - ???

 Bugs:
     - If you find one (or more), then tell me please. I'll try to fix it as
       soon as possible.
 --------------------------------------------------------------------------*/

#ifndef OBJECTBASE_H
#define OBJECTBASE_H

#include <map>
#include <string>
#include <fstream>

namespace Dusk
{
  /* class ObjectBase
           This class' purpose is to hold the IDs of all distinct static objects
           in the game and their corresponding mesh name. "Static objects" in
           the above context are all non-moving objects which cannot be picked
           up by the player or a NPC. Examples of static objects would be walls,
           trees, furniture (non-movable) and so on.
           Things like an apple or a bottle are NOT "static objects", they are
           "items", because they can be picked up and/or used by the player. See
           the class ItemBase for items.


           Think of ObjectBase as a sort of look-up table, e.g.:

             ID  |  Mesh
           ------+------------------
           tree  | tree_oak.mesh
           Chair | wooden_chair.mesh
            ...  | ...
  */
  class ObjectBase
  {
    public:
      /* destructor */
      virtual ~ObjectBase();

      /* singleton access */
      static ObjectBase& GetSingleton();

      /* returns true, if an object with the given ID exists */
      bool hasObject(const std::string& IDOfObject) const;

      /* adds object with given ID and mesh. If an object with that ID already
         exists, it will be overwritten. If ID or mesh is an empty string,
         nothing will be done.
      */
      void addObject(const std::string& ID, const std::string& Mesh);

      /* tries to delete the object with the given ID and returns true if such
         an object was present (before deleting it, that is)
      */
      bool deleteObject(const std::string& ID_of_Object);

      /* deletes ALL objects, use with caution (or not at all) */
      void ClearAllObjects();

      /* returns the number of objects which are currently present */
      unsigned int NumberOfObjects() const;

      /* returns the mesh path of the given object, or an empty string, if no
          object with that ID is present. However, if UseMarkerOnError is set
          to true (default), this function will return the path to a predefined
          error mesh instead.
      */
      std::string GetMeshName(const std::string& ID, const bool UseMarkerOnError=true) const;

      /* tries to save all objects to the stream and returns true on success */
      bool SaveToStream(std::ofstream& Stream) const;

      /* tries to read the next object from stream and returns true, if successful */
      bool LoadFromStream(std::ifstream& Stream);

      /* helper functions to access internal map iterators - not used in-game,
         only used by Editor application.
      */
      std::map<std::string, std::string>::const_iterator GetFirst() const;
      std::map<std::string, std::string>::const_iterator GetEnd() const;
    private:
      /*  constuctor */
      ObjectBase();

      /* empty copy constructor - singleton pattern */
      ObjectBase(const ObjectBase& op){}
      std::map<std::string, std::string> m_ObjectList;
  };//class

}//namespace

#endif
