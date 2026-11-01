#include "scene_actor_reset.h"
#include "contact_swarm.h"
#include "display_sprite_playback.h"

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

void ActorPhase_EmitTrailSprite(struct SceneActorRegistryResetView *actor,
                                const struct ContactPosition *position, u32 angle)
{
    struct AxeTrailContext *context = actor->context;
    struct ContactSwarmSprite *sprite;
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
    sprite = (struct ContactSwarmSprite *)(context->nextTrail * sizeof(struct AxeTrailSlot) +
                                           (u32)context);
    sprite = (struct ContactSwarmSprite *)((u8 *)sprite + spriteOffset);
    state = (struct DisplaySequenceState *)(context->nextTrail * sizeof(struct AxeTrailSlot) +
                                            (u32)context);
    state = (struct DisplaySequenceState *)((u8 *)state + (u32) &
                                            ((struct AxeTrailContext *)0)->trail[0].sequence);
    PlayerCue_StepSequenceEnded(sprite, state);
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
