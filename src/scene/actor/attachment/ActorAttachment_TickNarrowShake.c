#include "scene/actor/actor_attachment_state.h"
void ActorAttachment_TickNarrowShake(void *unused, struct ActorAttachmentSlotView *slot)
{
    struct ContactSwarmSprite *sprite = &slot->sprite;

    if (slot->timer <= 0) {
        u32 shaking = 1;
        if (ActorAttachment_TestByteFlags(&slot->status[0], shaking)) {
            s32 mask = ~1;
            slot->status[0] &= mask;
        }
    } else {
        u32 shaking = 1;
        if (ActorAttachment_TestByteFlags(&slot->status[0], shaking)) {
            gRandomIndex = (gRandomIndex + 1) & 0x3FF;
            sprite->position.x += Mod(((u16 *)RANDOM_TABLE_ADDR)[gRandomIndex], 2) - 1;
        }
        slot->timer--;
    }
}
