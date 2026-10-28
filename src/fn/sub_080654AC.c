// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_sprite_playback.h"
void ActorAttachment_TickWideShake(void *, struct ActorAttachmentSlotView *);
void ActorAttachment_TickDescendingSprite(struct ActorAttachmentSlotView *slot)
{
    if (gActorAttachmentManager) {
        struct ActorAttachmentManagerView *manager = gActorAttachmentManager;
        struct ContactSwarmSprite *sprite = &slot->sprite;
        struct DisplaySequenceState *sequence = slot->sequences;
        register u32 ended asm("r8") = ActorAttachment_StepSprite(sprite, sequence);
        register struct ContactPosition *anchor asm("r2");
        struct ContactPosition *ground;
        register u32 height asm("r0");
        register u32 baseY asm("r1");
        if (slot->frame <= 1) {
            height = slot->unknown34 + 25;
            slot->unknown34 = height;
            anchor = slot->anchor;
            baseY = (u16)slot->position.z;
            baseY += (u16)anchor->y;
        } else {
            height = slot->unknown34 + 10;
            slot->unknown34 = height;
            anchor = slot->anchor;
            baseY = (u16)slot->position.z;
            {
                register u32 y asm("r6") = (u16)anchor->y;
                baseY += y;
            }
        }
        height += baseY;
        slot->position.pad = height;
        ground = anchor;
        if (slot->position.pad > slot->position.y + ground->y) {
            /* Preserve the observed anchor halfword reread before slot Y. */
            u32 y = *(volatile u16 *)&ground->y;
            y += (u16)slot->position.y;
            slot->position.pad = y;
        }
        if (slot->position.pad < slot->position.y + ground->y || slot->timer > 0) {
            u32 zero = 0;
            asm("" : "+r"(zero));
            sequence->index = zero;
            sequence->elapsed = zero;
        }
        if (slot->counters[0] <= 29) {
            u32 zero = 0;
            sequence->index = zero;
            sequence->elapsed = zero;
        }
        if (ended) {
            slot->flags31 = 0;
            {
                s32 id = slot->slotId;
                u32 *occupied;
                u32 one;
                occupied = &manager->occupiedMask;
                one = 1;
                *occupied &= ~(one << id);
                sprite->flags |= one;
            }
            slot->status[0] = 0;
            slot->update = NULL;
            slot->counters[0] = 0;
            Sprite_Remove(sprite);
            slot->active48 = 0;
            slot->slotId = -1;
        } else {
            sprite->flags &= ~1;
            if (slot->anchor) {
                sprite->position = *slot->anchor;
                {
                    register u32 x asm("r0") = (u16)slot->offset.x;
                    register u32 spriteX asm("r3") = (u16)sprite->position.x;
                    sprite->position.x = x + spriteX;
                }
                {
                    u32 y = (u16)slot->offset.y;
                    register u32 projectedY asm("r6") = (u16)slot->position.pad;
                    y += projectedY;
                    sprite->position.y = y;
                }
                {
                    u32 z = (u16)slot->offset.z;
                    u32 spriteZ = (u16)sprite->position.z;
                    sprite->position.z = z + spriteZ;
                }
            }
            ActorAttachment_TickWideShake(manager, slot);
            slot->counters[0]++;
        }
    }
}
