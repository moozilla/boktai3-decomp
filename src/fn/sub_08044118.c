#include "contact_swarm.h"
void Shadow_Destroy(void *);
void ContactSwarm_DestroySlot(struct ContactSwarm *pool, struct ContactSwarmSlot *slot, s32 index)
{
    Collider_Remove(&slot->display.healthCollider);
    Shadow_Destroy(slot->shadow);
    Sprite_Remove(&slot->display.animation.draw.sprite);
}
