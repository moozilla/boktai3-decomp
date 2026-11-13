#include "actor_attachment_manager.h"
/* ActorAttachment_ReleaseAll: Releases occupied attachment slots and removes their sprites or parts without identity filtering */
#define ActorAttachment_ReleaseAll sub_08066CB8
void Effect_Remove(void *);
static inline u8 IsOccupied(u32 *mask, u32 bit)
{
    u32 bits;
    bits = *mask;
    if (bits & bit)
        return TRUE;
    return FALSE;
}
static inline u32 Occupancy(struct ActorAttachmentManagerView *manager)
{
    return manager->occupiedMask;
}
void ActorAttachment_ReleaseAll(void)
{
    struct ActorAttachmentManagerView *manager;
    struct ActorAttachmentManagerView *incoming = gActorAttachmentManager;
    s32 i;
    u32 *occupied;
    s32 next;
    u8 *scratch;
    if (!incoming)
        return;

    manager = incoming;
    if (!Occupancy(manager))
        return;

    i = 0;
    {
        u32 *low = &manager->occupiedMask;

        occupied = low;
    }
    for (; i <= 7; i++) {
        u32 bit = 1U << i;
        u8 used;
        scratch = (u8 *)occupied;

        used = IsOccupied((u32 *)scratch, bit);
        if (used) {
            struct ActorAttachmentSlotView *slot = &manager->slots[i];
            {
                struct ActorAttachmentPartSprite *part = slot->parts;
                struct ContactSwarmSprite *sprite = &slot->sprite;
                s8 *lowId = &slot->slotId;
                s8 *id;
                s32 index;

                index = *lowId;
                if (index >= 0) {
                    u32 kind;
                    {
                        u32 bit = 1 << index;

                        scratch = (u8 *)occupied;

                        *(u32 *)scratch &= ~bit;
                    }
                    kind = slot->resourceKind;
                    if (kind <= 1 || kind == 4 || kind == 5 || kind == 6) {
                        u8 *active = &slot->active48;
                        u32 present = *active;

                        {
                            s8 *low = &slot->slotId;
                            id = low;
                        }
                        if (present) {
                            Sprite_Remove(sprite);
                            *active = 0;
                        }
                    } else {
                        s32 j = 0;
                        id = lowId;
                        scratch = &slot->partMask;

                    next_part: {
                            {
                                u32 bits = *scratch >> j;
                                u32 bit = 1;

                                if (bits & bit)
                                    Effect_Remove(part);
                            }
                            part++;
                            j++;
                            if (j <= 2)
                                goto next_part;
                        }
                        *scratch = 0;
                    }
                    Memory_Clear(slot, 0x138);
                    {
                        u32 invalid = 255;

                        scratch = (u8 *)id;
                        *scratch = invalid;
                    }
                }
            }
        }
    }
}
