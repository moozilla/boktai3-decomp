// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_rising_playback.h"
void ActorAttachment_TickRisingPart(struct ActorAttachmentSlotView *slot)
{
    s8 *finalSlotId;
    if (gUnk_02000124) {
        struct ActorAttachmentManagerView *manager = gUnk_02000124;
        struct ActorAttachmentPartSprite *sprite = slot->parts;
        struct DisplaySequenceState *sequence = slot->sequences;
        const struct ActorAttachmentPartResource *resource;
        u8 *phase;
        register u8 *cleanupPhase asm("r6");
        u32 random;
        u32 randomMask;
        {
            u32 kind = slot->kind;
            if (kind == 2)
                goto resource;
            if (kind == 3)
                goto resource;
            asm("" ::: "r3");
            resource = NULL;
            goto playback;
        resource:
            resource = manager->partResource;
        }
    playback:
        ActorAttachment_StepRisingPart(sprite, sequence, resource);
        {
            struct ActorAttachmentSlotView *view = slot;
            s32 radius, wave;
            asm("" : "+l"(view) : : "r0", "r1");
            radius = view->counters[0] >> 1;
            wave = gUnk_086149C4[slot->phase[0]];
            slot->jitter[0].x = (wave * radius) >> 12;
        }
        slot->jitter[0].y += 2;
        random = gRandomIndex + 1;
        randomMask = 0x3FF;
        random &= randomMask;
        gRandomIndex = random;
        phase = slot->phase;
        {
            register const u16 *table asm("r2");
            s32 step;
            u32 previous;
            random <<= 1;
            table = (const u16 *)gUnk_0203B400;
            step = *(u16 *)(random + (u32)table);
            step >>= 4;
            step &= 7;
            previous = *phase;
            asm("" : : "r"(previous));
            step += previous;
            *phase = step;
        }
        {
            struct ActorAttachmentSlotView *view = slot;
            sprite->position.x = view->anchor->x + view->jitter[0].x;
            sprite->position.y = view->anchor->y + view->jitter[0].y + view->position.z;
            sprite->position.z = view->anchor->z - view->jitter[0].x;
            sprite->flags &= ~1;
            {
                s32 height = view->jitter[0].y;
                cleanupPhase = phase;
                asm("" : "+&r"(cleanupPhase) : "r"(phase));
                if (height <= 60)
                    sequence->index = 0;
                else if (height <= 120)
                    sequence->index = 1;
                else
                    sequence->index = 2;
            }
        }
        if (slot->jitter[0].y > 210) {
            s32 i;
            s8 *slotId;
            u8 *partMask;
            u32 one;
            register s8 *savedSlotId asm("r9");
            slot->flags31 = 0;
            slotId = &slot->slotId;
            asm("" : : "l"(slotId) : "r0", "r1", "r2", "r3");
            {
                s32 id = *slotId;
                struct ActorAttachmentManagerView *owner = manager;
                u32 *occupied;
                u32 bit;
                occupied = &owner->occupiedMask;
                one = 1;
                bit = one << id;
                *occupied &= ~bit;
            }
            sprite->flags |= one;
            i = 0;
            savedSlotId = slotId;
            partMask = &slot->partMask;
            asm("" : : "l"(i), "r"(slotId), "r"(savedSlotId), "r"(partMask) : "r0", "r1", "r2");
            finalSlotId = savedSlotId;
            for (; i <= 2; i++) {
                slot->status[i] = 0;
                cleanupPhase[i] = 0;
                slot->counters[i] = 0;
                slot->thresholds[i] = 0;
            }
            slot->update = NULL;
            Effect_Remove(sprite);
            *partMask = 0;
            *(u8 *)finalSlotId = 0xFF;
        } else {
            struct ActorAttachmentSlotView *view = slot;
            view->counters[0]++;
        }
    }
}
