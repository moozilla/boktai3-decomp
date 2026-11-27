#include "scene/motion/scene_actor_chase_reaction.h"
#include "scene/motion/scene_actor_availability.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/motion/scene_actor_chase.h"
#define FIELD(p, o, T) (*(T *)((u8 *)(p) + (o)))
extern const s16 gUnk_086149C4[];
void Centipede_UpdateSegmentPositions(void *, s32, s32);
void Centipede_SelectWanderRoute(void *);
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
void Centipede_UpdateKnockbackSlide(struct ChaseActor *p)
{
    if (Chase_TakeArrived(p)) {
        struct ContactPosition *velocity;
        u32 angle;
        u8 *direction;
        u32 factor, raw;
        SceneActorChase_SetControl(p, p->base.onStanding, 0);
        direction = &p->base.reactionDirection;
        raw = FIELD(p, 0x174, u16);
        velocity = &p->base.secondaryMotion;
        angle = *direction;
        factor = raw >> 3;
        velocity->x = Scale(gUnk_086149C4[(angle + 64) & 255] * factor);
        velocity->y = 0;
        velocity->z = Scale(gUnk_086149C4[angle] * factor);
        p->base.secondaryMotion.y = 0x20;
        p->base.field2E8 = 2;
    }
    Centipede_UpdateSegmentPositions(p, 6, 0x1000);
    {
        if (Flags_MaskedWord(16, &p->base.statusFlags)) {
            Centipede_SelectWanderRoute(p);
            SceneActorChase_SetControl(p, p->base.onAlternateEntry, 1);
            SceneActorChase_SetState(p, 0, 0, 2);
            Sound_PlaySong(983);
        } else {
            p->base.counter2AC++;
        }
    }
}
