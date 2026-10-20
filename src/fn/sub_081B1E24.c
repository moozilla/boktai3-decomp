#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"

#ifdef NONMATCHING
// fuzzy: 90.7%
// Remaining differences: register lifetimes, inline setup order, and literal-pool placement.
struct VanargandPart {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x1c];
    struct DisplaySequenceState sequence;
    u8 motion[0x38];
    struct SceneVec *base;
    u8 unknown94[8];
    struct SceneVec origin;
    struct SceneVec target;
    s16 heading, distance;
    u8 unknownB0[0xac];
    u32 variant, flags, state, timer;
    u8 unknown16C[2];
    u8 reset, enter, finished;
};
void MotionTrack_Initialize(void *, const void *, const void *, s32, u32);
void OrbitMotion_BeginDistanceWave(void *, const void *, const void *, s32, s32);
void MotionTrack_Update(void *);
void Vanargand_EmitPartTrail(void *);
void Sound_PlaySong(u32);
void AnimatedParticles_EmitBurst(void *, s32, s32, s32, s32, s32, s32, s32);
struct SceneVec *Player_GetEffectivePosition(u32);
s32 Math_Atan2(s32, s32);
extern const s16 gUnk_086149C4[];
static inline void Difference(struct SceneVec *dst, struct SceneVec *a, struct SceneVec *b)
{
    dst->x = a->x - b->x;
    dst->y = a->y - b->y;
    dst->z = a->z - b->z;
}
static inline void SetState(struct VanargandPart *p, u32 state)
{
    p->state = state;
    p->timer = 0;
}
static inline void Offset(struct SceneVec *v, s32 x, s32 z)
{
    v->x = x + (u16)v->x;
    v->z = z + (u16)v->z;
}
// confidence=low: periodically retargets the part beside the player for 64 ticks.
void Vanargand_UpdatePartTracking(u8 *boss, struct VanargandPart *p)
{
    struct SceneVec delta;
    u32 mask = 8;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        p->state = 0;
        p->timer = 0;
        p->enter = 1;
        Anim_SetSequence(&p->sequence, *(void **)(boss + 0x4d0), 8, (u8)p->variant, 0);
    }
    switch (p->state) {
        case 0:
            if (!(p->timer & 15)) {
                Difference(&delta, Player_GetEffectivePosition(0), p->base);
                if (p->variant == 1)
                    Offset(&delta, 512, -512);
                else
                    Offset(&delta, -512, 512);
                MotionTrack_Initialize(p->motion, &p->origin, &delta, 0, 4);
            }
            MotionTrack_Update(p->motion);
            if (++p->timer & 64) {
                SetState(p, 1);
                p->enter = 1;
            }
            break;
        case 1:
            p->finished = 1;
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Vanargand_UpdatePartTracking);
#endif
