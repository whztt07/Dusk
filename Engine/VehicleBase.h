/*---------------------------------------------------------------------------
 Author:  thoronador
 Date:    2010-09-22
 Purpose: VehicleBase Singleton class
          Represents a look-up table for all distinct Vehicle objects
          within the game.

 History:
     - 2010-09-22 (rev 243) - initial version (by thoronador)
     - 2010-09-24 (rev 244) - mountpoint data and functions for retrieval added

 ToDo list:
     - ???

 Bugs:
     - No known bugs. If you find one (or more), then tell me please.
 --------------------------------------------------------------------------*/

#ifndef VEHICLEBASE_H
#define VEHICLEBASE_H

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <OgreVector3.h>

namespace Dusk
{

struct MountpointData
{
  Ogre::Vector3 offset;
  Ogre::Vector3 rotation;
};//struct

struct VehicleRecord
{
  std::string Mesh;
  std::string Name;
  float MaxSpeed;
  unsigned int MountpointCount;
  std::vector<MountpointData> Mountpoints;
}; //struct

class VehicleBase
{
  public:
    /* singleton access method */
    static VehicleBase& getSingleton();

    /* destructor */
    ~VehicleBase();

    /* adds a vehicle with the given ID and data.

       remarks:
           This function always succeeds, except for empty ID or mesh path, or
           some other invalid data. In that case, nothing is added.
           If a vehicle record with the same ID already exists, it is
           overwritten by the new data.

       parameters:
           ID   - ID of the vehicle
           data - data for that vehicle
    */
    void addVehicle(const std::string& ID, const VehicleRecord& data);

    /* returns true, if a vehicle with the given ID is present

       parameters:
           ID - the ID of the vehicle
    */
    bool hasVehicle(const std::string& ID) const;

    /* Returns the mesh of vehicle with given ID, if present. */
    std::string getVehicleMesh(const std::string& ID, const bool UseMarkerOnError=true) const;

    /* returns the name of the given vehicle, or an empty string if no vehicle
       with that ID is present

       parameters:
           ID - the ID of the vehicle
    */
    std::string getVehicleName(const std::string& ID) const;

    /* returns the maximum speed of the given vehicle, or zero if no vehicle
       with that ID is present

       parameters:
           ID - the ID of the vehicle
    */
    float getVehicleSpeed(const std::string& ID) const;

    /* returns the number of mountpoints for the given vehicle, or zero if no
       vehicle with that ID is present

       parameters:
           ID - the ID of the vehicle
    */
    unsigned int getVehicleMountpoints(const std::string& ID) const;

    /* returns the offset of a specified mountpoint for the given vehicle.
       If no vehicle of with that ID is present, the return value is undefined.

       parameters:
           ID  - the ID of the vehicle
           idx - zero-based index of the mountpoint

       remarks:
           The value of the parameter idx has to be less than the value
           returned by getVehicleMountpoints().
    */
    const Ogre::Vector3& getMountpointOffset(const std::string& ID, unsigned int idx) const;

    /* returns the rotation of a specified mountpoint for the given vehicle.
       If no vehicle of with that ID is present, the return value is undefined.

       parameters:
           ID  - the ID of the vehicle
           idx - zero-based index of the mountpoint

       remarks:
           The value of the parameter idx has to be less than the value
           returned by getVehicleMountpoints().
    */
    const Ogre::Vector3& getMountpointRotation(const std::string& ID, unsigned int idx) const;

    /* returns the data of a specified mountpoint for the given vehicle.
       If no vehicle of with that ID is present, or mountpoint is not within the
       acceptable range, the return value is undefined.

       parameters:
           ID  - the ID of the vehicle
           idx - zero-based index of the mountpoint

       remarks:
           The value of the parameter idx has to be less than the value
           returned by getVehicleMountpoints().
    */
    const MountpointData& getMountpointData(const std::string& ID, unsigned int idx) const;

    /* deletes a vehicle by ID and returns true, if a vehicle was deleted */
    bool deleteVehicle(const std::string& ID);

    /* deletes all present vehicles */
    void clearAll();

    /* returns the number of distinct vehicles */
    unsigned int getVehicleNumber() const;

    /* tries to save all vehicles to the stream and returns true on success

       parameters:
           OutStream - the output stream that is used to save the vehicle data
    */
    bool saveAllToStream(std::ofstream& OutStream) const;

    /* Loads one(!) single vehicle record from stream; returns true on success,
       false otherwise. The data of the last loaded vehicle is probably
       inconsistent after that function failed, so don't rely on it in that
       case.

       parameters:
           InStream - the input stream that is used to load the weapon data
    */
    bool loadNextVehicleFromStream(std::ifstream& InStream);
  private:
    /* constructor - private due to singleton pattern */
    VehicleBase();

    /* private, empty copy constructor - there shall be only one */
    VehicleBase(const VehicleBase& op) {}

    //list of vehicles
    std::map<std::string, VehicleRecord> m_Vehicles;

    MountpointData mp_null;
}; //class

} //namespace

#endif // VEHICLEBASE_H