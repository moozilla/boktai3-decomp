#include "name_entry.h"
#include "game_state.h"

u8 *PlayerName_GetBuffer(void);
void TextEntry_CopyInitialName(struct NameEntryTask *p, u8 *out)
{
    const u8 *src;
    s32 i;
    if (p->initialName)
        src = p->initialName;
    else if (!p->slot)
        src = PlayerName_GetBuffer();
    else
        src = gGameState->playerName + 16;
    i = 0;
    do {
        u8 c = src[i];
        if (!c)
            break;
        out[i] = c;
        i++;
    } while (i <= 10);
    out[i] = 0;
}
