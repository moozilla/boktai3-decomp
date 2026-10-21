#include "global.h"

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
    do {
        result = value;
        goto done;
    } while (0);
flash:
    result = p->flashValue;
done:
    return result;
}
