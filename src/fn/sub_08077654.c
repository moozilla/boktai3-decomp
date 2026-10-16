// MATCHING_CONCESSION: register-pinning
#include "scene_actor_placement.h"
static inline u8 SceneActorPlacement_TestAndSaveFlags(u32 mask, u16 *source, u16 **saved)
{
    u32 bits = *source & mask;
    *saved = source;
    if (bits)
        return 1;
    return 0;
}
u32 SceneActorPlacement_ConsumeCallbackFlag(struct SceneActorPlacementView *argument)
{
    struct SceneActorPlacementView *actor = argument;
    u8 unavailable;
    if (actor) {
        u32 mask = 0x40;
        if ((actor->flags2D4 & mask) || SceneActorPlacement_ReadFlags32(actor, 0x20000) ||
            actor->health <= 0)
            unavailable = 1;
        else
            unavailable = 0;
    } else
        unavailable = 0;
    asm("" : "+r"(unavailable));
    if (!unavailable && actor->flags2D6) {
        u8 pending;
        u16 *flags;
        u32 mask = 0x80;
        u16 *source;
        asm("" : "+r"(actor) : "r"(mask));
        source = &actor->flags2D6;
        pending = SceneActorPlacement_TestAndSaveFlags(mask, source, &flags);
        if (pending) {
            void *callback = actor->callback23C;
            if (callback)
                sub_0824923C(actor, callback);
            SceneActorPlacement_ClearFlags(flags, ~0x180);
            return 1;
        }
    }
    return 0;
}
