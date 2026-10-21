// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"
struct VanargandPart {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x1c];
    struct DisplaySequenceState sequence;
    u8 motion[0x38];
    struct SceneVec *basePosition;
    u8 unknown94[8];
    struct SceneVec origin;
    struct SceneVec target;
    s16 heading, distance, particleAngle;
    u8 unknownB2[0xaa];
    u32 variant, flags, state, timer;
    u8 currentAction, previousAction, reset, enter, finished;
    u8 unknown171[7];
    u32 actionTimer;
    u8 unknown17C[0x20];
    u32 flashTimer;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);
extern const s16 gUnk_086149C4[];
static inline void StepSequence(void *obj)
{
    struct VanargandPart *owner = obj;
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 duration, flags, bit;
    state = &owner->sequence;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = owner->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    owner->sprite.frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
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
            ((struct VanargandPart *)obj)->sprite.flags |= 8;
        else
            ((struct VanargandPart *)obj)->sprite.flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r3");
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

static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void Vector(struct SceneVec *v, s32 angle, s32 scale)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * scale);
    v->y = 0;
    {
        s32 sample = gUnk_086149C4[angle & 255];
        v->z = Scale(sample * scale);
    }
}
static inline u8 Ended(struct VanargandPart *p)
{
    return p->motion[0x29] == 2;
}

void CpuSet(const void *, void *, u32);
void Palette_InterpolateBanks(void *, const void *, const void *, s32, s32);
// confidence=low: moves to the radial target and fades from the alternate palette.
void Vanargand_UpdatePartRevealFade(u8 *boss, struct VanargandPart *p, s32 frame)
{
    u32 mask = 12;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        Vector(&p->origin, p->heading, p->distance);
        p->sprite.flags &= ~1;
        CpuSet(*(void **)((u8 *)p + 0x194), *(void **)((u8 *)p + 0x38), 0x04000008);
        {
            void *resource = *(void **)(boss + 0x4d0);
            Anim_SetSequence(&p->sequence, resource, 9, (u8)p->variant, 0);
        }
        StepSequence(p);
        {
            u32 zero = 0;
            register u32 offset asm("r2") = 0x164;
            asm("" : "+r"(zero), "+r"(offset));
            {
                register u32 *ptr asm("r0") = (u32 *)((u8 *)p + offset);
                *ptr = zero;
            }
            {
                u32 off = 0x168;
                {
                    u32 *ptr = (u32 *)((u8 *)p + off);
                    *ptr = zero;
                }
            }
            {
                u32 off = 0x16f;
                u8 *enter;
                enter = (u8 *)p + off;
                *enter = 1;
            }
        }
    }
    {
        register u32 offset asm("r1") = 0x164;
        u32 *state;
        u32 value;
        state = (u32 *)((u8 *)p + offset);
        value = *state;
        switch (value) {
            case 0: {
                void *palette = *(void **)((u8 *)p + 0x38);
                u32 alternateOffset = 0x194;
                void **ptr;
                void *alternate;
                ptr = (void **)((u8 *)p + alternateOffset);
                alternate = *ptr;
                Palette_InterpolateBanks(palette, alternate, *(void **)((u8 *)p + 0x190), frame, 6);
            }
                if (frame > 63) {
                    u32 next = 1;
                    u32 off = 0x164;
                    register u32 *state asm("r1");
                    state = (u32 *)((u8 *)p + off);
                    *state = next;
                    {
                        u32 off2 = 0x168;
                        {
                            u32 *ptr = (u32 *)((u8 *)p + off2);
                            *ptr = 0;
                        }
                    }
                    {
                        register u8 *ptr asm("r1");
                        off += 11;
                        asm("" : "+r"(off));
                        ptr = (u8 *)p + off;
                        *ptr = next;
                    }
                }
                break;
            case 1: {
                register u32 off asm("r3") = 0x170;
                asm("" : "+r"(off));
                {
                    register u8 *ptr asm("r0") = (u8 *)p + off;
                    *ptr = 1;
                }
            } break;
        }
    }
}
