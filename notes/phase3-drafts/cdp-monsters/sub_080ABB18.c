#include "global.h"
#include "scene_actor_reset.h"
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
void SceneActorControl_OnIdle(void);
static inline void clearHalf(u16 *p, u32 mask)
{
    *p = mask & *p;
}
static inline void setHalf(u16 *p, u32 mask)
{
    *p = mask | *p;
}
static inline void clearWord(u32 *p, u32 mask)
{
    *p = mask & *p;
}
// confidence=low: removes excess segments after damage and restores idle control.
#define TentacleActor_RecoverFromSegmentLoss sub_080ABB18
u32 TentacleActor_RecoverFromSegmentLoss(struct SceneActorRegistryResetView *p)
{
    struct Group *g = p->context;
    u16 *flags;
    s32 count;
    if (p->reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x19c);
    flags = &p->flags2D4;
    clearHalf(flags, 0xfffffeff);
    {
        u32 bit = 4;
        bit = bit | g->flags;
        g->flags = bit;
    }
    g->timer = 240;
    p->colliderA.responseValue += 30;
    g->flashTimer = 16;
    clearWord(&p->flags2D0, 0xffefffff);
    clearHalf(flags, -2);
    count = Div(p->health, g->healthStep);
    for (;;) {
        if (count >= g->activeCount) {
            clearHalf(flags, -33);
            p->field2B2 = 1;
            p->field2B0 = 0;
            p->field2B5 = 16;
            p->target = SceneActorControl_OnIdle;
            p->field2C8 = 0;
            clearWord(&p->flags114, -2);
            p->word104 = 0;
            p->state2A8 = 0;
            p->phase2A9 = 0;
            p->behavior2AA = 5;
            p->counter2AC = 0;
            p->field2B1 = 1;
            p->body.direction = p->tracking.heading;
            p->field2C2 = 64;
            return 1;
        }
        ActorTrail_RemoveLastSegment(p);
        count++;
    }
}
