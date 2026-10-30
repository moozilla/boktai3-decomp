#include "global.h"
#include "scene_actor_reaction.h"

/* Bat variant of SceneActorReaction_ResumeSavedTarget (tentative): after 60 frames it
 * resumes either the saved route target (kind 1) or, when flag 0x80 is set, group +0x48
 * (kind 0x1A); before that it rewinds a mode-2 animation. */

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
void Bat_ResumeSavedTarget(struct SceneActorRegistryResetView *actor, s32 frame)
{
    u8 *group = (u8 *)&actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 12);
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
        } else {
            void *target;
            SceneActor_SetPositionXZ(&actor->body.id, (*(struct BatRoutePoint **)(group + 0x1FC))->x,
                                     (*(struct BatRoutePoint **)(group + 0x1FC))->z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            target = actor->entryTarget228;
            Bat_SetControl(actor, 1, target);
            {
                u32 behavior = 7;
                actor->state2A8 = 0;
                actor->phase2A9 = 0;
                actor->behavior2AA = behavior;
                actor->counter2AC = 0;
                actor->field2B1 = 1;
            }
            {
                u32 mask = 16;
                actor->flags2CC |= mask;
            }
        }
    } else {
        struct BatAnim *anim = &((struct BatDrawable *)actor->drawable)->anim;
        if (anim->mode == 2) {
            u16 f = anim->frame;
            u8 loop = anim->loop;
            if (f == (loop != 0) && loop != 1)
                anim->frame = 0;
        }
    }
}
