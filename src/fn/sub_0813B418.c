#include "global.h"
struct ResponseActor {
    u8 pad0[0x448];
    u32 flags;
};
s32 Div(s32, s32);
#define HAS_FLAG(actor, mask) ((actor) && ((actor)->flags & (mask)))
// confidence=low: flag 0x1000 halves the value, flag 0x800 doubles it, both leave it unchanged.
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// The ROM re-tests the actor for NULL where agbcc threads the jump here.
inline static u32 Arg1_1(struct ResponseActor *actor)
{
    return actor->flags & 0x1000;
}

s32 ActorResponse_Apply(struct ResponseActor *actor, s32 value)
{
    if (actor && (actor->flags & 0x1000)) {
        if (actor->flags & 0x800)
            return value;
    }
    if (actor && Arg1_1(actor))
        value -= Div(value, 2);
    else if (actor && (actor->flags & 0x800))
        value <<= 1;
    return value;
}

#else
INCLUDE_ASM("asm/nonmatching", ActorResponse_Apply);
#endif
