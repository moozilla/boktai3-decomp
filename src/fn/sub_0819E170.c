#include "global.h"
#define ActorAction_WaitThenChangeBehavior sub_0819E170
struct ActorActionSequence {
    u8 unknown0[0x90];
    u32 timer90;
    u8 unknown94[0xA2 - 0x94];
    u8 flagA2;
    u8 unknownA3;
    u32 counterA4;
    u8 unknownA8[2];
    u8 animation, nextAnimation, mode, submode, previousMode, animationDone, entered, pending;
    u8 unknownB2[0xC4 - 0xB2];
    u32 timer;
    u8 unknownC8[0x100 - 0xC8];
    void (*callback)(void);
};
void sub_081A1D18(void);
static inline u8 ConsumeEntry(struct ActorActionSequence *p)
{
    if (p->entered) {
        p->entered = 0;
        p->animationDone = 0;
        return 1;
    }
    return 0;
}
void sub_0819E170(struct ActorActionSequence *p)
{
    u32 *timer;
    if (ConsumeEntry(p)) {
        u32 animation = 0x24;
        p->animation = animation;
    }
    timer = &p->timer;
    if (++*timer > 60) {
        u32 one = 1;
        u32 zero = 0;
        u32 previous = 4;
        p->mode = one;
        p->submode = zero;
        p->previousMode = previous;
        *timer = zero;
        p->entered = one;
        {
            void (*callback)(void) = sub_081A1D18;
            u32 animation = 0x25;
            u32 offset = 0xAB;
            *((u8 *)p + offset) = animation;
            offset += 0x55;
            *(void (**)(void))((u8 *)p + offset) = callback;
        }
        p->timer90 = zero;
        p->pending = one;
        p->flagA2 = zero;
        p->counterA4 = zero;
    }
}
