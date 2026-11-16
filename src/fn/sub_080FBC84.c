#include "scene_actor_reset.h"
#include "contact_swarm.h"
#include "display_sprite_playback.h"

struct AxeTrailSlotEmitTrailSprite {
    union {
        struct DisplaySequenceSprite sprite;
        struct ContactSwarmSprite spatial;
    } draw;
    struct ContactSwarmGraphics graphics;
    struct DisplaySequenceState sequence;
    const void *resource;
    u16 lifetime, unknown5E;
};
struct AxeTrailContextEmitTrailSprite {
    u8 unknown0[0x708];
    struct AxeTrailSlotEmitTrailSprite trail[4];
    u8 unknown888[0xE44 - 0x888];
    u16 nextTrail, spacing;
    u8 unknownE48[0xED7 - 0xE48];
    u8 variant;
};
s32 Mod(s32, s32);

static inline const void *Axe_TrailResource(struct AxeTrailContextEmitTrailSprite *context, u32 index)
{
    return context->trail[index].resource;
}
static inline void Axe_CopyTrailScaleX(struct DisplaySequenceSprite *source,
                                       struct AxeTrailContextEmitTrailSprite *row)
{
    row->trail[0].draw.sprite.palette = source->palette;
}
static inline void Axe_CopyTrailScaleY(struct DisplaySequenceSprite *source,
                                       struct AxeTrailContextEmitTrailSprite *row)
{
    row->trail[0].draw.sprite.paletteVariant = source->paletteVariant;
}

void ActorPhase_EmitTrailSprite(struct ChaseActor *actor, const struct ContactPosition *position,
                                u32 angle)
{
    struct AxeTrailContextEmitTrailSprite *context = actor->base.context;
    struct ContactSwarmSprite *sprite;
    struct DisplaySequenceState *state;
    u32 spriteOffset;
    if (((struct DisplaySequenceSprite *)actor->base.drawable)->flags & 1)
        return;
    {
        s32 zero = Mod(actor->base.frameCounter, context->spacing);
        if (zero)
            return;
        {
            struct DisplaySequenceState *source =
                &((struct SceneActorControlMotionDrawable *)actor->base.drawable)->sequence;
            u16 selection = source->unknownA;
            u32 indexOffset = context->nextTrail * sizeof(struct AxeTrailSlotEmitTrailSprite);
            struct AxeTrailSlotEmitTrailSprite *slot;
            struct DisplaySequenceState *destination;
            const void *resource;
            spriteOffset = (u32) & ((struct AxeTrailContextEmitTrailSprite *)0)->trail[0];
            slot = (struct AxeTrailSlotEmitTrailSprite *)((u8 *)context + (indexOffset + spriteOffset));
            destination = &slot->sequence;
            resource = Axe_TrailResource(context, context->nextTrail);
            Anim_SetSequence(destination, resource, selection, context->variant, zero);
        }
    }
    sprite = (struct ContactSwarmSprite *)(context->nextTrail * sizeof(struct AxeTrailSlotEmitTrailSprite) +
                                           (u32)context);
    sprite = (struct ContactSwarmSprite *)((u8 *)sprite + spriteOffset);
    state = (struct DisplaySequenceState *)(context->nextTrail * sizeof(struct AxeTrailSlotEmitTrailSprite) +
                                            (u32)context);
    state = (struct DisplaySequenceState *)((u8 *)state + (u32) &
                                            ((struct AxeTrailContextEmitTrailSprite *)0)->trail[0].sequence);
    PlayerCue_StepSequenceEnded(sprite, state);
    context->trail[context->nextTrail].draw.spatial.position = *position;
    context->trail[context->nextTrail].draw.sprite.flags &= ~1;
    context->trail[context->nextTrail].draw.sprite.flags |= 2;
    context->trail[context->nextTrail].lifetime = 8;
    context->trail[context->nextTrail].draw.sprite.attribute = angle;
    {
        struct DisplaySequenceSprite *source = actor->base.drawable;
        struct AxeTrailContextEmitTrailSprite *row =
            (void *)((u8 *)context + context->nextTrail * sizeof(struct AxeTrailSlotEmitTrailSprite));
        Axe_CopyTrailScaleX(source, row);
    }
    {
        struct DisplaySequenceSprite *source = actor->base.drawable;
        struct AxeTrailContextEmitTrailSprite *row =
            (void *)((u8 *)context + context->nextTrail * sizeof(struct AxeTrailSlotEmitTrailSprite));
        Axe_CopyTrailScaleY(source, row);
    }
    if (++context->nextTrail > 3)
        context->nextTrail = 0;
}
