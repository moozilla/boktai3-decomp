#include "challenge_menu.h"

void ChallengeMenu_Update(struct ChallengeMenu *menu)
{
    sub_081C58BC(menu);
    if (menu->stateCallback)
        Callback_InvokeActor(menu, menu->stateCallback);
}
