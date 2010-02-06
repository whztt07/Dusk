/*---------------------------------------------------------------------------
 Author:  thoronador
 Date:    2010-02-05
 Purpose: QuestLog Singleton class
          holds all quest entries the player actually gets during the game
          (This is not to be confused with the class Journal!)

 History:
     - 2010-02-05 (rev 163) - initial version (by thoronador)

 ToDo list:
     - ???

 Bugs:
     - If you find one (or more), then tell me please. I'll try to fix it as
       soon as possible.
 --------------------------------------------------------------------------*/

#ifndef QUESTLOG_H
#define QUESTLOG_H

#include <fstream>
#include <map>
#include <string>
#include <vector>

namespace Dusk
{
  struct QLogEntry
  {
    std::string questID;
    unsigned int index;
  };

  class QuestLog
  {
    public:
      /* singleton access */
      static QuestLog& GetSingleton();

      /* destructor */
      virtual ~QuestLog();

      /* adds the quest entry for quest questID and index index to the QuestLog

         return value:
             returns true, if the entry was added. Returns false, if not. This
             will also return false, if that entry is already present.
      */
      bool addQuestEntry(const std::string& questID, const unsigned int index);

      /* returns true, if at least one entry for quest questID is present */
      bool hasQuest(const std::string& questID) const;

      /* returns true, if the given quest is already finished */
      bool isQuestFinished(const std::string& questID) const;

      /* returns true, if the entry of quest questID and index index is present */
      bool hasQuestEntry(const std::string& questID, const unsigned int index) const;

      /* returns the vector of all quests that have been finished yet */
      std::vector<std::string> listFinishedQuests() const;

      /* returns the vector of all quests that have NOT been finished yet */
      std::vector<std::string> listActiveQuests() const;

      /* returns a vector of all quest entries in the QuestLog in chronological
         order, skipping offset entries and returning not more than limit
         entries

         parameters:
             offset - the number of entries to skip (zero, if you want to start
                       at the beginning)
             limit  - the maximum number of entries to return. Default is 30.
                      If limit is set to zero, all available entries will be
                      returned.
      */
      std::vector<QLogEntry> listQuestEntries(const unsigned int offset, unsigned int limit=30) const;

      /* deletes all data, producing an empty QuestLog */
      void ClearAllData();

      /* returns the number of quest entries in QuestLog */
      unsigned int NumberOfQuestEntries() const;

      /* tries to save data to stream and returns true on success, false otherwise */
      bool SaveToStream(std::ofstream& output) const;

      /* tries to load all data from stream and returns true on success, false
         otherwise
      */
      bool LoadFromStream(std::ifstream& input);
    private:
      QuestLog();
      QuestLog(const QuestLog& op) {}

      /* vector which keeps track of all recieved quest entries in chronological
         order

         remarks:
             This is the only "real" data of the QuestLog class. All other data
             mambers (m_PresentEntries and m_FinishedQuests) are just for faster
             lookup of certain data bits.
      */
      std::vector<QLogEntry> m_TimeLine;

      /* map to allow fast(er) presence checks without going through the above
         vector

         remarks:
             The second unsigned int in the second map is the index of the
             QLogEntry structure within the vector m_TimeLine.
      */
      std::map<std::string, std::map<unsigned int, unsigned int> > m_PresentEntries;

      /* map for retrieving quest state without iterating through vector */
      std::map<std::string, bool> m_FinishedQuests;

  }; //class

} //namespace

#endif // QUESTLOG_H