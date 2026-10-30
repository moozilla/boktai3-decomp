#include "global.h"
#include "scene_actor_reaction.h"

/* Sibling of Bat_ResumeSavedTarget (Bat_ResumeSavedTarget): after 60 frames it either resumes group
 * +0x48 (kind 0x1A, flag 0x80 set) or goes idle (kind 0x10) and routes to group +0x58;
 * before that it rewinds a mode-2 animation. */

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
    return actor->flags2D4 & mask;
}
static inline void Bat_SetControl(struct SceneActorRegistryResetView *actor, u32 kind,
                                  void *control)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
void SceneActorRoute_SetReachableDestination(struct SceneActorRegistryResetView *actor,
                                             struct ContactPosition *destination);
void Golem_ResumeSavedTarget(struct SceneActorRegistryResetView *actor, s32 frame)
{
    u8 *group = (u8 *)&actor->group.common;
    struct BatAnim *anim = &((struct BatDrawable *)actor->drawable)->anim;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        actor->field2B3 = 0;
    }
    if (frame > 60) {
        u16 blocked;
        if (!actor->field2B3)
            return;
        actor->field2B0 = 1;
        blocked = Bat_MaskFlags(actor, 128);
        if (blocked) {
            void *target;
            SceneActor_SetPositionXZ(&actor->body.id, (*(struct BatRoutePoint **)(group + 0x1FC))->x,
                                     (*(struct BatRoutePoint **)(group + 0x1FC))->z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            target = *(void **)(group + 0x48);
            Bat_SetControl(actor, 0x1A, target);
            {
                u32 behavior = 12;
                actor->state2A8 = 0;
                actor->phase2A9 = 0;
                actor->behavior2AA = behavior;
                actor->counter2AC = 0;
                actor->field2B1 = 1;
            }
            {
                u16 *flags = (u16 *)(group + 0x40);
                u16 mask = 4;
                *flags = mask | *flags;
            }
            actor->field2C2 = 0x20;
        } else {
            SceneActor_SetPositionXZ(&actor->body.id, (*(struct BatRoutePoint **)(group + 0x1FC))->x,
                                     (*(struct BatRoutePoint **)(group + 0x1FC))->z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            {
                void *target = SceneActorControl_OnIdle;
                Bat_SetControl(actor, 0x10, target);
            }
            {
                u32 phase = 2;
                actor->state2A8 = 0;
                actor->phase2A9 = phase;
                actor->behavior2AA = 0;
                actor->counter2AC = 0;
                actor->field2B1 = 1;
            }

            actor->flags2CC |= 16;
            SceneActorRoute_SetReachableDestination(actor,
                                                    (struct ContactPosition *)(group + 0x58));
        }
    } else {
        if (anim->mode == 2) {
            u16 f = anim->frame;
            u8 loop = anim->loop;
            if (f == (loop != 0) && loop != 1)
                anim->frame = 0;
        }
    }
}
