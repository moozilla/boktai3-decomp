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
    if (p->entered) {
        p->flags = (p->flags & ~1) | 2;
        p->scaleTimer = 30;
        p->timer = 0;
        p->entered = 0;
        p->scaleX = 1;
        p->scaleY = 1;
    }
    if (!Mod(p->timer, 12))
        FragmentEffect_SpawnRandomParticle(&p->position);
    if (p->scaleTimer > 0) {
        p->scaleX += Div(64 - p->scaleX, p->scaleTimer);
        p->scaleY += Div(64 - p->scaleY, p->scaleTimer);
        p->scaleTimer--;
    }
    Step(p);
    if (p->scaleTimer <= 0) {
        p->state = 2;
        p->flags &= ~2;
        p->entered = 1;
    } else {
        p->timer++;
    }
}
