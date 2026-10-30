#include "global.h"
// MATCHING_CONCESSION: register-pinning
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
    u16 *savedFlags;
    s32 count;
    u8 *initialCount;
    u32 *motionCounter;
    void **control;
    u8 *direction, *state, *phase, *behavior;
    u32 *stateCounter;
    u8 *done, *savedDirection, *queued, *prepared, *kind;
    u16 *motionTimer;
    register u32 *animationFlags asm("r9");
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
    *(u16 *)((u8 *)p + 0x178) += 30;
    g->flashTimer = 16;
    clearWord(&p->flags2D0, 0xffefffff);
    clearHalf(flags, -2);
    count = Div(p->health, g->healthStep);
    initialCount = &g->activeCount;
    savedFlags = flags;
    prepared = &p->field2B2;
    done = &p->field2B0;
    kind = &p->field2B5;
    control = &p->target;
    motionTimer = &p->field2C8;
    animationFlags = &p->flags114;
    motionCounter = &p->word104;
    state = &p->state2A8;
    phase = &p->phase2A9;
    behavior = &p->behavior2AA;
    stateCounter = &p->counter2AC;
    queued = &p->field2B1;
    savedDirection = &p->tracking.heading;
    direction = &p->body.direction;
    if (count < *initialCount)
        do {
            ActorTrail_RemoveLastSegment(p);
            count++;
        } while (count < g->activeCount);
    {
        u32 nextFlags;
        u8 zero8;
        register u32 zero asm("r3");
        void *callback;
        u32 code;
        u8 one;
        nextFlags = -33;
        {
            u16 *cursor = savedFlags;
            nextFlags &= *cursor;
        }
        zero8 = 0;
        zero = 0;
        {
            u16 *cursor = savedFlags;
            *cursor = nextFlags;
        }
        callback = (void *)SceneActorControl_OnIdle;
        code = 16;
        one = 1;
        *prepared = one;
        *done = zero8;
        *kind = code;
        *control = callback;
        {
            register u16 *timer asm("r1") = motionTimer;
            *timer = zero;
        }
        {
            u32 mask = -2;
            register u32 *cursor asm("r5");
            cursor = animationFlags;
            *cursor = mask & *cursor;
        }
        {
            register u32 *cursor asm("r0") = motionCounter;
            asm("" : : "r"(cursor));
            *cursor = zero;
        }
        {
            u32 behaviorCode = 5;
            asm("" : : "r"(behaviorCode));
            *state = zero8;
            *phase = zero8;
            *behavior = behaviorCode;
        }
        *stateCounter = zero;
        *queued = one;
        *direction = *savedDirection;
    }
    p->field2C2 = 64;
    return 1;
}
