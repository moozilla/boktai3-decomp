#include "global.h"

#ifdef NONMATCHING
// fuzzy: 95.8%
// fix.py best version (reprinted C)
// confidence=low: coordinates a part strike with its opposite part.
#include "contact_swarm.h"
#include "scene_vec.h"

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
static inline s32 TruncateShift(s32 x, u32 bits)
{
    return x >= 0 ? x >> bits : -((-x) >> bits);
}
static inline void MakeVector(struct SceneVec *out, s32 angle, s32 scale)
{
    out->x = TruncateShift(gUnk_086149C4[(angle + 64) & 255] * scale, 12);
    out->y = 0;
    out->z = TruncateShift(gUnk_086149C4[angle & 255] * scale, 12);
}
static inline void AddVector(struct SceneVec *dst, struct SceneVec *a, struct SceneVec *b)
{
    dst->x = a->x + b->x;
    dst->y = a->y + b->y;
    dst->z = a->z + b->z;
}
static inline void Difference(struct SceneVec *dst, struct SceneVec *a, struct SceneVec *b)
{
    dst->x = a->x - b->x;
    dst->y = a->y - b->y;
    dst->z = a->z - b->z;
}
static inline void SetFlags(struct VanargandPart *p, u32 mask)
{
    p->flags |= mask;
}
static inline void SetState(struct VanargandPart *p, u32 state)
{
    p->state = state;
    p->timer = 0;
}
static inline u8 MotionEnded(struct VanargandPart *p)
{
    return p->motion[0x29] == 2;
}
u32 ScenePair_CountPositiveCounters(void *);
void Vanargand_UpdatePartPairedStrike(u8 *boss, struct VanargandPart *p, u32 tick)
{
    u8 *enter;
    u32 mask = 0x20;
    s32 t1;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        Difference(&p->target, Player_GetEffectivePosition(0), p->base);
        if (ScenePair_CountPositiveCounters(boss) == 2) {
            if (p->variant == 1) {
                p->target.x += 112;
                p->target.z -= 112;
            } else {
                p->target.x -= 112;
                p->target.z += 112;
            }
        } else if (p->variant == 1) {
            p->target.x -= 256;
            p->target.z += 256;
        } else {
            p->target.x += 256;
            p->target.z -= 256;
        }
        p->target.y = p->origin.y;
        p->state = 0;
        p->timer = 0;
        p->enter = 1;
    }
    switch (p->state) {
        case 0: {
            struct SceneVec delta;
            enter = &p->enter;
            if (p->enter) {
                *enter = 0;
                if (p->variant == 1)
                    MakeVector(&delta, 224, 256);
                else
                    MakeVector(&delta, 96, 256);
                AddVector(&delta, &p->origin, &delta);
                MotionTrack_Initialize(p->motion, &p->origin, &delta, 0, 2);
            }
            MotionTrack_Update(p->motion);
            if (MotionEnded(p)) {
                SetState(p, 1);
                *enter = 1;
            }
            break;
        }

        case 1:
            enter = &p->enter;
            if (p->enter) {
                u32 zero = 0;
                *enter = zero;
                MotionTrack_Initialize(p->motion, &p->origin, &p->target, 0, 1);
                *((u32 *)(((u8 *)p) + 0x19c)) = zero;
            }
            if ((++p->timer) > 0) {
                Sound_PlaySong(0x588);
                SetState(p, 2);
                *enter = 1;
            }
            break;

        case 2:
            MotionTrack_Update(p->motion);
            if (MotionEnded(p)) {
                if ((ScenePair_CountPositiveCounters(boss) == 2) &&
                    (boss[(p->variant == 1) ? (0x205) : (0x421)] == 5)) {
                    struct SceneVec delta;
                    struct SceneVec position;
                    if (p->variant == 1)
                        MakeVector(&delta, 224, 256);
                    else
                        MakeVector(&delta, 96, 256);
                    AddVector(&delta, &p->origin, &delta);
                    MotionTrack_Initialize(p->motion, &p->origin, &delta, 0, 2);
                    Sound_PlaySong(0x1ba);
                    AddVector(&position, &p->target, p->base);
                    AnimatedParticles_EmitBurst(&position, *((s16 *)(((u8 *)p) + 0xb0)), 0, 64, 4,
                                                4, 4, 4);
                    SetState(p, 6);
                } else {
                    Sound_PlaySong(0x3d9);
                    t1 = 4;
                    AnimatedParticles_EmitBurst(&p->sprite.position, *((s16 *)(((u8 *)p) + 0xb0)),
                                                0, 64, t1, 4, 4, 4);
                    SetState(p, 6);
                }
                p->enter = 1;
                p->finished = 1;
            } else if (!(tick & 1))
                Vanargand_EmitPartTrail(p);
            break;

        case 3:
            enter = &p->enter;
            if (p->enter) {
                *enter = 0;
                SetFlags(p, 8);
                {
                    u16 saved = *((u16 *)(((u8 *)p) + 0x172));
                    *((u16 *)(((u8 *)p) + 0xf0)) = saved;
                }
                *((u32 *)(((u8 *)p) + 0x19c)) = 16;
            }
            if ((p->timer <= 44) && (!(p->timer & 15))) {
                struct SceneVec position;
                AddVector(&position, &p->target, p->base);
                AnimatedParticles_EmitBurst(&position, *((s16 *)(((u8 *)p) + 0xb0)), 0, 64, 4, 4, 4,
                                            4);
            }
            if ((++p->timer) > 90) {
                SetState(p, 4);
                *enter = 1;
            }
            break;

        case 4:
            SetFlags(p, 8);
            MotionTrack_Update(p->motion);
            if (MotionEnded(p)) {
                SetState(p, 5);
                p->enter = 1;
            }
            break;

        case 5:
            SetFlags(p, 8);
            if ((++p->timer) > 30) {
                SetState(p, 6);
                p->enter = 1;
            }
            break;

        case 6:
            SetFlags(p, 8);
            p->finished = 1;
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Vanargand_UpdatePartPairedStrike);
#endif
