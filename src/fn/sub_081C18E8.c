#include "global.h"
#define AnimatedObject_AdvanceThreeFrameCycle sub_081C18E8
/* Tentative: selects one of three frames from a wrapping fixed-point phase. */
struct AnimatedObject {
    u8 pad0[0x6c];
    struct {
        u8 pad0[0x10];
        u16 frame;
    } sprite;
    u8 pad80[0x34];
    u16 phase;
    u16 phaseStep;
};
void sub_081C18E8(struct AnimatedObject *p)
{
    struct AnimatedObjectSprite {
        u8 pad0[0x10];
        u16 frame;
    };
    u32 frame = p->phase >> 3;
    struct AnimatedObjectSprite *sprite = (void *)((u8 *)p + 0x6c);
    sprite->frame = frame;
    p->phase += p->phaseStep;
    if (p->phase > 23)
        p->phase = 0;
}
