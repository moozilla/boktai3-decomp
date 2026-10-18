#define SceneActorReaction_TickHiddenResponse sub_080B8D14
#define SceneActorReaction_SetProfileEntries  sub_080B7580
#include "global.h"
#include "scene_actor_control_helpers.h"
#include "scene_actor_script.h"
#include "scene_actor_animation.h"
void sub_080B5BEC(struct SceneActorRegistryResetView *);
void sub_080B7580(struct SceneActorRegistryResetView *);
void sub_080B7768(struct SceneActorRegistryResetView *);
s32 sub_080B8D14(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRegistryResetView *saved = actor;
    u32 mask = 0x4000;
    u32 offset = 0x2D4;
    u16 *field;
    field = (void *)((u8 *)actor + offset);
    if (*field & mask) {
        u32 mask = 0x10000;
        u32 *input;
        u32 *flags;
        u32 enabled;
        input = &((struct SceneActorScriptPropertiesView *)actor)->flags6DC;
        enabled = *input & mask;
        actor = (void *)input;
        flags = (void *)actor;
        if (SceneActorControl_TestFlags(enabled, ~0)) {
            u32 one = 1;
            if (saved->drawableKind == 0) {
                u32 *field = saved->drawable;
                *field |= one;
            } else {
                struct SceneActorAnimationTile *tile = (void *)((u8 *)saved->drawable + 0x20);
                tile->flags |= one;
            }
            {
                u32 mask = ~0xC000;
                *flags &= mask;
            }
        } else {
            sub_080B5BEC(saved);
        }
    } else {
        struct SceneActorScriptPropertiesView *context = (void *)actor;
        u16 *timer;
        sub_080B7580(actor);
        sub_080B7768(actor);
        timer = &context->timer6D8;
        if (*timer)
            --*timer;
    }
    return 1;
}
