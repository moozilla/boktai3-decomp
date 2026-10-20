#include "scene_actor_chase.h"

struct AxeTrailSprite {
    u32 flags;
    u8 pad4[2];
    u8 field6;
};
void Axe_EmitMotionTrail(void *);
void Axe_UpdateReturningTrail(void);

void Axe_UpdateTrailFadeIn(struct SceneActorRegistryResetView *actor, s32 frame)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        struct AxeTrailSprite *sprite = actor->drawable;
        sprite->flags |= 2;
    }
    Axe_EmitMotionTrail(actor);
    {
        struct AxeTrailSprite *sprite = actor->drawable;
        if (sprite->field6 <= 0x13)
            sprite->field6++;
    }
    if (frame > 0x3C) {
        struct AxeTrailSprite *sprite = actor->drawable;
        sprite->field6 = 0x14;
        Chase_SetCtl(actor, Axe_UpdateReturningTrail, 0x28);
    }
}
