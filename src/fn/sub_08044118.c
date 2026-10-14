#include "contact_swarm.h"
#define ContactSwarm_DestroySlot sub_08044118
void sub_08013684(void *);
void ContactSwarm_DestroySlot(struct ContactSwarm *pool, struct ContactSwarmSlot *slot, s32 index)
{
    Collider_Remove(&slot->healthCollider);
    sub_08013684(slot->shadow);
    Sprite_Remove(&slot->animation.sprite);
}
