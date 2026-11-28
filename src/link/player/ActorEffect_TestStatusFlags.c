#include "global.h"

struct ActorEffectStatus {
    u8 unknown0[0x38];
    u32 flags;
};

s32 ActorEffect_TestStatusFlags(void *unused0, void *unused1, struct ActorEffectStatus *s)
{
    u32 mask = 0x80;

    if (s->flags & mask)
        return 10;
    if (s->flags & 0x100)
        return 10;
    if (s->flags & 0x200)
        return 10;
    if (s->flags & 0x400)
        return 10;
    if (!(s->flags & 0x800))
        return 0;
    return 10;
}
