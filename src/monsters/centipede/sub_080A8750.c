#include "scene/actor/scene_actor_chase_reaction.h"
#include "scene/actor/scene_actor_availability.h"
#include "scene/actor/scene_actor_reset.h"
extern const s16 gUnk_086149C4[];
void sub_080A9DB0(void *, s32, s32);
void TentacleActor_SelectWanderRoute(void *);
static inline u8 TakeEntry(struct ChaseActor *p)
{
    if (p->base.field2B1) {
        p->base.field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
void TentacleActor_UpdateKnockback(struct ChaseActor *p)
{
    if (TakeEntry(p)) {
        struct ContactPosition *velocity;
        u32 angle;
        u32 factor = 4;
        u8 *direction;
        SceneActorChase_SetControl(p, p->base.onStanding, 0);
        direction = &p->base.body.direction;
        velocity = &p->base.secondaryMotion;
        angle = *direction;
        velocity->x = Scale(gUnk_086149C4[(angle + 64) & 255] * factor);
        velocity->y = 0;
        velocity->z = Scale(gUnk_086149C4[angle] * factor);
        p->base.secondaryMotion.y = 0;
        p->base.field2E8 = 2;
    }
    sub_080A9DB0(p, 6, 0x1000);
    {
        if (Flags_MaskedWord(16, &p->base.statusFlags)) {
            TentacleActor_SelectWanderRoute(p);
            SceneActorChase_SetControl(p, p->base.onAlternateEntry, 1);
            SceneActorChase_SetState(p, 0, 0, 2);
        } else {
            p->base.counter2AC++;
        }
    }
}
