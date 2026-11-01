#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"
#include "player_cue_sequence.h"

// confidence=low: player-directed sweeping strike followed by a timed retreat.

// confidence=low: inferred from adjacent Vanargand part callbacks and local state transitions.

// Exact with temporary allocation concessions in StepPlayback.
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
    dst->x = b->x + a->x;
    dst->y = b->y + a->y;
    dst->z = b->z + a->z;
}
static inline void Difference(struct SceneVec *dst, struct SceneVec *a, struct SceneVec *b)
{
    dst->x = a->x - b->x;
    dst->y = a->y - b->y;
    dst->z = a->z - b->z;
}
static inline s32 AimAngle(struct VanargandPart *p)
{
    s32 x = p->sprite.position.x, z = p->sprite.position.z;
    s32 dx = Player_GetEffectivePosition(0)->x;
    s32 angle;
    {
        s32 dz = Player_GetEffectivePosition(0)->z;
        angle = Math_Atan2(dx - x, dz - z) & 255;
    }
    if ((u32)(angle - 16) > 176)
        angle = 16;
    else if (angle > 112)
        angle = 112;
    return angle;
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
static inline u32 StepPlayback(struct VanargandPart *owner, struct DisplaySequenceState *state,
                               struct VanargandPart *base, u32 one, u32 flip)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, &base->sequence);
}

// confidence=low: aims at the player, winds back, strikes, and returns to rest.
static inline u32 Arg1(u32 k_, struct VanargandPart *p)
{
    return p->variant << k_;
}

void Vanargand_UpdatePartSweepingStrike(u8 *boss, struct VanargandPart *p, u32 tick)
{
    u8 *enter;
    u32 entering;
    u32 mask = 0x20;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        p->state = 0;
        p->timer = 0;
        p->enter = 1;
    }
    switch (p->state) {
        case 0: {
            struct SceneVec delta;
            entering = p->enter;
            enter = &p->enter;
            if (entering) {
                *enter = 0;
                MakeVector(&delta, AimAngle(p), 256);
                AddVector(&delta, &p->origin, &delta);
                {
                    void *motion = p->motion;
                    u32 angle = (Arg1(7, p) + 96) & 255;
                    OrbitMotion_BeginDistanceWave(motion, &p->origin, &delta, angle, 16);
                }
                {
                    void *data = *((void **)(boss + 0x4d0));
                    u32 flip = 4;
                    struct DisplaySequenceState *seq;
                    seq = &p->sequence;
                    Anim_SetSequence(seq, data, 8, (u8)p->variant, flip);
                    StepPlayback(p, &p->sequence, p, 1, flip);
                }
            }
            MotionTrack_Update(p->motion);
            if (MotionEnded(p)) {
                SetState(p, 1);
                *enter = 1;
            }
            break;
        }

        case 1: {
            struct SceneVec delta;
            entering = p->enter;
            enter = &p->enter;
            if (entering) {
                *enter = 0;
                MakeVector(&delta, (AimAngle(p) + 128) & 255, 512);
                AddVector(&delta, &p->origin, &delta);
                MotionTrack_Initialize(p->motion, &p->origin, &delta, 0, 3);
                Sound_PlaySong(0x588);
            }
            MotionTrack_Update(p->motion);
            if (MotionEnded(p)) {
                SetState(p, 2);
                *enter = 1;
            }
            break;
        }

        case 2:
            entering = p->enter;
            enter = &p->enter;
            if (entering) {
                p->enter = 0;
                Difference(&p->target, Player_GetEffectivePosition(0), p->base);
                {
                    struct SceneVec *player = Player_GetEffectivePosition(0);
                    u32 baseY = (u16)p->base->y;
                    u32 playerY = (u16)player->y;
                    p->target.y = playerY - baseY;
                }
                OrbitMotion_BeginDistanceWave(p->motion, &p->origin, &p->target,
                                              ((p->variant << 7) + 96) & 255, 16);
                *((u32 *)(((u8 *)p) + 0x19c)) = 0;
            }
            if ((++p->timer) > 23) {
                Sound_PlaySong(0x589);
                SetState(p, 3);
                *enter = 1;
            }
            break;

        case 3:
            MotionTrack_Update(p->motion);
            if (MotionEnded(p)) {
                Sound_PlaySong(0x3d9);
                AnimatedParticles_EmitBurst(&p->sprite.position, *((s16 *)(((u8 *)p) + 0xb0)), 0,
                                            64, 4, 4, 4, 4);
                SetState(p, 4);
                p->enter = 1;
            } else if (!(tick & 1))
                Vanargand_EmitPartTrail(p);
            break;

        case 4: {
            struct SceneVec *v;
            SetFlags(p, 8);
            entering = p->enter;
            enter = &p->enter;
            if (entering) {
                *enter = 0;
                {
                    u16 saved = *((u16 *)(((u8 *)p) + 0x172));
                    *((u16 *)(((u8 *)p) + 0xf0)) = saved;
                }
            }
            v = (struct SceneVec *)(p->motion + 0x14);
            v->x = TruncateShift(v->x, 1);
            v->y = TruncateShift(v->y, 1);
            v->z = TruncateShift(v->z, 1);
            AddVector(&p->origin, v, &p->origin);
            if ((++p->timer) > 59) {
                SetState(p, 5);
                *enter = 1;
            }
            break;
        }

        case 5: {
            struct SceneVec delta;
            SetFlags(p, 8);
            entering = p->enter;
            enter = &p->enter;
            if (entering) {
                *enter = 0;
                MakeVector(&delta, p->heading, p->distance);
                MotionTrack_Initialize(p->motion, &p->origin, &delta, 0, 4);
            }
            MotionTrack_Update(p->motion);
            if (MotionEnded(p)) {
                SetState(p, 6);
                *enter = 1;
            }
            break;
        }

        case 6:
            SetFlags(p, 8);
            p->finished = 1;
            break;
    }
}
