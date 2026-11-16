#include "global.h"
#include "scene_actor_reaction.h"

struct BatRoutePoint {
    u8 unknown0[0x1C];
    s16 x;
    u8 unknown1E[2];
    s16 z;
};
struct BatAnim {
    u8 unknown0[7];
    u8 loop;
    u16 mode;
    u8 unknownA[4];
    u16 frame;
};
struct BatDrawable {
    u8 unknown0[0x48];
    struct BatAnim anim;
};
void SceneActor_SetPositionXZ(void *, s32, s32);
static inline u32 Bat_MaskFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->base.flags2D4 & mask;
}
static inline void Bat_SetControl(struct SceneActorRegistryResetView *actor, u32 kind,
                                  void *control)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
void Mummy_ResumeSavedTarget(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct BatAnim *anim = &((struct BatDrawable *)actor->base.drawable)->anim;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        actor->base.restart = 0;
    }
    if (frame > 42) {
        u8 *group = (u8 *)&actor->group.common;
        u16 blocked;
        if (!actor->base.restart) {
            return;
        }
        actor->base.field2B0 = 1;
        SceneActor_SetPositionXZ(&actor->base.body.id,
                                 (*(struct BatRoutePoint **)(group + 0x1FC))->x,
                                 (*(struct BatRoutePoint **)(group + 0x1FC))->z);
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        blocked = Bat_MaskFlags(actor, 128);
        if (blocked) {
            void *target = *(void **)(group + 0x48);
            Bat_SetControl(actor, 0x25, target);
            {
                u32 behavior = 12;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = behavior;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = 1;
            }
            *(u16 *)(*(u8 **)((u8 *)actor + 0x5D4) + 6) = 1;
            {
                u16 *flags = (u16 *)(group + 0x40);
                u32 mask = 4;
                *flags = (*(&mask)) | (*flags);
            }
        } else {
            void *target;
            u32 behavior = 7;
            actor->base.state2A8 = blocked;
            actor->base.phase2A9 = blocked;
            actor->base.behavior2AA = behavior;
            actor->base.counter2AC = blocked;
            actor->base.field2B1 = 1;
            target = actor->base.entryTarget228;
            Bat_SetControl(actor, 1, target);
            *(u16 *)(*(u8 **)((u8 *)actor + 0x5D4) + 6) = blocked;
            actor->base.field2C2 = Div(64, 3);
            {
                u32 mask = 16;
                u32 *flags = &actor->base.flags2CC;
                *flags |= mask;
            }
        }
    } else if (anim->mode == 2) {
        u16 frame = anim->frame;
        u32 loop = anim->loop;
        if (loop) {
            if (frame == (loop - 1)) {
                goto rewind;
            }
            return;
        } else if (frame) {
            return;
        }
    rewind:
        anim->frame = 0;
    }
}
