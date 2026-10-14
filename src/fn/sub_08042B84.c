// MATCHING_CONCESSION: register-pinning
#include "contact_actor.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *pool, struct ContactActor *slot,
                                         u32 index)
{
    u8 flip[2];
    register struct ContactActorPool *base asm("r6") = pool;
    register struct ContactActor *actor asm("r5") = slot;
    u32 direction = ((actor->direction + 96) >> 6) & 3;
    register u8 *first asm("r1") = &flip[0];
    register u8 *second asm("r4") = &flip[1];
    u8 *saved;
    u32 check;
    check = (u8)(direction - 1);
    saved = second;
    if (check <= 1)
        *first = 1;
    else
        *first = 0;
    if (direction > 1)
        *second = 1;
    else
        *second = 0;
    Anim_SetSequence(&actor->sequence, base->sequenceResource, (u16)index, flip[0], *saved);
}
