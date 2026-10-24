#include "global.h"
struct ResponseActor {
    u8 pad0[0x448];
    u32 flags;
};
s32 Div(s32, s32);
// confidence=low: flag 0x1000 halves the value, flag 0x800 doubles it, both leave it unchanged.
static inline u32 ResponseFlags_Test(const u32 *flags, u32 mask)
{
    return *flags & mask;
}

static inline u32 ResponseActor_GetHalfFlag(struct ResponseActor *actor)
{
    u32 flags;

    if (!actor)
        return 0;
    flags = ResponseFlags_Test(&actor->flags, 0x1000);
    return flags & 0x1000;
}

s32 ActorResponse_Apply(struct ResponseActor *actor, s32 value)
{
    if (actor && ResponseFlags_Test(&actor->flags, 0x1000)) {
        if (ResponseFlags_Test(&actor->flags, 0x800))
            return value;
    }
    if (ResponseActor_GetHalfFlag(actor))
        value -= Div(value, 2);
    else if (actor && ResponseFlags_Test(&actor->flags, 0x800))
        value <<= 1;
    return value;
}
