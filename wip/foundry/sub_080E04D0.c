#define SceneActorAnimation_TickEntryEleven sub_080E04D0
#include "global.h"
#include "scene_actor_registry.h"
#include "scene_actor_animation.h"
s32 Div(s32, s32);
void sub_08220D44(void *, u32);
void sub_08220F00(void *, u32);
static inline u8 TakePrepared(struct SceneActorRegistryActor *actor)
{
    if (actor->field2B2) {
        actor->field2B2 = 0;
        return 1;
    }
    return 0;
}
static inline u32 TestFlags(struct SceneActorRegistryActor *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
void sub_080E04D0(struct SceneActorRegistryActor *actor)
{
    u32 saved;
    u16 *parameter;
    if (TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 11);
        actor->field2B3 = 0;
    }
    saved = *(u16 *)((u32)actor + 0x2C2);
    if (TestFlags(actor, 0x80))
        *(u16 *)((u32)actor + 0x2C2) = Div(64, 3);
    if (TestFlags(actor, 0xA02F)) {
        parameter = (u16 *)((u8 *)actor + 0x2C2);
        *parameter = 64;
    } else {
        parameter = (u16 *)((u8 *)actor + 0x2C2);
        if (!*parameter)
            *parameter = 64;
    }
    if (actor->drawableKind == 0)
        sub_08220D44((u8 *)actor->drawable + 0x48, *parameter);
    else
        sub_08220F00((u8 *)actor->drawable + 0x20, *parameter);
    actor->field2C2 = saved;
    if (*((u8 *)actor + 0x2C2 - 15))
        actor->field2B0 = 1;
}
