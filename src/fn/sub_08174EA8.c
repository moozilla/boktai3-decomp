#define Presentation_InitListValues sub_08174EA8
#define gGameState gUnk_02000710
#include "global.h"
#include "game_state.h"
#include "presentation_entry_list.h"
void sub_08174EA8(struct PresentationEntryList *panel)
{
    s32 i;
    if (panel->source->mode != 6) {
        if (*(s16 *)gUnk_02000710->unk42 > 196)
            *(s16 *)gUnk_02000710->unk42 = 0;
        panel->entryCount = *(u16 *)gUnk_02000710->unk42;
    } else {
        panel->entryCount = 0;
    }
    {
        u16 zero = 0;
        for (i = 2; i >= 0; i--)
            panel->pendingValues[i] = zero;
    }
}
