#include "global.h"
#include "engine.h"
s32 GameState_GetUnlockEntry(s32 index);
void GameState_SetUnlockSlot(s32 index, s32 value);
s32 GameState_HasUnlockedInventoryEntry(s32 value);
#define STATE_S16(offset) (*(s16 *)((u8 *)gGameState + (offset)))
#define SLOT78(i)         (*(s16 *)((u8 *)gGameState + (i) * 2 + 0x78))
#define SLOT68(i)         (*(s16 *)((u8 *)gGameState + (i) * 2 + 0x68))

void sub_0822E764(s32 a, s32 b)
{
    s32 entry = GameState_GetUnlockEntry(a);
    s32 i, n, sel;
    s32 list[8];
    s32 *p;

    GameState_SetUnlockSlot(a, GameState_GetUnlockEntry(b));
    GameState_SetUnlockSlot(b, entry);
    for (i = 0; i <= 7; i++) {
        if (SLOT78(i) == a)
            SLOT78(i) = b;
        else if (SLOT78(i) == b)
            SLOT78(i) = a;
    }
    n = 0;
    for (i = 0, p = list; i <= 11; i++) {
        if (GameState_HasUnlockedInventoryEntry(i)) {
            *p++ = i;
            n++;
        }
    }
    sel = -1;
    if (STATE_S16(0x50) > 7) {
        for (i = 0; i < n; i++) {
            if (SLOT68(STATE_S16(0x50)) == list[i]) {
                sel = i;
                break;
            }
        }
    }
    for (; n <= 7; n++)
        list[n] = -1;
    for (i = 0; i <= 7; i++)
        SLOT78(i) = list[i];
    if ((u32)sel <= 7)
        STATE_S16(0x50) = sel + 8;
}
