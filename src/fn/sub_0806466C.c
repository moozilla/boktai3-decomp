// MATCHING_CONCESSION: register-pinning
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
            register u32 flagsOffset asm("r2");
            register u8 *flags asm("ip");
            register u32 idOffset asm("r3");
            register s8 *slotId asm("r9");
            u8 *active;
            u32 activeOffset;
            u8 *status;
            register u32 zero asm("r3");
            register u8 *secondary asm("r6");
            s16 *threshold;
            register s32 *counter asm("r1");
            i = 0;
            /* offsetof-style constants retain the original separate address births. */
            flagsOffset = (u32) & ((struct ActorAttachmentSlot *)0)->flags31;
            asm("" : "+r"(flagsOffset));
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
                    register u8 *low asm("r1") = flags;
                    *low = clear;
                }
                {
                    register struct ContactSwarmSprite *low asm("r2") = sprite;
                    {
                        u32 value = low->flags;
                        bit = 1;
                        value |= bit;
                        low->flags = value;
                    }
                }
                {
                    register s8 *low asm("r3") = slotId;
                    s32 id;
                    id = *low;
                    {
                        register struct ActorAttachmentOwner *recovered asm("r5") = owner;
                        register u32 *occupied asm("r2");
                        u32 offset;
                        asm("" : "+r"(recovered));
                        offset = 0xA1C;
                        occupied = (u32 *)((u8 *)recovered + offset);
                        bit <<= id;
                        *occupied &= ~bit;
                    }
                }
                Sprite_Remove(sprite);
                {
                    register u8 *low asm("r5") = active;
                    *low = clear;
                }
                {
                    u32 invalid = 255;
                    register u8 *idLow asm("r1");
                    idLow = (u8 *)slotId;
                    *idLow = invalid;
                }
                return 1;
            }
        }
    } else {
        if (slot->timer > 10 && IsRemoving(slot)) {
            u32 index = (gUnk_03005308 + 1) & 1023;
            s32 jitter;
            gUnk_03005308 = index;
            {
                u32 offset = index << 1;
                register const u16 *table asm("r2");
                table = gUnk_0203B400;
                jitter = Mod(*(const u16 *)(offset + (u32)table), 20);
            }
            {
                register struct ContactSwarmSprite *low asm("r3") = sprite;
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
