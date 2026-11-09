#include "contact_sequence_actor.h"
void ContactSequenceActor_SetFlags(struct ContactSequenceActor *, u32);
u32 ContactSequenceActor_GetFlags(struct ContactSequenceActor *, u32);
extern void (*const gUnk_086059A4[])(struct ContactSequenceOwner *, struct ContactSequenceActor *);
static inline s32 scaleOffset(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
extern const u32 gUnk_0860597C[];
void ContactSequenceActor_Update(struct ContactSequenceOwner *parent,
                                 struct ContactSequenceActor *actor)
{
    struct ContactPosition jitter;
    actor->flags = 0;
    gUnk_086059A4[actor->state](parent, actor);
    if (actor->cooldown) {
        AnimData_SelectPaletteBank(&actor->graphics, 0x192);
        ContactSequenceActor_SetFlags(actor, 1);
        --actor->cooldown;
    } else {
        u32 tile = gUnk_0860597C[actor->sequence.index];
        AnimData_SelectPaletteBank(&actor->graphics, tile);
        if (tile == 0x165)
            actor->graphics.paletteData = parent->palette;
    }
    if (actor->sequence.index <= 2)
        actor->sprite.priority = 3;
    else
        actor->sprite.priority = 2;
    if (ContactSequenceActor_GetFlags(actor, 1)) {
        struct ContactCollider *collider = &actor->healthCollider;
        collider->flags |= 4;
    } else {
        struct ContactCollider *collider = &actor->healthCollider;
        s32 mask = -5;
        collider->flags &= mask;
    }
    if (actor->flash) {
        --actor->flash;
        if (!actor->flash) {
            actor->sprite.position = actor->position;
        } else {
            struct ContactPosition *offset;
            u32 index = (gRandomIndex + 1) & 1023;
            const u8 *sample;
            u32 mask;
            u32 flash;
            u32 direction;
            u32 amplitude;
            gRandomIndex = index;
            sample = (const u8 *)RANDOM_TABLE_ADDR + index * 2;
            mask = 255;
            flash = actor->flash;
            offset = &jitter;
            direction = *sample;
            amplitude = flash * 4;
            offset->x = scaleOffset(gUnk_086149C4[(direction + 64) & mask] * amplitude);
            offset->y = 0;
            offset->z = scaleOffset(gUnk_086149C4[direction] * amplitude);
            {
                struct ContactPosition *destination = &actor->sprite.position;
                const struct ContactPosition *source = &jitter;
                const struct ContactPosition *first = &jitter;
                destination->x = first->x + actor->position.x;
                destination->y = actor->position.y + source->y;
                destination->z = source->z + actor->position.z;
            }
        }
    }
    actor->healthCollider.cost = 0;
    ++actor->stateFrame;
}
