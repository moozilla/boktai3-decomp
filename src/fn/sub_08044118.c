#include "contact_swarm.h"
void sub_08013684(void *);
void ContactSwarm_DestroySlot(struct ContactSwarm *pool, struct ContactSwarmSlot *slot, s32 index)
{
    Collider_Remove(&slot->display.healthCollider);
    sub_08013684(slot->shadow);
    Sprite_Remove(&slot->display.animation.draw.sprite);
}
