// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// fuzzy: 87.0%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: sequence setup, field-access order, and register allocation.
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
static inline void CopyScaleX(struct TrailSprite *src, u8 *dest)
{
    typeof(src->scaleX) t1;
    t1 = src->scaleX;
    *((u8 *)(dest + 0x710)) = t1;
}

static inline void CopyScaleY(struct TrailSprite *src, u8 *dest)
{
    *(u8 *)(dest + 0x711) = src->scaleY;
}
static inline u32 Step(struct TrailSprite *input, struct DisplaySequenceState *state)
{
    register struct TrailSprite *sprite asm("r6") = input;
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    {
        u32 offset = state->index << 1;
        const u16 *table = state->commands;
        entry = (const u16 *)(offset + (u32)table);
    }
    sprite->frame = *entry >> 6;
    {
        register u32 input asm("r0") = state->flags;
        u32 mask = 1;
        register u32 left asm("r1") = (u8)(input & mask);
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
            entry = (const u16 *)(offset + (u32)table);
        }
        state->frameWeight = FrameWeight(*entry);
        state->duration = (state->frameWeight * state->speed) >> 6;
        if (!state->duration)
            state->duration = 1;
    } else
        done = 0;
    return done;
}

void ActorPhase_EmitTrailSprite(struct SceneActorRegistryResetView *actor,
                                const struct ContactPosition *position, u32 angle)
{
    u8 *context = actor->context;
    struct TrailSprite *sprite;
    struct DisplaySequenceState *state;
    typeof(actor->drawable) t1;
    typeof(context + ((*((u16 *)(context + 0xE44))) * 0x60)) t2;
    typeof(actor->drawable) t3;
    typeof(context + ((*((u16 *)(context + 0xE44))) * 0x60)) t4;
    if ((*((u32 *)actor->drawable)) & 1)
        return;
    {
        register s32 zero asm("ip") = Mod(actor->field2CA, *((u16 *)(context + 0xE46)));
        if (zero)
            return;
        {
            struct DisplaySequenceState *source = (void *)(((u8 *)actor->drawable) + 0x48);
            u16 sequence = source->unknownA;
            {
                u32 k1 = 0x760;
                Anim_SetSequence(
                    (void *)(((context + 0x708) + ((*((u16 *)(context + 0xE44))) * 0x60)) + 0x48),
                    *((void **)(((*((u16 *)(context + 0xE44))) * 0x60) + ((u32)(context + k1)))),
                    sequence, context[0xED7], zero);
            }
        }
    }
    sprite = (void *)((((*((u16 *)(context + 0xE44))) * 0x60) + ((u32)context)) + 0x708);
    state = (void *)((((*((u16 *)(context + 0xE44))) * 0x60) + ((u32)context)) + 0x750);
    Step(sprite, state);
    *((struct ContactPosition *)((((*((u16 *)(context + 0xE44))) * 0x60) + ((u32)context)) +
                                 0x724)) = *position;
    {
        u8 *slots = context + 0x708;
        *((u32 *)(((*((u16 *)(context + 0xE44))) * 0x60) + ((u32)slots))) &= ~1;
        *((u32 *)(((*((u16 *)(context + 0xE44))) * 0x60) + ((u32)slots))) |= 2;
    }
    *((u16 *)((((*((u16 *)(context + 0xE44))) * 0x60) + ((u32)context)) + 0x764)) = 8;
    *((u8 *)((context + ((*((u16 *)(context + 0xE44))) * 0x60)) + 0x70E)) = angle;
    t1 = actor->drawable;
    t2 = context + ((*((u16 *)(context + 0xE44))) * 0x60);
    CopyScaleX(t1, t2);
    t3 = actor->drawable;
    t4 = context + ((*((u16 *)(context + 0xE44))) * 0x60);
    CopyScaleY(t3, t4);
    if ((++(*((u16 *)(context + 0xE44)))) > 3)
        *((u16 *)(context + 0xE44)) = 0;
}

#else
INCLUDE_ASM("asm/nonmatching", ActorPhase_EmitTrailSprite);
#endif
