#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_chase.h"
struct Group {
    u8 pad0[0x8d0];
    u16 flags;
    u8 pad8d2[4];
    u16 timer;
    u8 pad8d8[0x12];
    u8 activeCount, healthStep;
    u8 pad8ec[8];
    u16 flashTimer;
};
void Sound_PlaySong(u32);
u8 ActorTrail_RemoveLastSegment(void *);
// confidence=low: removes excess segments after damage and restores idle control.
#define TentacleActor_RecoverFromSegmentLoss sub_080ABB18
static inline void ClearHalf(u16 *p, u32 mask)
{
    *p = mask & *p;
}
static inline void ClearWord(u32 *p, u32 mask)
{
    *p = mask & *p;
}
static inline u16 *MotionFlags(struct SceneActorRegistryResetView *actor)
{
    return &actor->flags2D4;
}
static inline void ClearMotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D4 = mask & actor->flags2D4;
}
// gcse's PRE computes the field addresses of the idle-control stores above the trail loop
// (its stack slots follow the hash-table order of a 65-bucket table), and cse2 turns the
// motion-flags address into a copy of the one the head computed through MotionFlags.
u32 TentacleActor_RecoverFromSegmentLoss(struct SceneActorRegistryResetView *p)
{
    struct Group *g = p->context;
    s32 count;
    if (p->reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x19c);
    ClearHalf(MotionFlags(p), ~0x100);
    {
        u16 bit = 4;
        bit = bit | g->flags;
        g->flags = bit;
    }
    g->timer = 240;
    {
        u16 value = p->colliderA.responseValue + 30;
        p->colliderA.responseValue = value;
    }
    g->flashTimer = 16;
    ClearWord(&p->flags2D0, ~0x100000);
    ClearHalf(MotionFlags(p), ~1);
    count = Div(p->health, g->healthStep);
    while (count < g->activeCount) {
        ActorTrail_RemoveLastSegment(p);
        count++;
    }
    ClearMotionFlags(p, ~0x20);
    Chase_SetCtl(p, SceneActorControl_OnIdle, 16);
    Chase_SetBehavior(p, 5);
    p->body.direction = p->tracking.heading;
    p->field2C2 = 64;
    return 1;
}
