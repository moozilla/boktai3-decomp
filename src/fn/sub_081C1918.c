#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "player.h"

s32 Div(s32, s32);
void Sound_PlaySong(u32 song);

void ControlPanel_UpdateGaugeIndicator(struct Player *p)
{
    register u32 v asm("r0");
    s32 inc;
    {
        register u8 *b asm("r0") = (u8 *)gPlayers[0];
        register u32 off asm("r2");
        register u16 *q asm("r1");
        off = 0x524;
        asm("" : "+r"(off));
        q = (u16 *)(b + off);
        v = *q;
    }

    if (v != 0) {
        {
            u32 a = v;
            inc = Div(0x80, a);
        }
        if (inc <= 0)
            inc = 1;
    } else
        inc = 1;

    p->indicatorStep = p->indicatorStep + inc;
    if (p->indicatorStep > 0xb)
        p->indicatorStep = p->indicatorStep - 0xc;
    p->position.x = p->indicatorStep >> 2;

    if (p->indicatorTickCounter == 0) {
        u32 seg;
        {
            u8 *b = (u8 *)gPlayers[0];
            register u32 off asm("r2");
            off = 0x522;
            asm("" : "+r"(off));
            seg = *(u16 *)(b + off);
        }
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
