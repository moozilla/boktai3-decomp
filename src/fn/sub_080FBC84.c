#include "scene_actor_reset.h"
#include "contact_swarm.h"
#include "display_sequence_inline.h"

struct AxeTrailSlot {
    union {
        struct DisplaySequenceSprite sprite;
        struct ContactSwarmSprite spatial;
    } draw;
    struct ContactSwarmGraphics graphics;
    struct DisplaySequenceState sequence;
    const void *resource;
    u16 lifetime, unknown5E;
};
struct AxeTrailContext {
    u8 unknown0[0x708];
    struct AxeTrailSlot trail[4];
    u8 unknown888[0xE44 - 0x888];
    u16 nextTrail, spacing;
    u8 unknownE48[0xED7 - 0xE48];
    u8 variant;
};
s32 Mod(s32, s32);

static inline const void *Axe_TrailResource(struct AxeTrailContext *context, u32 index)
{
    return context->trail[index].resource;
}
static inline void Axe_CopyTrailScaleX(struct DisplaySequenceSprite *source,
                                       struct AxeTrailContext *row)
{
    row->trail[0].draw.sprite.palette = source->palette;
}
static inline void Axe_CopyTrailScaleY(struct DisplaySequenceSprite *source,
                                       struct AxeTrailContext *row)
{
    row->trail[0].draw.sprite.paletteVariant = source->paletteVariant;
}

#define AXE_CAPTURE_FRAME_COMMAND(command, table, offset)                                          \
    do {                                                                                           \
        (command) = (const u16 *)((u32)(table) + (offset));                                        \
    } while (0)
#define AXE_CAPTURE_NEXT_COMMAND(command, state)                                                   \
    do {                                                                                           \
        (command) = (state)->commands + (state)->index;                                            \
    } while (0)
static inline u32 Axe_StepSequence(struct DisplaySequenceSprite *owner,
                                   struct DisplaySequenceState *state)
{
    const u16 *command;
    u32 ended;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        AXE_CAPTURE_FRAME_COMMAND(command, table, offset);
    }
    owner->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
                ended = 1;
            } else {
                state->index--;
                ended = 0;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
                ended = 1;
            } else {
                ended = 0;
            }
        }
        AXE_CAPTURE_NEXT_COMMAND(command, state);
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    } else {
        ended = 0;
    }
    return ended;
}

void ActorPhase_EmitTrailSprite(struct SceneActorRegistryResetView *actor,
                                const struct ContactPosition *position, u32 angle)
{
    struct AxeTrailContext *context = actor->context;
    struct DisplaySequenceSprite *sprite;
    struct DisplaySequenceState *state;
    u32 spriteOffset;
    if (((struct DisplaySequenceSprite *)actor->drawable)->flags & 1)
        return;
    {
        s32 zero = Mod(actor->field2CA, context->spacing);
        if (zero)
            return;
        {
            struct DisplaySequenceState *source =
                &((struct SceneActorControlMotionDrawable *)actor->drawable)->sequence;
            u16 selection = source->unknownA;
            u32 indexOffset = context->nextTrail * sizeof(struct AxeTrailSlot);
            struct AxeTrailSlot *slot;
            struct DisplaySequenceState *destination;
            const void *resource;
            spriteOffset = (u32) & ((struct AxeTrailContext *)0)->trail[0];
            slot = (struct AxeTrailSlot *)((u8 *)context + (indexOffset + spriteOffset));
            destination = &slot->sequence;
            resource = Axe_TrailResource(context, context->nextTrail);
            Anim_SetSequence(destination, resource, selection, context->variant, zero);
        }
    }
    sprite = (struct DisplaySequenceSprite *)(context->nextTrail * sizeof(struct AxeTrailSlot) +
                                              (u32)context);
    sprite = (struct DisplaySequenceSprite *)((u8 *)sprite + spriteOffset);
    state = (struct DisplaySequenceState *)(context->nextTrail * sizeof(struct AxeTrailSlot) +
                                            (u32)context);
    state = (struct DisplaySequenceState *)((u8 *)state + (u32) &
                                            ((struct AxeTrailContext *)0)->trail[0].sequence);
    Axe_StepSequence(sprite, state);
    context->trail[context->nextTrail].draw.spatial.position = *position;
    context->trail[context->nextTrail].draw.sprite.flags &= ~1;
    context->trail[context->nextTrail].draw.sprite.flags |= 2;
    context->trail[context->nextTrail].lifetime = 8;
    context->trail[context->nextTrail].draw.sprite.attribute = angle;
    {
        struct DisplaySequenceSprite *source = actor->drawable;
        struct AxeTrailContext *row =
            (void *)((u8 *)context + context->nextTrail * sizeof(struct AxeTrailSlot));
        Axe_CopyTrailScaleX(source, row);
    }
    {
        struct DisplaySequenceSprite *source = actor->drawable;
        struct AxeTrailContext *row =
            (void *)((u8 *)context + context->nextTrail * sizeof(struct AxeTrailSlot));
        Axe_CopyTrailScaleY(source, row);
    }
    if (++context->nextTrail > 3)
        context->nextTrail = 0;
}
