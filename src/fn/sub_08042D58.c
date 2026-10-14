// MATCHING_CONCESSION: register-pinning
#include "contact_actor.h"
#define ContactActor_UpdateAdvance          sub_08042D58
#define ContactActor_SetDirectionalSequence sub_08042B84
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
static inline s32 truncateShift12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void ContactActor_UpdateAdvance(struct ContactActorPool *pool, struct ContactActor *slotInput,
                                s32 indexInput)
{
    struct ContactActor *slot = slotInput;
    s32 index = indexInput;
    struct ContactPosition delta;
    struct ContactPosition *write;
    register u32 rawSpeed asm("r0");
    u32 direction;
    s32 speed;
    if (slot->enter)
        slot->enter = 0;
    rawSpeed = slot->speed;
    asm("" : "+r"(rawSpeed));
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
        {
            const struct ContactPosition *first = &delta;
            slot->position.x += first->x;
        }
        position->y += read->y;
        position->z += read->z;
        Collider_SetPosition(&slot->healthCollider, position, 0);
    }
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    if (slot->stateFrame >= index + 2) {
        slot->state = 1;
        slot->enter = 1;
        slot->stateFrame = 0;
    }
    slot->stateFrame++;
}
