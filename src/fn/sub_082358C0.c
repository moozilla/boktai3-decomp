#include "global.h"

#ifdef NONMATCHING
// fuzzy: 92.5%
// fix.py best version (reprinted C)
// Draft: advance an 8-step blend of two positions, then switch handler.
// Open: ~1/~2 mask constants should share one register (movs/rsbs then subs #1),
// and the 0x50/0x51 byte stores' address setup is ordered differently.
s32 Div(s32, s32);
void Sound_PlaySong(u32);
void sub_08235794(u8 *);

#define F16(o) (*(u16 *)(p + (o)))
#define S16(o) (*(s16 *)(p + (o)))

void sub_082358C0(u8 *p)
{
    u16 *counter;
    u8 *q;
    *((u32 *)(p + 0x24)) &= ~1;
    counter = (u16 *)(p + 0x34e);
    (*counter)++;
    if ((*counter) > 7) {
        {
            u32 *d1 = (u32 *)(p + 0x4);
            d1[8] &= ~2;
        }
        *((void (**)(u8 *))(p + 0x350)) = sub_08235794;
        *counter = 0;
        Sound_PlaySong(0x284);
    } else {
        q = p + 0x51;
        *q = ((*counter) * 2) + 0x20;
        *(q - 1) = *q;
        *((u16 *)(p + 0x3c)) = Div(
            ((*((s16 *)(p + 0x7c))) * (*counter)) + ((*((s16 *)(p + 0x84))) * (8 - (*counter))), 8);
        *((u16 *)(p + 0x3e)) = Div(
            ((*((s16 *)(p + 0x7e))) * (*counter)) + ((*((s16 *)(p + 0x86))) * (8 - (*counter))), 8);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_082358C0);
#endif
