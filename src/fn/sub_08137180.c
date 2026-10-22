#include "global.h"

s32 Callback_InvokeActor(void *, void *);

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))

static inline u8 ConsumeEntry(u8 *actor)
{
    if (actor[0x2b1]) {
        actor[0x2b1] = 0;
        return 1;
    }
    return 0;
}

static inline void SetBehavior(u8 *actor, u32 behavior)
{
    actor[0x2a8] = 0;
    actor[0x2a9] = 0;
    actor[0x2aa] = behavior;
    FIELD(actor, 0x2ac, u32) = 0;
    actor[0x2b1] = 1;
}

// confidence=low: scene actor wait state: on entry retargets the saved target; after 60 idle frames switches to behaviour 2.
void sub_08137180(u8 *actor)
{
    if (ConsumeEntry(actor)) {
        void *saved = FIELD(actor, 0x22c, void *);
        u32 zero = 0;
        actor[0x2b2] = 1;
        actor[0x2b0] = zero;
        actor[0x2b5] = zero;
        FIELD(actor, 0x21c, void *) = saved;
        FIELD(actor, 0x2c8, u16) = zero;
        {
            u32 *flags = &FIELD(actor, 0x114, u32);
            s32 mask = ~1;
            *flags &= mask;
        }
        FIELD(actor, 0x104, u32) = zero;
    }
    if (!(u8)Callback_InvokeActor(actor, FIELD(actor, 0x270, void *))) {
        if (FIELD(actor, 0x2ac, s32) > 59)
            SetBehavior(actor, 2);
        else
            FIELD(actor, 0x2ac, s32)++;
    }
}
