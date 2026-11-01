// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "engine.h"
#include "ram.h"
#include "player_cue_sequence.h"
struct Fragment {
    u32 flags;
    u8 active, unknown5[3];
    s8 scaleX, scaleY;
    u8 unknownA[2];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct ContactPosition position;
    u8 unknown24[8];
    struct DisplaySequenceState sequence;
    u8 collider[0x54];
    struct ContactPosition center, offset;
    u32 angle, angularSpeed;
    s16 direction;
    u16 unknownAA;
    s16 radius, scaleTimer;
    u32 unknownB0;
    s32 lifetime;
    u16 unknownB8;
    u8 state;
    s8 slot;
    u16 timer;
    u8 entered;
};
struct Pool {
    u8 pad[0x38];
    u32 used;
};
extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
s32 Div(s32, s32);
void FragmentEffect_SpawnRandomParticle(struct ContactPosition *);
void FragmentEffect_UpdateCollider(struct Fragment *);
void Sprite_Remove(void *);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline void Step(struct Fragment *sprite)
{
    PlayerCue_StepSequence(sprite, &sprite->sequence);
}
// confidence=low: F422 fragment state interpolates sprite scale on appearance
void FragmentEffect_GrowToFullSize(struct Pool *pool, struct Fragment *p)
{
    s16 *scaleTimer;
    u16 *timer;
    if (p->entered) {
        p->flags = (p->flags & ~1) | 2;
        p->scaleTimer = 30;
        p->timer = 0;
        p->entered = 0;
        p->scaleX = 1;
        p->scaleY = 1;
    }
    {
        u16 *frame = &p->timer;
        s32 remainder = Mod(*frame, 12);
        timer = frame;
        if (!remainder)
            FragmentEffect_SpawnRandomParticle(&p->position);
    }
    {
        s16 *initial = &p->scaleTimer;
        register s32 remaining asm("r1") = *initial;
        scaleTimer = initial;
        asm("" : "+r"(scaleTimer));
        if (remaining > 0) {
            p->scaleX += Div(64 - p->scaleX, *scaleTimer);
            p->scaleY += Div(64 - p->scaleY, *scaleTimer);
            (*scaleTimer)--;
        }
    }
    Step(p);
    {
        s32 remaining;
        u32 hold;
        remaining = *scaleTimer;
        if (remaining <= 0) {
            p->state = 2;
            p->flags &= ~2;
            p->entered = 1;
        } else {
            u16 *frame = timer;
            (*frame)++;
        }
    }
}
