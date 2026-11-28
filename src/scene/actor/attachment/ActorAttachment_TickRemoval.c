#include "scene/actor/actor_attachment_slot.h"
static inline u8 IsRemoving(struct ActorAttachmentSlot *slot)
{
    u32 bit = 1;
    if (slot->status[0] & bit)
        return TRUE;
    return FALSE;
}
u8 ActorAttachment_TickRemoval(void *owner, void *slotInput)
{
    struct ActorAttachmentSlot *slot = slotInput;
    struct ContactSwarmSprite *sprite = &slot->sprite;

    if (slot->timer <= 0) {
        if (IsRemoving(slot)) {
            s32 i;
            s32 id;

            for (i = 0; i < 3; i++) {
                slot->status[i] = 0;
                slot->secondaryStatus[i] = 0;
                slot->counters[i] = 0;
                slot->thresholds[i] = 0;
            }
            slot->unknown24 = 0;
            slot->unknown4C = 0;
            slot->flags31 = 0;
            sprite->flags |= 1;
            id = slot->slotId;
            ((struct ActorAttachmentOwner *)owner)->occupiedSlots &= ~(1 << id);
            Sprite_Remove(sprite);
            slot->active = 0;
            slot->slotId = -1;
            return 1;
        }
    } else {
        if (slot->timer > 10 && IsRemoving(slot)) {
            gRandomIndex = (gRandomIndex + 1) & 1023;
            sprite->position.x += Mod(((u16 *)RANDOM_TABLE_ADDR)[gRandomIndex], 20) - 10;
        }
        ActorAttachment_NotifySourceState(slot);
        slot->timer--;
    }
    return 0;
}
