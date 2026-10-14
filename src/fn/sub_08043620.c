// MATCHING_CONCESSION: register-pinning
#include "contact_actor.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
struct ContactReactionGraphics {
    u8 prefix[0xC];
    const void *tileData;
};
static inline s32 truncateShift12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void ContactActor_UpdateReaction(struct ContactActorPool *pool, struct ContactActor *slot,
                                 s32 index)
{
    struct ContactPosition delta;
    register struct ContactPosition *write asm("r3");
    u32 direction;
    u32 speed;
    u32 rawSpeed;
    if (slot->enter) {
        slot->enter = 0;
        {
            register struct ContactCollider *collider asm("r2") = &slot->healthCollider;
            collider->flags |= 4;
        }
        AnimData_SelectTiles(slot->graphics, 0x192);
    }
    rawSpeed = 20 - slot->stateFrame;
    write = &delta;
    direction = slot->direction;
    speed = rawSpeed * 2;
    write->x = truncateShift12(gUnk_086149C4[(direction + 64) & 255] * speed);
    write->y = 0;
    {
        const s16 *entry = gUnk_086149C4 + direction;
        write->z = truncateShift12(*entry * speed);
    }
    {
        struct ContactPosition *position = &slot->position;
        const struct ContactPosition *read = &delta;
        const struct ContactPosition *first = &delta;
        slot->position.x += first->x;
        position->y += read->y;
        position->z += read->z;
    }
    ContactActor_SetDirectionalSequence(pool, slot, 1);
    if (slot->stateFrame > 19) {
        slot->state = 1;
        slot->enter = 1;
        slot->stateFrame = 0;
        slot->sprite.flags &= ~1;
    } else {
        if (slot->stateFrame == 6) {
            struct ContactReactionGraphics *graphics = (void *)slot->graphics;
            {
                void *argument = graphics;
                asm("" : "+r"(argument));
                AnimData_SelectTiles(argument, 0x2D3);
            }
            graphics->tileData = (u8 *)pool + 0x1700;
        }
        if ((slot->stateFrame >> 2) & 1)
            slot->sprite.flags &= ~1;
        else
            slot->sprite.flags |= 1;
        slot->stateFrame++;
    }
}
