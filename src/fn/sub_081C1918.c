#include "global.h"
#include "player.h"

#ifdef NONMATCHING
// fuzzy: 89.8%
// foundry draft (worker best 88.5%); what differs is mostly register allocation and instruction order
#define ControlPanel_UpdateGaugeIndicator sub_081C1918

s32 Div(s32, s32);
void Sound_PlaySong(u32 song);

void sub_081C1918(struct Player *p)
{
    u16 v = gUnk_02000580[0]->indicatorUpdateRate;
    s32 inc;

    if (v != 0)
        inc = Div(0x80, v);
    if (v == 0 || inc <= 0)
        inc = 1;

    p->indicatorStep = p->indicatorStep + inc;
    if (p->indicatorStep > 0xb)
        p->indicatorStep = p->indicatorStep - 0xc;
    p->position.x = p->indicatorStep >> 2;

    if (p->indicatorTickCounter == 0) {
        u32 seg = gUnk_02000580[0]->displaySegmentCount;
        if (seg <= 1)
            Sound_PlaySong(0x23A);
        else if (seg <= 3)
            Sound_PlaySong(0x23B);
        else if (seg <= 5)
            Sound_PlaySong(0x8f << 2);
        else
            Sound_PlaySong(0x23D);
    }

    p->indicatorTickCounter = p->indicatorTickCounter + 1;
    if (p->indicatorTickCounter > 9)
        p->indicatorTickCounter = 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081C1918);
#endif
