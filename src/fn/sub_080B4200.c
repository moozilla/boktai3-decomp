#include "scene_actor_placement.h"
struct VornMessageActor {
    u8 unknown0[0x50];
    u32 identity;
    u8 unknown54[0x37C];
    u8 *context;
};
// confidence=low: category messages six/seven toggle the foreign-position notification latch.
void Vorn_ClearForeignPosition(struct SceneActorPlacementView *actor, const u16 *message)
{
    if (!SceneActorPlacement_IsUnavailable(actor)) {
        struct VornMessageActor *view = (void *)actor;
        if (message[1] != view->identity)
            view->context[0x6C0] = 0;
    }
}
