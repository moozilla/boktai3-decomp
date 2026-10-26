#include "global.h"
#include "game_state.h"
struct TextEntry {
    u8 pad0[0x370];
    u32 useSavedName;
    u8 pad374[0x20];
    const u8 *initialName;
};
u8 *PlayerName_GetBuffer(void);
void TextEntry_CopyInitialName(struct TextEntry *p, u8 *out)
{
    const u8 *src;
    s32 i;
    if (p->initialName)
        src = p->initialName;
    else if (!p->useSavedName)
        src = PlayerName_GetBuffer();
    else
        src = (u8 *)gGameState + 0x790;
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
