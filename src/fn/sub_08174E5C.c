#define gGameState gUnk_02000710
#include "global.h"
#include "game_state.h"
#include "presentation_entry_list.h"
void Presentation_CommitListValues(struct PresentationEntryList *panel)
{
    s32 i;
    for (i = 0; i <= 2; i++) {
        ((u16 *)(gUnk_02000710->unk16 + 2))[i] += panel->pendingValues[i];
        panel->pendingValues[i] = 0;
    }
    *(u16 *)gUnk_02000710->unk42 = panel->entryCount;
}
