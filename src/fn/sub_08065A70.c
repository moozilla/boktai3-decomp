// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_state.h"
void ActorAttachment_TickWideShake(void *, struct ActorAttachmentSlotView *);
void ActorAttachment_TickLandingBounce(struct ActorAttachmentSlotView *slot)
{
    struct ActorAttachmentManagerView *manager = gActorAttachmentManager;
    if (manager) {
        struct ContactSwarmSprite *sprite = &slot->sprite;
        sprite->flags &= ~1;
        sprite->frame = slot->frame;
        if (slot->anchor) {
            sprite->position = *slot->anchor;
            if (slot->counters[0] <= 3) {
                slot->unknown34 += 20;
                {
                    struct ContactPosition *anchor = slot->anchor;
                    u32 z = (u16)slot->position.z;
                    z += (u16)anchor->y;
                    slot->position.pad = slot->unknown34 + z;
                }
                sprite->position.y = slot->position.pad;
            } else if (slot->counters[0] <= 9) {
                slot->timer = 1;
                {
                    struct ContactPosition *anchor = slot->anchor;
                    u32 z;
                    z = (u16)slot->position.z;
                    z += (u16)anchor->y;
                    {
                        u32 height = slot->unknown34;
                        asm("" : "+r"(height));
                        height += z;
                        sprite->position.y = height;
                    }
                }
                slot->status[0] |= 1;
                ActorAttachment_TickWideShake(manager, slot);
            } else {
                s32 y = slot->anchor->y + slot->position.z;
                s32 wave;
                u8 *phase;
                const s16 *table;
                y += (s16)slot->unknown34;
                table = gUnk_086149C4;
                phase = slot->phase;
                wave = table[*phase];
                sprite->position.y = y + (wave >> 8);
                *phase += 4;
            }
            sprite->position.x += slot->offset.x;
            sprite->position.y += slot->offset.y;
            sprite->position.z += slot->offset.z;
            if (slot->counters[0] <= 9)
                slot->counters[0]++;
        }
    }
}
