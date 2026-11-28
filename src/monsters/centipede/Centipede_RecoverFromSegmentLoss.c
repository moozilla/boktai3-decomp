#include "global.h"
#include "monsters/centipede/centipede_body.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/motion/scene_actor_chase.h"
void Sound_PlaySong(u32);
u8 ActorTrail_RemoveLastSegment(void *);
// confidence=low: removes excess segments after damage and restores idle control.
static inline void ClearHalf(u16 *p, u32 mask)
{
    *p = mask & *p;
}
static inline void ClearWord(u32 *p, u32 mask)
{
    *p = mask & *p;
}
static inline u16 *MotionFlags(struct ChaseActor *actor)
{
    return &actor->base.motionFlags;
}
static inline void ClearMotionFlags(struct ChaseActor *actor, u32 mask)
{
    actor->base.motionFlags = mask & actor->base.motionFlags;
}
// gcse's PRE computes the field addresses of the idle-control stores above the trail loop
// (its stack slots follow the hash-table order of a 65-bucket table), and cse2 turns the
// motion-flags address into a copy of the one the head computed through MotionFlags.
u32 Centipede_RecoverFromSegmentLoss(struct ChaseActor *p)
{
    struct TentacleBody *body = p->base.context;
    s32 count;
    if (p->base.reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x19c);
    ClearHalf(MotionFlags(p), ~0x100);
    {
        u16 bit = 4;
        bit = bit | body->attackFlags;
        body->attackFlags = bit;
    }
    body->hitReactionTimer = 240;
    {
        u16 value = p->base.colliderA.responseValue + 30;
        p->base.colliderA.responseValue = value;
    }
    body->unknown8F4 = 16;
    ClearWord(&p->base.statusFlags, ~0x100000);
    ClearHalf(MotionFlags(p), ~1);
    count = Div(p->base.health, body->healthPerSegment);
    while (count < body->spreadStage) {
        ActorTrail_RemoveLastSegment(p);
        count++;
    }
    ClearMotionFlags(p, ~0x20);
    Chase_SetCtl(p, SceneActorControl_OnIdle, 16);
    Chase_SetBehavior(p, 5);
    p->base.body.direction = p->base.tracking.heading;
    p->base.field2C2 = 64;
    return 1;
}
