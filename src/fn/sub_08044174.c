#include "contact_swarm.h"
void ContactSwarm_SetDirectionalSequence(struct ContactSwarm *pool, struct ContactSwarmSlot *slot,
                                         u32 index)
{
    u8 flip[2];
    u32 direction = ((slot->direction + 96) >> 6) & 3;
    u8 *first = &flip[0];
    u8 *second = &flip[1];
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
    Anim_SetSequence(&slot->display.animation.sequence, pool->sequenceResource, (u16)index, flip[0],
                     *saved);
}
