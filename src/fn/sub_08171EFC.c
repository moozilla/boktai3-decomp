// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: bounces the discard icon before entering consumable discard selection.
#define PauseInventory_UpdateDiscardBounce sub_08171EFC
void Sound_PlaySong(u32);
void PauseInventory_BeginConsumableDiscardSelection(u8 *, u32);
struct S08171EFC {
    u8 pad0[0xA4E];
    u16 timer;
    u8 padA50[0x26A2 - 0xA50];
    u16 y;
};
void PauseInventory_UpdateDiscardBounce(struct S08171EFC *s)
{
    u16 *timer = &s->timer;
    s32 v;
    if (!*timer)
        Sound_PlaySong(0x10E);
    v = *timer + 1;
    *timer = v;
    if ((u16)v < 12) {
        if (((v & 0xFFFF) >> 1) & 1)
            s->y = 104;
        else
            s->y = 104 - ((20 - *timer) >> 3);
        return;
    }
    asm("" : "+r"(v));
    if ((u16)v < 16)
        s->y = 104;
    else
        PauseInventory_BeginConsumableDiscardSelection((u8 *)s, 0);
}
