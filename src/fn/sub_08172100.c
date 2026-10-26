#include "global.h"
#include "game_state.h"

/* tentative: adds or removes an inventory slot from the 8-entry equipped list */

s32 SolarFruit_GetInventorySlot(s32);
s32 SolarFruit_IsInventorySlotEquipped(s32);
void Sound_PlaySong(u32);

#define FRUIT_SLOT(i) (*(s16 *)((u8 *)gGameState + (i) * 2 + 0x90))
#define FRUIT_CUR     (*(s16 *)((u8 *)gGameState + 0x54))

void SolarFruit_ToggleEquipSlot(u8 *p)
{
    s32 buf[8];
    s32 slot, i, n, sel;

    slot = p[2] * 2 + p[1];
    if (SolarFruit_GetInventorySlot(slot) == -1)
        goto fail;
    if (SolarFruit_IsInventorySlotEquipped(slot)) {
        n = 0;
        for (i = 0; i <= 7; i++) {
            buf[i] = FRUIT_SLOT(i);
            if (buf[i] > -1)
                n++;
        }
        if (n > 1) {
            n = 0;
            for (i = 0; i <= 7; i++) {
                if (buf[i] == slot) {
                    s16 *cur = &FRUIT_CUR;
                    if (n == *cur)
                        *cur = 0;
                    else if (n < *cur)
                        (*cur)--;
                } else {
                    FRUIT_SLOT(n) = buf[i];
                    n++;
                }
            }
            for (; n <= 7; n++)
                FRUIT_SLOT(n) = -1;
            Sound_PlaySong(0xdd);
        } else
            goto fail;
    } else {
        for (i = 0; i <= 7; i++)
            buf[i] = FRUIT_SLOT(i);
        n = 0;
        sel = -1;
        for (i = 0; i <= 7; i++) {
            if (i == slot) {
                if (n > 7)
                    goto fail;
                buf[n] = i;
                sel = n++;
            } else if (SolarFruit_IsInventorySlotEquipped(i)) {
                if (n > 7)
                    goto fail;
                buf[n] = i;
                n++;
            }
        }
        if (sel < 0) {
        fail:
            Sound_PlaySong(0x192);
        } else {
            for (; n <= 7; n++)
                buf[n] = -1;
            for (i = 0; i <= 7; i++)
                FRUIT_SLOT(i) = buf[i];
            FRUIT_CUR = sel;
            Sound_PlaySong(0xdd);
        }
    }
}
