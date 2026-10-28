// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_sprite_playback.h"
void ActorAttachment_TickNarrowShake(void *, struct ActorAttachmentSlotView *);
void ActorAttachment_TickAscendingSprite(struct ActorAttachmentSlotView *slot)
{
    if (gActorAttachmentManager) {
        struct ActorAttachmentManagerView *manager = gActorAttachmentManager;
        struct ContactSwarmSprite *sprite = &slot->sprite;
        struct DisplaySequenceState *sequence = slot->sequences;
        u32 ended = ActorAttachment_StepSprite(sprite, sequence);
        register struct ContactPosition *anchor asm("r2");
        struct ContactPosition *ground;
        register u32 height asm("r1");
        register u32 baseY asm("r0");
        register u32 cachedY asm("r9");
        u32 sourceY;
        if (slot->frame <= 1) {
            height = slot->unknown34 - 4;
            slot->unknown34 = height;
            anchor = slot->anchor;
            baseY = (u16)slot->position.z;
            baseY += (u16)anchor->y;
        } else {
            height = slot->unknown34 - 3;
            slot->unknown34 = height;
            anchor = slot->anchor;
            baseY = (u16)slot->position.z;
            {
                register u32 y asm("r6") = (u16)anchor->y;
                baseY += y;
            }
        }
        height += baseY;
        {
            u32 y = (u16)slot->position.y;
            height = y + height;
            slot->position.pad = height;
            sourceY = y;
        }
        ground = anchor;
        cachedY = sourceY;
        {
            register s32 projectedY asm("r0") = slot->position.pad;
            if (projectedY < slot->position.y + ground->y - 16) {
                u32 y = (u16)ground->y;
                y += cachedY;
                slot->position.pad = y - 16;
            }
        }
        if (slot->position.pad > slot->position.y + ground->y || slot->timer > 0) {
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
                sprite->position.y = slot->position.pad;
                {
                    register u32 z asm("r0") = (u16)slot->offset.z;
                    register u32 spriteZ asm("r6") = (u16)sprite->position.z;
                    sprite->position.z = z + spriteZ;
                }
            }
            ActorAttachment_TickNarrowShake(manager, slot);
            slot->counters[0]++;
        }
    }
}
