// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "global.h"
#include "display_animation.h"
#ifdef NONMATCHING
// fuzzy: 61.6%
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: stack-vector placement, source-coordinate addresses and allocation.
struct NidhoggrVector {
    s16 x, y, z, pad;
};
struct NidhoggrSprite {
    u32 flags;
    u8 pad4[12];
    u16 frame;
    u8 pad12[10];
    struct NidhoggrVector position;
    u8 pad24[8];
};
struct NidhoggrEffect {
    void *update, *destroy;
    u8 animation[0x1c];
    struct NidhoggrSprite sprite;
    struct DisplaySequenceState sequence;
    u8 pad60[0x114];
    void *anchor;
    struct NidhoggrVector velocity;
    u8 state, age;
    s16 ground;
    u8 *boss;
};
#ifndef NIDHOGGR_MASK
#define NIDHOGGR_MASK "r5"
#endif
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_086141BC[];
extern u32 gUnk_03005304;
void Nidhoggr_CreateTimedPositionEffect(void *, void *);
void Collider_Remove(void *);
s32 Math_Atan2(s32, s32);
struct SeekingPackedVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
union SeekingVector {
    struct SeekingPackedVector packed;
    struct NidhoggrVector coordinates;
    u32 words[2];
};
static inline s32 RandomSample(void)
{
    gUnk_03005304 = (gUnk_03005304 + 1) & 1023;
    return gUnk_086141BC[gUnk_03005304];
}
static inline void OffsetPointAdd(union SeekingVector *v, struct NidhoggrVector *position)
{
    v->coordinates.x = position->x + v->coordinates.x;
    v->coordinates.y = v->coordinates.y + position->y;
    v->coordinates.z = position->z + v->coordinates.z;
}
// confidence=low: steers an attack effect toward the player with two randomized planar components.
#define Nidhoggr_UpdateSeekingAttackEffect sub_0818FEE4
void Nidhoggr_UpdateSeekingAttackEffect(struct NidhoggrEffect *pArg)
{
    register struct NidhoggrEffect *p asm("r6") = pArg;
    u8 *state;
    union SeekingVector motion;
    union SeekingVector immediatePoint;
    union SeekingVector expiredPoint;
    s32 dx;
    s32 dy;
    s32 dz;
    s32 angle;
    s32 randomAngle;
    struct NidhoggrVector *position;
    asm("" : : "r"(p));
    {
        struct NidhoggrSprite *sprite = &p->sprite;
        struct DisplaySequenceState *state = &p->sequence;
        const volatile u16 *command;
        u32 ended;
        u32 duration;
        u32 flags;
        u32 bit;
        {
            u32 offset = state->index << 1;
            const volatile u16 *table;
            table = p->sequence.commands;
            command = (const volatile u16 *)(((u32)table) + offset);
        }
        sprite->frame = (*command) >> 6;
        {
            u32 input = state->flags;
            register u32 first asm("r1");
            u32 maskBit = 1;
            bit = maskBit;
            asm("" : "+r"(bit));
            first = bit;
            first &= input;
            if (first != ((((*command) & 0x30) >> 4) & bit))
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
            if (first != ((((*command) & 0x30) >> 4) & bit))
                sprite->flags |= 8;
            else
                sprite->flags &= ~8;
        }
        {
            u32 elapsed = state->elapsed + 1;
            register u32 mask asm("r5");
            register u32 temp asm("r1");
            register u32 savedDuration asm("r3");
            state->elapsed = elapsed;
            temp = 0xFFFF;
            asm("" : "+r"(temp));
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
                    command = (const volatile u16 *)(((u32)table) + offset);
                }
                state->frameWeight = (*command) & 15;
                {
                    register s32 weight asm("r1") = state->frameWeight;
                    s32 speed;
                    speed = state->speed;
                    speed = (speed * weight) >> 6;
                    state->duration = speed;
                    if (!((u8)speed))
                        state->duration = 1;
                }
            } else {
                ended = 0;
            }
        }
    }
    state = &p->state;
    if ((*state) == 0)
        goto initial_phase;
    if ((*state) == 255)
        goto immediate_impact;
    {
        u8 *player = gUnk_02000710;
        position = &p->sprite.position;
        dx = (*((s16 *)(player + 0x30))) - position->x;
        dy = (*((s16 *)(player + 0x32))) - position->y;
        dz = (*((s16 *)(player + 0x34))) - position->z;
    }
    angle = Math_Atan2(dx, dz);
    if (RandomSample() > 0xdfff)
        goto strong_motion;
    motion.words[0] = (u16)(gUnk_086149C4[(angle + 64) & 255] >> 10);
    motion.packed.z = gUnk_086149C4[angle & 255] >> 10;
    goto motion_ready;
initial_phase: {
    s16 speed = ++p->velocity.x;
    if (speed > 8)
        p->velocity.x = 8;
    {
        u8 *age = &p->age;
        if ((*age) > 30) {
            (*state)++;
            *age = 0;
        }
    }
    return;
}

immediate_impact: {
    u8 *boss = p->boss;
    immediatePoint.words[0] = 0xffa00000;
    do {
        immediatePoint.packed.z = 0;
    } while (0);
    OffsetPointAdd(&immediatePoint, &p->sprite.position);
    Nidhoggr_CreateTimedPositionEffect(&immediatePoint, boss);
    goto destroy;
}

strong_motion:
    motion.words[0] = (u16)(gUnk_086149C4[(angle + 64) & 255] >> 9);

    motion.packed.z = gUnk_086149C4[angle & 255] >> 9;
motion_ready:
    randomAngle = (u8)RandomSample();

    motion.packed.x += gUnk_086149C4[(randomAngle + 64) & 255] >> 10;
    motion.packed.z += gUnk_086149C4[randomAngle & 255] >> 10;
    p->velocity = motion.coordinates;
    if (p->age == 0)
        p->state++;
    if (p->state <= 1)
        goto proximity;
    if (p->age <= 88)
        goto proximity;
    {
        u8 *boss = p->boss;
        expiredPoint.words[0] = 0xff800000;
        expiredPoint.packed.z = 0;
        OffsetPointAdd(&expiredPoint, position);
        Nidhoggr_CreateTimedPositionEffect(&expiredPoint, boss);
        Collider_Remove(((u8 *)p) + 0x120);
    }
destroy:
    p->update = 0;

    if (p->destroy) {
        Callback_InvokeActor(p, p->destroy);
        p->destroy = 0;
    }
    return;
proximity:
    dx += 48;

    dy += 256;
    dz += 48;
    if (((((((u32)dx) <= 96) && (dy >= 0)) && (dy <= 256)) && (dz >= 0)) && (dz <= 96))
        p->state = 255;
    return;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0818FEE4);
#endif
