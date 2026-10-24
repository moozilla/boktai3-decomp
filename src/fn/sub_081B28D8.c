// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"

// confidence=low: provisional role inferred from adjacent Vanargand part callbacks.

// confidence=low: inferred from adjacent Vanargand part callbacks and local state transitions.

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
    struct ContactSwarmSprite *sprite = &owner->sprite;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = base->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = one;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= flip;
        else
            owner->sprite.flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask;
        register u32 temp asm("r2");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        asm("" : "+r"(mask));
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                    ended = 1;
                } else {
                    ended = 0;
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed *= weight;
                speed >>= 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}

// confidence=low: aims at the player, winds back, strikes, and returns to rest.
void Vanargand_UpdatePartGroundStrike(u8 *boss, struct VanargandPart *p, u32 tick)
{
    u8 *enter;
    register u32 entering;
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
                    void *data = *(void **)(boss + 0x4d0);
                    register u32 flip = 4;
                    struct DisplaySequenceState *seq;
                    seq = &p->sequence;
                    Anim_SetSequence(seq, data, 7, (u8)p->variant, flip);
                    StepPlayback(p, &p->sequence, p, 1, flip);
                }
                MotionTrack_Initialize(p->motion, &p->origin, &delta, 0, 2);
                Sound_PlaySong(0x588);
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
                MotionTrack_Initialize(p->motion, &p->origin, &delta, 0, 4);
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
                MotionTrack_Initialize(p->motion, &p->origin, &p->target, 0, 1);
                *(u32 *)((u8 *)p + 0x19c) = 0;
            }
            if (++p->timer > 19) {
                SetState(p, 3);
                *enter = 1;
                Sound_PlaySong(0x589);
            }
            break;
        case 3:
            MotionTrack_Update(p->motion);
            if (MotionEnded(p)) {
                Sound_PlaySong(0x1ba);
                AnimatedParticles_EmitBurst(&p->sprite.position, *(s16 *)((u8 *)p + 0xb0), 0, 64, 4,
                                            4, 4, 4);
                SetState(p, 4);
                p->enter = 1;
            } else if (!(tick & 1))
                Vanargand_EmitPartTrail(p);
            break;
        case 4: {
            u32 entering;
            u32 *flags;
            /* Keep the flags address for the update after the entry effect. */
            {
                u32 mask = 8;
                u32 *initialFlags = &p->flags;
                *initialFlags |= mask;
            }
            entering = p->enter;
            flags = &p->flags;
            enter = &p->enter;

            if (entering) {
                *enter = 0;
                {
                    u16 saved = *(u16 *)((u8 *)p + 0x172);
                    *(u16 *)((u8 *)p + 0xf0) = saved;
                }
                *(u32 *)((u8 *)p + 0x19c) = 8;
            }
            if (!(p->timer & 15) && p->timer <= 44)
                AnimatedParticles_EmitBurst(&p->sprite.position, *(s16 *)((u8 *)p + 0xb0), 0, 64, 4,
                                            4, 4, 4);
            {
                u32 mask = 128;
                *flags |= mask;
            }
            if (++p->timer > 89) {
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
