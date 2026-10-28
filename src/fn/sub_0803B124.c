#include "global.h"
#include "input.h"

// Tentative result points animation; input skips to the completed display.
struct P {
    u8 pad[0x18];
    u32 timer;
    u8 state, entered;
    u8 pad1E[10];
    s16 points;
};
int Mod(int, int);
void Sound_PlaySong(int);
void Anim_DrawDecimalDigits(void *, void *, int, int, int, int, int, int, int);

void WirelessResultPanel_UpdatePointsReveal(struct P *p)
{
    int value;
    u8 *drawing;
    if (p->entered)
        p->entered = 0;
    if (gPadState[1] & 0x30F)
        p->timer = 128;
    if (p->timer <= 64) {
        value = (p->points * p->timer) >> 6;
        if ((u32)value > 9999)
            value = 9999;
        if (value && Mod(p->timer, 2) == 0)
            Sound_PlaySong(0x2A5);
    } else {
        value = p->points;
    }
    drawing = (u8 *)p + 0x5F8;
    {
        int number = value;
        if (number > 9999)
            number = 9999;
        else if (number < 0)
            number = 0;
        Anim_DrawDecimalDigits(drawing, (u8 *)p + 0x30, number, 4, 51, 104, 96, 8, 0);
    }
    if (p->timer > 127) {
        p->state = 4;
        p->entered = 1;
        p->timer = 0;
    } else {
        p->timer++;
    }
}
