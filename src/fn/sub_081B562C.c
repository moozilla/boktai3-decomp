// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: keyframe pointer allocation, vector lifetimes and projection stores.
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
static inline void Vector(struct SceneVec *v, s32 angle, s16 scale)
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

void CpuSet(const void *, void *, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);
void Sound_PlaySong(u32);
void sub_081B0248(void *);
extern s16 gUnk_03005418[];
static inline s32 Divide32(s32 value)
{
    return value >= 0 ? value >> 5 : -((-value) >> 5);
}
static inline void SetState(struct VanargandPart *p, u32 state)
{
    p->state = state;
    p->timer = 0;
}

struct EmergenceProjection {
    s16 x, y, z;
} __attribute__((aligned(4)));
static inline void Project(struct EmergenceProjection *out, struct SceneVec *position)
{
    s32 x = position->x, z = position->z;
    s32 sum, height;
    out->x = Divide32((x - z) * 3);
    sum = Divide32((x + z) * 3);
    height = Divide32(position->y * 3);
    out->y = sum - height;
    out->z = sum + height;
}
// confidence=low: reveals the controller and moves its part outward after two keyframe tracks.
#define Vanargand_UpdatePartEmergence sub_081B562C
static inline void StepSequence_081B562C(void *obj)
{
    struct VanargandPart *owner = obj;
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 duration;
    u32 flags;
    u32 bit;
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

static inline void Project_081B562C(struct EmergenceProjection *out, struct SceneVec *position)
{
    s32 x = position->x, z = position->z;
    s32 sum;
    s32 height;
    out->x = Divide32((x - z) * 3);
    sum = Divide32((x + z) * 3);
    height = Divide32(position->y * 3);
    out->y = sum - height;
    out->z = sum + height;
}

void Vanargand_UpdatePartEmergence(u8 *boss, struct VanargandPart *p)
{
    struct EmergenceProjection projected;
    u8 *enter;
    u32 entering;
    u32 state;
    u32 mask = 12;
    typeof(*((void **)(((u8 *)p) + 0x190))) t1;
    typeof(*((void **)(((u8 *)p) + 0x38))) t2;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        if (!(p->sprite.flags & 1)) {
            p->finished = 1;
            SetState(p, 3);
            p->enter = 1;
            return;
        }
        *((u32 *)(boss + 0x20)) &= ~1;
        if (p->variant == 0)
            *((u32 *)(boss + 0x20)) |= 4;
        else
            *((u32 *)(boss + 0x20)) &= ~4;
        {
            void *data = *((void **)(boss + 0x4d0));
            u32 zero = 0;
            asm("" : "+r"(zero));
            Anim_SetSequence(&p->sequence, data, 9, (u8)p->variant, zero);
        }
        StepSequence_081B562C(p);
        t2 = *((void **)(((u8 *)p) + 0x38));
        t1 = *((void **)(((u8 *)p) + 0x190));
        CpuSet(t1, t2, 0x04000008);
        SetState(p, 0);
        p->enter = 1;
    }
    state = p->state;
    switch (state) {
        case 0:
            entering = p->enter;
            enter = &p->enter;
            if (entering) {
                {
                    u8 k1 = 0;
                    *enter = k1;
                }
                Anim_SetKeyframes(boss + 0x18, boss + 0x78, 0, 2, 0);
                Sound_PlaySong(0x58d);
            }
            if (Anim_Step(boss + 0x18, boss + 0x78)) {
                SetState(p, 1);
                *enter = 1;
            }
            p->sprite.position = *((struct ContactPosition *)p->basePosition);
            break;

        case 1:
            entering = p->enter;
            enter = &p->enter;
            if (entering) {
                {
                    u8 k2 = 0;
                    *enter = k2;
                }
                Anim_SetKeyframes(boss + 0x18, boss + 0x78, 1, 2, 0);
            }
            if (Anim_Step(boss + 0x18, boss + 0x78)) {
                SetState(p, 2);
                *enter = 1;
                *((u32 *)(boss + 0x20)) |= 1;
            }
            p->sprite.position = *((struct ContactPosition *)p->basePosition);
            break;

        case 2: {
            struct SceneVec target;
            entering = p->enter;
            enter = &p->enter;
            if (entering) {
                *enter = 0;
                p->sprite.flags &= ~1;
                Vector(&p->origin, p->heading, 640);
                Vector(&target, p->heading, p->distance);
                MotionTrack_Initialize(p->motion, &p->origin, &target, 0, 4);
                sub_081B0248(p);
                Sound_PlaySong(0x58e);
            }
            MotionTrack_Update(p->motion);
            if (Ended(p)) {
                SetState(p, 3);
                *enter = 1;
            }
            break;
        }

        case 3:
            p->finished = 1;
            break;
    }

    Project_081B562C(&projected, p->basePosition);
    projected.x = (projected.x - gUnk_03005418[0]) + 120;
    projected.y = (projected.y - gUnk_03005418[1]) + 92;
    *((struct EmergenceProjection *)(boss + 0x38)) = projected;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081B562C);
#endif
