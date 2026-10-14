// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
s32 TetherActor_SetHeadingEntry(struct TetherActor *, u32);
s32 TetherActor_SetCardinalEntry(struct TetherActor *, u32);
s32 TetherActor_SetEntry(struct TetherActor *input, u32 inputBase)
{
    struct TetherActor *actor = input;
    u32 base = inputBase;
    register u32 kind asm("r0") = actor->kind;
    s32 result;
    if (kind != 9) {
        register u32 entry asm("r1") = kind;
        asm("" : "+r"(entry));
        entry = base + entry;
        result = TetherActor_SetHeadingEntry(actor, entry);
    } else
        result = TetherActor_SetCardinalEntry(actor, base);
    return result;
}
