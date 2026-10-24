#include "global.h"
#include "contact_actor.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: advances falling motion entries and releases them at ground or expiry.
// Remaining differences: second flip-flag pointer lifetime and animation-mask allocation.
#define MotionEntry_UpdateFalling sub_081A42AC
struct MotionEntry {
    u32 flags;
    u8 unknown4[0xC];
    u16 frame;
    u8 unknown12[0xA];
    struct ContactPosition position;
    u8 unknown24[8];
    struct DisplaySequenceState sequence;
    struct ContactPosition savedPosition, velocity;
    void *origin;
    u16 lifetime, tick, radius, height;
    u8 type, mode;
    u16 angularStep;
};
void Hresvelgr_ReleaseEffect(void *);
static inline void Step(struct MotionEntry *input)
{
    struct MotionEntry *slot = input;
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    state = &slot->sequence;
    command = (const volatile u16 *)(((u32)slot->sequence.commands) + (state->index << 1));
    slot->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        {
            u32 bits = *command;
            u32 flip = 0x30;
            flip &= bits;
            flip >>= 4;
            flip &= bit;
            if (first != flip)
                slot->flags |= 4;
            else
                slot->flags &= ~4;
        }
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
            slot->flags |= 8;
        else
            slot->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        asm("" : "+r"(temp));
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
    return;
}

static inline void AddVelocity(struct ContactPosition *out, struct ContactPosition *v)
{
    out->x = out->x + v->x;
    out->y = out->y + v->y;
    out->z = out->z + v->z;
}
inline static void Step_1(struct MotionEntry *slot, struct MotionEntry *input)
{
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    state = &slot->sequence;
    command = (const volatile u16 *)(((u32)slot->sequence.commands) + (state->index << 1));
    slot->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        {
            u32 bits = *command;
            u32 flip = 0x30;
            flip &= bits;
            flip >>= 4;
            flip &= bit;
            if (first != flip)
                slot->flags |= 4;
            else
                slot->flags &= ~4;
        }
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
            slot->flags |= 8;
        else
            slot->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        asm("" : "+r"(temp));
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
    return;
}

void MotionEntry_UpdateFalling(struct MotionEntry *input)
{
    struct MotionEntry *p = input;
    AddVelocity(&p->position, &p->velocity);
    if (p->velocity.y > (-4))
        p->velocity.y--;
    if (p->position.y <= 0) {
        Hresvelgr_ReleaseEffect(p);
    } else {
        if (p->type != 3)
            Step_1(p, p);
        if ((--p->lifetime) == 0)
            Hresvelgr_ReleaseEffect(p);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081A42AC);
#endif
