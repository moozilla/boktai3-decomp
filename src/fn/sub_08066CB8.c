// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_manager.h"
/* ActorAttachment_ReleaseAll: Releases occupied attachment slots and removes their sprites or parts without identity filtering */
#define ActorAttachment_ReleaseAll sub_08066CB8
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
void ActorAttachment_ReleaseAll(void)
{
    struct ActorAttachmentManagerView *manager;
    struct ActorAttachmentManagerView *incoming = gActorAttachmentManager;
    register s32 i asm("r3");
    register u32 *occupied asm("r10");
    s32 next;
    u8 *scratch;
    if (!incoming)
        return;

    manager = incoming;
    {
        u32 *test = &manager->occupiedMask;

        if (!*test)
            return;
    }
    asm("" : "+m"(manager));
    i = 0;
    {
        register u32 *low asm("r2") = &manager->occupiedMask;
        asm("" : "+r"(low));
        occupied = low;
    }
    do {
        u32 bit = 1U << i;
        u8 used;
        scratch = (u8 *)occupied;

        used = IsOccupied((u32 *)scratch, bit);
        {
            s32 lowNext = i + 1;
            next = lowNext;
        }
        if (used) {
            register u32 offset asm("r0") =
                (u32) & ((struct ActorAttachmentManagerView *)0)->slots[i];
            register struct ActorAttachmentManagerView *base asm("r2");
            register struct ActorAttachmentSlotView *slot asm("r6");

            base = manager;

            slot = (struct ActorAttachmentSlotView *)((u8 *)base + offset);
            {
                register struct ActorAttachmentPartSprite *part asm("r4") = slot->parts;
                struct ContactSwarmSprite *sprite = &slot->sprite;
                s8 *lowId = &slot->slotId;
                register s8 *id asm("r8");
                s32 index;

                index = *lowId;
                if (index >= 0) {
                    u32 kind;
                    {
                        register u32 bit asm("r1") = 1 << index;

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
                        next = ++i;
                        if (present) {
                            Sprite_Remove(sprite);
                            *active = 0;
                        }
                    } else {
                        register s32 j asm("r5") = 0;
                        id = lowId;
                        scratch = &slot->partMask;

                        do {
                            {
                                u32 bits = *scratch >> j;
                                register u32 bit asm("r2") = 1;

                                if (bits & bit)
                                    Effect_Remove(part);
                            }
                            part++;
                            j++;
                        } while (j <= 2);
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
        i = next;
    } while (i <= 7);
}
