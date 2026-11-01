#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"
#include "player_cue_sequence.h"
// confidence=low: emits six radial effects, waits for animation 10, then deactivates the part.

// confidence=low: inferred from adjacent Vanargand part callbacks and local state transitions.

// Exact with register-pinning and empty-asm concessions.
struct VanargandState {
    u32 state;
    u32 timer;
};
struct VanargandPart {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x1c];
    struct DisplaySequenceState sequence;
    u8 motion[0x44];
    struct SceneVec origin;
    u8 unknownA4[8];
    s16 heading;
    u8 unknownAE[0xae];
    u32 variant;
    u32 flags;
    struct VanargandState control;
    u8 unknown16C[2];
    u8 reset, enter, finished;
};
extern const s16 gUnk_086149C4[];
void CpuSet(const void *, void *, u32);
void Sound_PlaySong(u32);
void VanargandEffect_Create(u32, s32, const void *, const void *, u32);
void ActorStatus_Deactivate(void *, void *);
static inline s32 TruncateScale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void MakeVector(struct SceneVec *out, s32 angle, s32 scale)
{
    out->x = TruncateScale(gUnk_086149C4[(angle + 64) & 255] * scale);
    out->y = 0;
    out->z = TruncateScale(gUnk_086149C4[angle & 255] * scale);
}
static inline void AddVector(struct SceneVec *dst, struct SceneVec *a, struct SceneVec *b)
{
    dst->x = b->x + a->x;
    dst->y = b->y + a->y;
    dst->z = b->z + a->z;
}
static inline void ClearVector(struct SceneVec *dst)
{
    volatile u32 zero = 0;
    CpuSet((const void *)&zero, dst, 0x05000002);
}
static inline void SetState(struct VanargandPart *p, u32 state)
{
    p->control.state = state;
    p->control.timer = 0;
}
void Vanargand_ReleaseRadialEffects(u8 *boss, struct VanargandPart *p)
{
    u32 mask = 12;
    p->flags |= mask;
    if (p->reset) {
        struct SceneVec velocity;
        struct SceneVec position;
        s32 i;
        struct VanargandState *state;
        u32 *timer;
        u8 *entered;
        p->reset = 0;
        Anim_SetSequence(&p->sequence, *((void **)(boss + 0x4d0)), 10, (u8)p->variant, 0);
        ClearVector(&velocity);
        i = 0;
        state = &p->control;
        timer = &state->timer;
        entered = &p->enter;
        do {
            MakeVector(&position, (i * 255) / 6, 128);
            AddVector(&position, (struct SceneVec *)(&p->sprite.position), &position);
            VanargandEffect_Create(1, i << 1, &position, &velocity, p->variant);
            i++;
        } while (i <= 5);
        Sound_PlaySong(0x58c);
        state->state = 0;
        *timer = 0;
        *entered = 1;
    }
    switch (p->control.state) {
        case 0:
            if (PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)p, &p->sequence)) {
                SetState(p, 1);
                p->enter = 1;
            }
            break;

        case 1:
            p->finished = 1;
            ActorStatus_Deactivate(boss, p);
            break;
    }
}
