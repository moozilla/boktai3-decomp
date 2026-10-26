// MATCHING_CONCESSION: register-pinning
#include "contact_sequence_actor.h"
void ContactSequenceActor_SetFlags(struct ContactSequenceActor *, u32);
u32 ContactSequenceActor_GetFlags(struct ContactSequenceActor *, u32);
extern void (*const gUnk_086059A4[])(struct ContactSequenceOwner *, struct ContactSequenceActor *);
extern const u32 gUnk_0860597C[];
void ContactSequenceActor_Update(struct ContactSequenceOwner *parent,
                                 struct ContactSequenceActor *actor)
{
    struct ContactPosition jitter;
    actor->flags = 0;
    sub_08249240(parent, actor, (void *)gUnk_086059A4[actor->state]);
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
        u32 result;
        u32 flags;
        flags = collider->flags;
        result = 4;
        result |= flags;
        collider->flags = result;
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
            u32 index = (gRandomIndex + 1) & 1023;
            u32 direction;
            register u32 amplitude asm("r6");
            struct ContactPosition *offset;
            const u8 *sample;
            register u32 flash asm("r1");
            u32 angleMask;
            const s16 *sine;
            register s32 x asm("r2");
            s32 value;
            gRandomIndex = index;
            sample = (const u8 *)RANDOM_TABLE_ADDR + index * 2;
            angleMask = 255;
            flash = actor->flash;
            asm("" : "+r"(flash));
            offset = &jitter;
            direction = *sample;
            amplitude = flash * 4;
            sine = gUnk_086149C4;
            value = sine[(direction + 64) & angleMask] * amplitude;
            if (value >= 0)
                x = value >> 12;
            else {
                value = -value;
                value >>= 12;
                x = -value;
            }
            {
                u32 zero = 0;
                offset->x = x;
                offset->y = zero;
            }
            value = sine[direction] * amplitude;
            if (value >= 0)
                offset->z = value >> 12;
            else
                offset->z = -((-value) >> 12);
            {
                struct ContactPosition *destination = &actor->sprite.position;
                struct ContactPosition *source;
                struct ContactPosition *firstSource;
                register u32 coordinate asm("r0");
                u32 delta;
                register u32 lastDelta asm("r3");
                source = &jitter;
                firstSource = &jitter;
                asm("" : "+r"(firstSource));
                coordinate = (u16)actor->position.x;
                delta = (u16)firstSource->x;
                destination->x = coordinate + delta;
                coordinate = (u16)source->y;
                delta = (u16)actor->position.y;
                destination->y = coordinate + delta;
                coordinate = (u16)actor->position.z;
                lastDelta = (u16)source->z;
                destination->z = coordinate + lastDelta;
            }
        }
    }
    actor->healthCollider.cost = 0;
    ++actor->stateFrame;
}
