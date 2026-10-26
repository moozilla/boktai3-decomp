#include "global.h"
#include "game_state.h"
#include "presentation_entry_list.h"
void Presentation_InitListValues(struct PresentationEntryList *panel)
{
    s32 i;
    if (panel->source->mode != 6) {
        if (*(s16 *)gGameState->unk42 > 196)
            *(s16 *)gGameState->unk42 = 0;
        panel->entryCount = *(u16 *)gGameState->unk42;
    } else {
        panel->entryCount = 0;
    }
    {
        u16 zero = 0;
        for (i = 2; i >= 0; i--)
            panel->pendingValues[i] = zero;
    }
}
