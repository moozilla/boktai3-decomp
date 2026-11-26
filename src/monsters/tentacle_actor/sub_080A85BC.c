#include "engine/shared/scene_actor_chase_reaction.h"
#include "engine/shared/scene_actor_availability.h"
#include "engine/shared/scene_actor_reset.h"
#define FIELD(p, o, T) (*(T *)((u8 *)(p) + (o)))
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
void TentacleActor_UpdateKnockbackSlide(struct ChaseActor *p)
{
    if (TakeEntry(p)) {
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
    sub_080A9DB0(p, 6, 0x1000);
    {
        if (Flags_MaskedWord(16, &p->base.statusFlags)) {
            TentacleActor_SelectWanderRoute(p);
            SceneActorChase_SetControl(p, p->base.onAlternateEntry, 1);
            SceneActorChase_SetState(p, 0, 0, 2);
            Sound_PlaySong(983);
        } else {
            p->base.counter2AC++;
        }
    }
}
