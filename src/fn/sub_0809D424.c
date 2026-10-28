#include "global.h"
#include "player.h"
void SceneActorControl_OnIdle(void);
struct S {
    u8 pad[0xDC];
    u8 extra[0x10];
    u8 pad3[0x104 - 0xEC];
    u32 at104;
    u8 pad4[0x114 - 0x108];
    u32 flags114;
    u8 pad5[0x21C - 0x118];
    void (*callback)(void);
    u8 pad6[0x2A8 - 0x220];
    u8 a, b, c, pad7;
    u32 count;
    u8 done, queued, taken;
    u8 pad8[2];
    u8 code;
    u8 pad9[0x2C8 - 0x2B6];
    u16 timer;
    u8 pad10[0x2D4 - 0x2CA];
    u16 hflags;
    u8 pad11[0x3D0 - 0x2D6];
    u8 *other;
};

void Lun_EnterIdleOrCopyCell(struct S *s)
{
    u32 mask;
    u8 *g;
    u16 zero;
    u16 zero_2;
    g = s->other;
    mask = 0x100;
    zero_2 = s->hflags;
    zero = zero_2 & mask;
    if (zero) {
        u16 v = *(u16 *)(g + 0x442);
        *(u16 *)(g + 0x444) = v;
    } else {
        void (*fn)(void) = SceneActorControl_OnIdle;
        u8 code = 0x10;
        u8 one;
        u8 zero8;
        u32 *flags;
        u8 *taken;
        u8 *extra;
        taken = &s->taken;
        one = 1;
        *taken = one;
        s->done = zero;
        s->code = code;
        s->callback = fn;
        zero8 = 0;
        s->timer = zero;
        flags = &s->flags114;
        {
            u32 clear = -2;
            *flags &= clear;
        }
        s->at104 = zero;
        s->a = zero8;
        s->b = one;
        s->c = zero8;
        s->count = zero;
        s->queued = one;
        extra = s->extra;
        {
            u32 bit = 8;
            bit |= extra[15];
            extra[15] = bit;
        }
    }
}
