#define Vanargand_UpdateDelayedPartAnimation sub_081ACB60
#include "global.h"
struct VanargandPart {
    u8 pad0[0x48];
    u8 animation[0x10];
    void *data;
    u32 timer;
};
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
    u8 padB2[0x3fc-0xb2];
    struct VanargandPart part;
    u8 pad45C[0x500-0x45c];
    u16 partActive;
};
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void Vanargand_SetPendingCue(void *, u16);
void sub_081ACB60(struct Vanargand *p)
{
    u32 mask = 4;
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 4);
    if (p->pending) {
        p->pending = 0;
        p->phase = 0;
        p->timer = 0;
    }
    switch (p->phase) {
    case 0:
        if (++p->timer > 16) {
            p->phase = 1;
            p->timer = 0;
        }
        break;
    case 1:
        if (p->timer == 0) {
            struct VanargandPart *part = &p->part;
            Anim_SetSequence(part->animation, part->data, 0, 0, 0);
            p->partActive = 1;
            part->timer = 0;
        }
        if (++p->timer > 15) {
            u8 next = 2;
            u32 zero = 0;
            p->phase = next;
            p->timer = zero;
            p->finished = 1;
        }
        break;
    case 2:
    default:
        break;
    }
}
