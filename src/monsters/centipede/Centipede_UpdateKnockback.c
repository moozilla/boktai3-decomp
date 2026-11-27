#include "scene/motion/scene_actor_chase_reaction.h"
#include "scene/motion/scene_actor_availability.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/motion/scene_actor_chase.h"
extern const s16 gUnk_086149C4[];
void Centipede_UpdateSegmentPositions(void *, s32, s32);
void Centipede_SelectWanderRoute(void *);
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
void Centipede_UpdateKnockback(struct ChaseActor *p)
{
    if (Chase_TakeArrived(p)) {
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
    Centipede_UpdateSegmentPositions(p, 6, 0x1000);
    {
        if (Flags_MaskedWord(16, &p->base.statusFlags)) {
            Centipede_SelectWanderRoute(p);
            SceneActorChase_SetControl(p, p->base.onAlternateEntry, 1);
            SceneActorChase_SetState(p, 0, 0, 2);
        } else {
            p->base.counter2AC++;
        }
    }
}
