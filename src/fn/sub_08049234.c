// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
#define TetherActor_SetEntry sub_08049234
#define TetherActor_SetHeadingEntry sub_08049180
#define TetherActor_SetCardinalEntry sub_080491C4
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
    }
    else
        result = TetherActor_SetCardinalEntry(actor, base);
    return result;
}
