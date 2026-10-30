// MATCHING_CONCESSION: register-pinning
#include "global.h"

#include "scene_actor_reset.h"
#include "display_animation.h"
struct TrailSprite {
    u32 flags;
    u8 unknown4[2], angle, unknown7, scaleX, scaleY;
    u8 unknownA[6];
    u16 frame;
    u8 unknown12[0xA];
    struct ContactPosition position;
};
s32 Mod(s32, s32);

static inline u32 FrameWeight(u16 v)
{
    register u32 value asm("r1") = v;
    u32 mask = 15;
    return mask & value;
}
static inline void CopyAngle(struct TrailSprite *src, u8 *dest)
{
    u8 angle = src->angle;
    *(u8 *)(dest + 0x70E) = angle;
}
static inline void CopyScaleX(struct TrailSprite *src, u8 *dest)
{
    u8 scaleX;
    scaleX = src->scaleX;
    *((u8 *)(dest + 0x710)) = scaleX;
}

static inline void CopyScaleY(struct TrailSprite *src, u8 *dest)
{
    u8 scaleY = src->scaleY;
    *(u8 *)(dest + 0x711) = scaleY;
}
static inline u32 Step(struct TrailSprite *input, u8 *context, u32 offset)
{
    register struct TrailSprite *sprite asm("r6") = input;
    struct DisplaySequenceState *state = (void *)(offset + (u32)context);
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    state = (void *)((u8 *)state + 0x750);
    {
        u32 offset = state->index << 1;
        const u16 *table = state->commands;
        entry = (const u16 *)((u32)table + offset);
    }
    sprite->frame = *entry >> 6;
    {
        register u32 input asm("r0") = state->flags;
        u32 mask = 1;
        register u32 left = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        register u32 input asm("r0") = state->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    n = ++state->elapsed;
    if (n >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                } else {
                    state->index = state->frameCount - 1;
                }
                done = 1;
            } else {
                state->index--;
                done = 0;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if ((u8)(state->flags & 8)) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                    done = 1;
                } else {
                    state->index = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        {
            u32 offset = state->index << 1;
            const u16 *table = state->commands;
            entry = (const u16 *)((u32)table + offset);
        }
        state->frameWeight = FrameWeight(*entry);
        state->duration = (state->frameWeight * state->speed) >> 6;
        if (!state->duration)
            state->duration = 1;
    } else
        done = 0;
    return done;
}

void Axe_EmitMotionTrail(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    struct TrailSprite *sprite;
    void *sourceDrawable;
    u8 *slotBase;
    void *scaleDrawable;
    u8 *scaleSlotBase;
    if ((*((u32 *)actor->drawable)) & 1)
        return;
    {
        register s32 zero = Mod(actor->field2CA, *((u16 *)(context + 0xE46)));
        if (zero)
            return;
        {
            struct DisplaySequenceState *source = (void *)(((u8 *)actor->drawable) + 0x48);
            u16 sequence = source->unknownA;
            {
                u32 offset = *((u16 *)(context + 0xE44)) * 0x60;
                u32 spriteOffset = 0x708;
                u8 *destination = context + (offset + spriteOffset);
                u8 *sequenceState = destination + 0x48;
                u32 sequenceOffset = 0x760;
                void *commands = *(void **)(context + sequenceOffset + offset);
                u32 speedOffset = 0xED7;
                u8 speed = context[speedOffset];
                Anim_SetSequence((void *)sequenceState, commands, sequence, speed, zero);
            }
        }
    }
    sprite = (void *)(((*((u16 *)(context + 0xE44))) * 0x60) + (u32)context);
    sprite = (void *)((u8 *)sprite + 0x708);
    Step(sprite, context, *((u16 *)(context + 0xE44)) * 0x60);
    {
        u32 offset = *((u16 *)(context + 0xE44)) * 0x60;
        u8 *slot = context + offset;
        *((struct ContactPosition *)(slot + 0x724)) = actor->body.position;
    }
    {
        u32 offset = *((u16 *)(context + 0xE44)) * 0x60;
        u8 *slots = context + 0x708;
        *((u32 *)(slots + offset)) &= ~1;
        slots += *((u16 *)(context + 0xE44)) * 0x60;
        *((u32 *)slots) |= 2;
    }
    {
        u32 offset = *((u16 *)(context + 0xE44)) * 0x60;
        u8 *slot = context + offset;
        *((u16 *)(slot + 0x764)) = 8;
    }
    sourceDrawable = actor->drawable;
    slotBase = context + ((*((u16 *)(context + 0xE44))) * 0x60);
    CopyAngle(sourceDrawable, slotBase);
    scaleDrawable = actor->drawable;
    scaleSlotBase = context + ((*((u16 *)(context + 0xE44))) * 0x60);
    CopyScaleX(scaleDrawable, scaleSlotBase);
    sourceDrawable = actor->drawable;
    slotBase = context + ((*((u16 *)(context + 0xE44))) * 0x60);
    CopyScaleY(sourceDrawable, slotBase);
    if ((++(*((u16 *)(context + 0xE44)))) > 3)
        *((u16 *)(context + 0xE44)) = 0;
}
