#include "global.h"

#ifdef NONMATCHING
// fuzzy: 82.1%
// foundry draft (worker best 82.1%); what differs is mostly register allocation and instruction order
struct FlashingActorEffect {
    u8 unknown0[0xad2];
    u16 flashValue, flashTicks;
};
s32 ActorEffect_SelectFlashingValue(struct FlashingActorEffect *p, s32 value)
{
    s32 result;
    if (p->flashTicks) {
        u16 ticks = --p->flashTicks;
        if ((ticks >> 2) & 1)
            goto flash;
    }
    result = value;
    goto done;
flash:
    result = p->flashValue;
done:
    return result;
}
#else
INCLUDE_ASM("asm/nonmatching", ActorEffect_SelectFlashingValue);
#endif
