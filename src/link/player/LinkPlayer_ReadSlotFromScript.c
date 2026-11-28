#include "global.h"
#include "player/player.h"

s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);

void LinkPlayer_ReadSlotFromScript(struct Player *p)
{
    s32 slot = Script_SeekToKeyword(0x69);

    if (slot != 0)
        slot = Script_GetValue();
    p->slot = slot;
}
