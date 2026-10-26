#include "actor_attachment_slot.h"
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
            u32 flagsOffset;
            u8 *flags;
            u32 idOffset;
            s8 *slotId;
            u8 *active;
            u32 activeOffset;
            u8 *status;
            u32 zero;
            u8 *secondary;
            s16 *threshold;
            s32 *counter;
            i = 0;
            /* offsetof-style constants retain the original separate address births. */
            flagsOffset = (u32) & ((struct ActorAttachmentSlot *)0)->flags31;
            flags = (u8 *)(flagsOffset + (u32)slot);
            idOffset = (u32) & ((struct ActorAttachmentSlot *)0)->slotId;
            slotId = (s8 *)(idOffset + (u32)slot);
            activeOffset = (u32) & ((struct ActorAttachmentSlot *)0)->active;
            active = (u8 *)(activeOffset + (u32)slot);
            status = slot->status;
            zero = 0;
            secondary = slot->secondaryStatus;
            threshold = slot->thresholds;
            counter = slot->counters;
            do {
                status[i] = zero;
                secondary[i] = zero;
                *counter++ = zero;
                *threshold++ = zero;
                i++;
            } while (i <= 2);
            {
                u32 clear = 0;
                u32 bit;
                slot->unknown24 = clear;
                slot->unknown4C = clear;
                {
                    u8 *low = flags;
                    *low = clear;
                }
                {
                    struct ContactSwarmSprite *low = sprite;
                    {
                        u32 value = low->flags;
                        bit = 1;
                        value |= bit;
                        low->flags = value;
                    }
                }
                {
                    s8 *low = slotId;
                    s32 id;
                    id = *low;
                    {
                        struct ActorAttachmentOwner *recovered = owner;
                        u32 *occupied;
                        u32 offset;
                        offset = 0xA1C;
                        occupied = (u32 *)((u8 *)recovered + offset);
                        bit <<= id;
                        *occupied &= ~bit;
                    }
                }
                Sprite_Remove(sprite);
                {
                    u8 *low = active;
                    *low = clear;
                }
                {
                    u32 invalid = 255;
                    u8 *idLow;
                    idLow = (u8 *)slotId;
                    *idLow = invalid;
                }
                return 1;
            }
        }
    } else {
        if (slot->timer > 10 && IsRemoving(slot)) {
            u32 index = (gRandomIndex + 1) & 1023;
            s32 jitter;
            gRandomIndex = index;
            {
                u32 offset = index << 1;
                const u16 *table;
                table = (const u16 *)0x0203B400;
                jitter = Mod(*(const u16 *)(offset + (u32)table), 20);
            }
            {
                struct ContactSwarmSprite *low = sprite;
                u32 x;
                x = (u16)low->position.x - 10;
                low->position.x = x + jitter;
            }
        }
        ActorAttachment_NotifySourceState(slot);
        slot->timer--;
    }
    return 0;
}
