#include "global.h"

// Advance an eight-step position blend, then switch handlers.
s32 Div(s32, s32);
void Sound_PlaySong(u32);
void sub_08235794(u8 *);

struct BlendActor {
    u8 pad00[0x24];
    u32 flags;
    u8 pad28[0x3c - 0x28];
    u16 x;
    u16 y;
    u8 pad40[0x50 - 0x40];
    u8 blend0;
    u8 blend1;
    u8 pad52[0x7c - 0x52];
    s16 targetX;
    s16 targetY;
    u8 pad80[4];
    s16 startX;
    s16 startY;
    u8 pad88[0x34e - 0x88];
    u16 counter;
    void (*handler)(u8 *);
};

void sub_082358C0(struct BlendActor *p)
{
    u16 *counter;
    u8 *q;
    s32 blend;
    p->flags &= ~1;
    counter = &p->counter;
    p->counter++;
    if (*counter > 7) {
        p->flags &= ~2;
        p->handler = sub_08235794;
        *counter = 0;
        Sound_PlaySong(0x284);
    } else {
        blend = *counter * 2 + 0x20;
        q = &p->blend1;
        *q = blend;
        *(q - 1) = *q;
        p->x = Div(p->targetX * *counter + p->startX * (8 - *counter), 8);
        p->y = Div(p->targetY * *counter + p->startY * (8 - *counter), 8);
    }
}
