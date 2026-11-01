#include "player_cue_sequence.h"
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
struct AxeTrailSlot {
    struct TrailSprite sprite;
    u8 unknown24[0x24];
    struct DisplaySequenceState sequence;
    void *commands;
    u16 remaining;
    u8 unknown5E[2];
};
struct AxeTrailContext {
    u8 unknown0[0x708];
    struct AxeTrailSlot slots[4];
    u8 unknown888[0x5BC];
    u16 nextSlot, interval;
    u8 unknownE48[0x8F];
    u8 speed;
};
s32 Mod(s32, s32);

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
void Axe_EmitMotionTrail(struct SceneActorRegistryResetView *actor)
{
    struct AxeTrailContext *trail = actor->context;
    u8 *context = (void *)trail;
    struct TrailSprite *sprite;
    void *sourceDrawable;
    u8 *slotBase;
    void *scaleDrawable;
    u8 *scaleSlotBase;
    if ((*((u32 *)actor->drawable)) & 1)
        return;
    {
        s32 zero = Mod(actor->field2CA, trail->interval);
        if (zero)
            return;
        {
            struct SceneActorControlMotionDrawable *drawable = actor->drawable;
            struct DisplaySequenceState *source = &drawable->sequence;
            u16 sequence = source->unknownA;
            {
                u32 offset = trail->nextSlot * sizeof(struct AxeTrailSlot);
                u32 spriteOffset = 0x708;
                u8 *destination = context + (offset + spriteOffset);
                u8 *sequenceState = destination + 0x48;
                u32 sequenceOffset = 0x760;
                void *commands = *(void **)(context + sequenceOffset + offset);
                u8 speed = trail->speed;
                Anim_SetSequence((void *)sequenceState, commands, sequence, speed, zero);
            }
        }
    }
    sprite = (void *)((trail->nextSlot * sizeof(struct AxeTrailSlot)) + (u32)context);
    sprite = (void *)((u8 *)sprite + 0x708);
    PlayerCue_StepSequence((void *)sprite, &trail->slots[trail->nextSlot].sequence);
    {
        u32 offset = ((struct AxeTrailContext *)context)->nextSlot * sizeof(struct AxeTrailSlot);
        u8 *slot = context + offset;
        *((struct ContactPosition *)(slot + 0x724)) = actor->body.position;
    }
    {
        u32 offset = ((struct AxeTrailContext *)context)->nextSlot * sizeof(struct AxeTrailSlot);
        u8 *slots = context + 0x708;
        *((u32 *)(slots + offset)) &= ~1;
        slots += ((struct AxeTrailContext *)context)->nextSlot * sizeof(struct AxeTrailSlot);
        *((u32 *)slots) |= 2;
    }
    {
        u32 offset = ((struct AxeTrailContext *)context)->nextSlot * sizeof(struct AxeTrailSlot);
        u8 *slot = context + offset;
        *((u16 *)(slot + 0x764)) = 8;
    }
    sourceDrawable = actor->drawable;
    slotBase =
        context + ((((struct AxeTrailContext *)context)->nextSlot) * sizeof(struct AxeTrailSlot));
    CopyAngle(sourceDrawable, slotBase);
    scaleDrawable = actor->drawable;
    scaleSlotBase =
        context + ((((struct AxeTrailContext *)context)->nextSlot) * sizeof(struct AxeTrailSlot));
    CopyScaleX(scaleDrawable, scaleSlotBase);
    sourceDrawable = actor->drawable;
    slotBase =
        context + ((((struct AxeTrailContext *)context)->nextSlot) * sizeof(struct AxeTrailSlot));
    CopyScaleY(sourceDrawable, slotBase);
    if ((++(((struct AxeTrailContext *)context)->nextSlot)) > 3)
        ((struct AxeTrailContext *)context)->nextSlot = 0;
}
