#include "global.h"
#ifdef NONMATCHING
// fuzzy: 98.4%
// CSE reuses the first (u16)v extension for the "< 16" test; the ROM re-extends it there
void Sound_PlaySong(u32);
void PauseInventory_BeginConsumableDiscardSelection(u8 *, u32);
struct S08171EFC {
    u8 pad0[0xA4E];
    u16 timer;
    u8 padA50[0x26A2 - 0xA50];
    u16 y;
};
void sub_08171EFC(struct S08171EFC *s)
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
    if ((u16)v < 16)
        s->y = 104;
    else
        PauseInventory_BeginConsumableDiscardSelection((u8 *)s, 0);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08171EFC);
#endif
