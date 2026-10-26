#include "global.h"
#include "game_state.h"

/* tentative: adds or removes an entry in the 8-entry unlock list */

s32 GameState_GetUnlockEntry(s32);
s32 GameState_HasUnlockedInventoryEntry(s32);
void Sound_PlaySong(u32);

#define UNLOCK_SLOT(i) (*(s16 *)((u8 *)gGameState + (i) * 2 + 0x78))
#define UNLOCK_CUR     (*(s16 *)((u8 *)gGameState + 0x50))

s32 GameState_ToggleUnlockEquipSlot(u8 *p)
{
    s32 buf[8];
    s32 slot, i, n, sel;

    slot = p[2] * 3 - 2;
    slot += p[1];
    if (!GameState_GetUnlockEntry(slot))
        goto fail;
    if (GameState_HasUnlockedInventoryEntry(slot)) {
        n = 0;
        for (i = 0; i <= 7; i++) {
            buf[i] = UNLOCK_SLOT(i);
            if (buf[i] > -1)
                n++;
        }
        if (n > 1) {
            n = 0;
            for (i = 0; i <= 7; i++) {
                if (buf[i] == slot) {
                    s16 *cur = &UNLOCK_CUR;
                    if (*cur > 7) {
                        if (n == *cur - 8)
                            *cur = 8;
                        else if (n < *cur - 8)
                            (*cur)--;
                    }
                } else {
                    UNLOCK_SLOT(n) = buf[i];
                    n++;
                }
            }
            for (; n <= 7; n++)
                UNLOCK_SLOT(n) = -1;
            Sound_PlaySong(0xdd);
        } else
            goto fail;
    } else {
        n = 0;
        sel = -1;
        for (i = 0; i <= 11; i++) {
            if (i == slot) {
                if (n > 7)
                    goto fail;
                buf[n] = i;
                sel = n + 8;
                n++;
            } else if (GameState_HasUnlockedInventoryEntry(i)) {
                if (n > 7)
                    goto fail;
                buf[n] = i;
                n++;
            }
        }
        if (sel < 0) {
        fail:
            Sound_PlaySong(0x192);
            return 0;
        }
        for (; n <= 7; n++)
            buf[n] = -1;
        for (i = 0; i <= 7; i++)
            UNLOCK_SLOT(i) = buf[i];
        UNLOCK_CUR = sel;
        Sound_PlaySong(0xdd);
    }
    return 1;
}
