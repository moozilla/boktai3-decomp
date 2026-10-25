#include "global.h"
#include "contact_actor.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: advances falling motion entries and releases them at ground or expiry.
#define MotionEntry_UpdateFalling sub_081A4420
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
static inline s32 MotionEntry_ScaleFrameWeight(s32 weight, s32 speed)
{
    weight *= speed;
    return weight >> 6;
}
static inline void MotionEntry_StepSequence(struct MotionEntry *input)
{
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 duration;
    u32 flags;
    u32 bit;
    state = &input->sequence;
    command = (const volatile u16 *)(((u32)input->sequence.commands) + (state->index << 1));
    input->frame = (*command) >> 6;
    {
        u32 animationFlags = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= animationFlags;
        {
            u32 bits = *command;
            u32 flip = 0x30;
            flip &= bits;
            flip >>= 4;
            flip &= bit;
            if (first != flip)
                input->flags |= 4;
            else
                input->flags &= ~4;
        }
    }
    {
        u32 animationFlags = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= animationFlags;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            input->flags |= 8;
        else
            input->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 maskValue asm("r1");
        u8 savedDuration;
        state->elapsed = elapsed;
        /* Keep the mask source and saved mask distinct through allocation. */
        maskValue = 0xFFFF;
        asm("" : "+r"(maskValue));
        mask = maskValue;
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
                    endFlag = 8;
                    endFlag &= flags;
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
                s32 weight = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = MotionEntry_ScaleFrameWeight(weight, speed);
                state->duration = speed;
                if (!((u8)speed))
                    state->duration = 1;
            }
        }
    }
    return;
}

static inline void MotionEntry_AddVelocity(struct ContactPosition *out, struct ContactPosition *v)
{
    out->x = out->x + v->x;
    out->y = out->y + v->y;
    out->z = out->z + v->z;
}
void MotionEntry_UpdateFalling(struct MotionEntry *input)
{
    struct MotionEntry *p = input;
    MotionEntry_AddVelocity(&p->position, &p->velocity);
    if (p->velocity.y > (-4))
        p->velocity.y--;
    if (p->position.y <= 0) {
        Hresvelgr_ReleaseEffect(p);
    } else {
        if (p->type != 3)
            MotionEntry_StepSequence(p);
        if ((--p->lifetime) == 0)
            Hresvelgr_ReleaseEffect(p);
    }
}
