#include "global.h"

struct SceneActorTimedValue {
    u8 unknown0[0x60];
    u16 timer;
    u8 unknown62[2];
    u32 value;
};

// confidence=low: counts down a timer at +0x434 and clears the word at +0x438 once it has expired.
void sub_08137ADC(u8 *actor)
{
    struct SceneActorTimedValue *t = (struct SceneActorTimedValue *)(actor + 0x3d4);
    if (t->timer)
        t->timer--;
    else
        t->value = 0;
}
