// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "player.h"
// confidence=low: provisional action interpretation from retained C and assembly.
static inline void Step(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st)
{
    struct DisplaySequenceState *state = st;
    struct ContactSwarmSprite *sprite = sp;
    const volatile u16 *command;
    u32 duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
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
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        mask = temp;
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
                } else {
                    state->index = index - 1;
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
                } else {
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
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        }
    }
}

u32 Player_ConsumeStateEntry(struct Player *);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Sound_PlaySong(u32);
void RisingSprite_Spawn(void *);
void SparkParticle_Spawn(void *, s32, s32, s32, s32, s32, s32, s32);
void Camera_RequestShake(u32);
extern const s16 gUnk_086149C4[];
#define B(o)  (*(u8 *)((u8 *)p + (o)))
#define H(o)  (*(u16 *)((u8 *)p + (o)))
#define SH(o) (*(s16 *)((u8 *)p + (o)))
static inline void Approach(struct Player *p)
{
    register u32 o47c asm("r1") = 0x47C;
    register u8 *a47c asm("r0");
    s16 *target;
    s32 x, z;
    asm("" : "+r"(o47c));
    a47c = (u8 *)p + o47c;
    target = *(s16 **)a47c;
    x = target[0] - SH(0x30);
    z = target[2] - SH(0x34);
    if (x >= 0)
        SH(0x38) = x >> 3;
    else
        SH(0x38) = -(-x >> 3);
    if (z >= 0)
        SH(0x3C) = z >> 3;
    else
        SH(0x3C) = -(-z >> 3);
}
static inline void Approach2(struct Player *p)
{
    s16 *target = *(s16 **)((u8 *)p + 0x47C);
    s32 x, z;
    x = target[0] - SH(0x30);
    z = target[2] - SH(0x34);
    if (x >= 0)
        SH(0x38) = x >> 3;
    else
        SH(0x38) = -(-x >> 3);
    if (z >= 0)
        SH(0x3C) = z >> 3;
    else
        SH(0x3C) = -(-z >> 3);
}
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}
// confidence=low: moves toward a saved effect position or recoils while shake decays.
void Player_UpdateRecoilMotion(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
        if (H(0x4C2)) {
            Sound_PlaySong(H(0x4C2));
            H(0x4C2) = 0;
        }
    }
    Anim_SetSequence((void *)((u8 *)p + 0x3FC), *(void **)((u8 *)p + 0x410), 2, p->heading,
                     p->side);
    Step((void *)((u8 *)p + 0x14C), (void *)((u8 *)p + 0x3FC));
    {
        register u32 dummy asm("r1");
        u32 off270;
        u16 *q270;
        off270 = 0x270;
        q270 = (u16 *)((u8 *)p + off270);
        asm("" ::"r"(dummy));
        if (*q270) {
            register u32 o479 asm("r6") = 0x479;
            register u8 *a479 asm("r0");
            asm("" : "+r"(o479));
            a479 = (u8 *)p + o479;
            if (*a479) {
                Approach(p);
                if (H(0x45A) <= 5) {
                    if (!(H(0x45A) & 1)) {
                        Sound_PlaySong(0x191);
                        RisingSprite_Spawn((u8 *)p + 0x30);
                    }
                    H(0x45A)++;
                }
            } else if (B(0x47B)) {
                Approach2(p);
                if (H(0x45A) <= 5) {
                    if (!(H(0x45A) & 1)) {
                        Sound_PlaySong(0x191);
                        SparkParticle_Spawn((u8 *)p + 0x30, 1, 0, 0, 0, 12, 2, 2);
                    }
                    H(0x45A)++;
                }
            } else if (B(0x489)) {
                Approach2(p);
                H(0x45A)++;
            } else {
                s32 angle = ((p->facing + 5) & 7) << 5;
                SH(0x38) = -Scale(gUnk_086149C4[(angle + 64) & 255] * H(0x270));
                SH(0x3C) = -Scale(gUnk_086149C4[angle] * H(0x270));
            }
            Camera_RequestShake(H(0x270));
            H(0x270) = H(0x270) * 3 >> 2;
        } else {
            H(0x270) = 0;
            B(0x479) = 0;
            B(0x47B) = 0;
            B(0x489) = 0;
            B(0x4BA) = 0;
            if (B(0x5B9) == 3 && B(0x5BB))
                ActorSequence_Begin(p, 22, 0);
            else {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 0, 0);
            }
        }
    }
}
