/*---------------------------------------------------------------------------
 Authors: thoronador
 Date:    2010-01-20
 Purpose: Menu Singleton class
          Manages display of in game menues.

 History:
     - 2010-01-20 (rev 158) - initial version (by thoronador)
     - 2010-01-26 (rev 159) - startDialogueWithNPC() and nextDialogueChoice()
                              added to handle display of dialogue lines

 ToDo list:
     - get a better material or texture for dialogue menu background
     - implement more stuff (like visualizing the inventory?)
     - ???

 Bugs:
     - Untested. If you find a bug (or more), then tell me please.
 --------------------------------------------------------------------------*/

#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include "NPC.h"

namespace Dusk
{

  class Menu
  {
    public:
      /* Singleton access method */
      static Menu& GetSingleton();

      /* destructor */
      virtual ~Menu();

      /* returns true, if the dialogue menu is currently visible */
      bool isDialogueActive() const;

      /* starts dialogue with given NPC and shows dialogue menu. Returns true,
         if a conversation was started.

         remarks:
             Will return false if another dialogue is active. In this case the
             active dialogue will be continued as if this function was not
             called.

         parameters:
             who - pointer to the NPC we want to talk to
      */
      bool startDialogueWithNPC(NPC* who);

      /* tells the menu class which dialogue option the player has chosen and
         progresses the dialogue accordingly. Returns true, if chosenOption was
         a valid parameter.

         parameter:
             chosenOption - number of option the player has chosen
                            Zero indicates that the player wants to quit the
                            dialogue menu. One means the first option, two is
                            the second option, and so on.
      */
      bool nextDialogueChoice(const unsigned int chosenOption);

      /* maximum number of dialogue choices to display at one time */
      static const unsigned int cMaxDialogueOptions;
    private:
      Menu();
      Menu(const Menu& op) {}

      /* shows the dialogue menu

         parameters:
             first   - the line the NPC is speaking
             options - vector of dialogue lines the player can choose from

         remarks:
             Currently, every vector containing more than cMaxDialogueOptions
             options will not be shown completely. Only the first few choices
             will be displayed.
      */
      void showDialogue(const std::string& first, const std::vector<std::string>& options);

      /* makes the dialogue menu disappear */
      void hideDialogue();

      /* delete all dialogue lines, i.e. their respective overlay elements */
      void killDialogueOverlayLines();
      unsigned int m_DialogueLineCount;

      std::vector<std::string> m_OptionIDs; //IDs of current dialogue options
      NPC* m_DialoguePartner; //pointer to NPC talking in dialogue, or NULL

      /* name of overlay for dialogue

         remarks:
             Don't change this, or you will have to edit the overlay script, too.
      */
      static const std::string cDialogueOverlay;
  };

} //namespace

#endif // MENU_H