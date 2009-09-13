#ifndef LIGHTBASE_H
#define LIGHTBASE_H

#include <string>
#include <map>
#include <fstream>

namespace Dusk
{

  struct LightRecord
  {
    public:
      float red,green,blue;
      float radius;
      bool IsBlack() const;
      void Normalise();
      static LightRecord GetBlack(const float d=0.0f);
      static LightRecord GetRed(const float d=0.0f);
      static LightRecord GetGreen(const float d=0.0f);
      static LightRecord GetBlue(const float d=0.0f);
  };

  class LightBase
  {
    public:
      virtual ~LightBase();
      static LightBase& GetSingleton();
      void addLight(const std::string& ID, const float r, const float g, const float b, const float radius);
      void addLight(const std::string& ID, const LightRecord& record);
      bool hasLight(const std::string& ID) const;
      LightRecord getLightData(const std::string& ID);
      bool deleteLight(const std::string& ID);
      void ClearAllData();
      unsigned int NumberOfLights() const;
      bool SaveAllToStream(std::ofstream& out_stream);
      bool LoadRecordFromStream(std::ifstream& in_stream);
    private:
      LightBase();
      LightBase(const LightBase& op){} //copy constructor
      std::map<std::string, LightRecord> m_LightList;
  };//class

}//namespace

#endif // LIGHTBASE_H
