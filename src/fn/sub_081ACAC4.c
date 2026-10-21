// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct Vanargand {
    u8 pad0[0x9c];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[7];
    u8 finished;
    u8 padB0;
    u8 pending;
    u8 padB2[0xf18 - 0xb2];
    u32 counterF18;
    u32 counterF1C;
    u16 delayF20;
    u16 delayF22;
    u8 padF24[0x10];
    s32 modeF34;
};
s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);
void Vanargand_SetPendingCue(void *, u16);
void Vanargand_WaitForActionRelease(struct Vanargand *p)
{
    struct Vanargand *actor = p;
    u32 flag = 4;
    p->flags |= flag;
    Vanargand_SetPendingCue(p, 3);
    if (p->pending) {
        p->pending = 0;
        if (p->modeF34 <= 2) {
            register u16 delay asm("r1") = 6;
            u16 *field;
            p->counterF1C = 0;
            field = &p->delayF20;
            *field = delay;
            field++;
            *field = delay;
            p->counterF18 = 0;
        }
        p->phase = 0;
        p->timer = 0;
    }
    if (!p->phase && !actor->delayF20) {
        s32 busy = Player_IsSlotInActionTwelveOrThirtyEight(0);
        if (!busy) {
            p->finished = 1;
            p->phase = 1;
            p->timer = busy;
        }
    }
}
