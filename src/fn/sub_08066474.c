// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_manager.h"
void ParticleSprite_Initialize(void *, const void *, s32);
void Effect_SetOrigin(void *, s32, s32);
struct ActorAttachmentSlotView *ActorAttachment_CreateMeterPartsSlot(
    u32 incomingFrame, struct ContactPosition *anchor, void *update, s32 z, u32 action, s16 *value,
    s32 total, u32 partCount, struct ActorAttachmentFollowView *follow, u32 sourceWord)
{
    struct ActorAttachmentManagerView *manager;
    s32 savedZ;
    void *savedUpdate = update;
    struct ActorAttachmentManagerView *existing;
    u32 frame;
    struct ActorAttachmentSlotView *slot;
    struct ActorAttachmentPartSprite *parts;
    s32 step;
    s32 scaledIndex;
    u8 *framePointer, *actionPointer, *phasePointer, *maskPointer, *kindPointer, *resourcePointer;
    s16 *thresholdPointer;
    s32 *counterPointer;
    s16 *timerPointer;
    u8 *activePointer;
    s32 i;
    s32 partIndex;
    register s32 sequenceOffset asm("r10");
    u32 clear;
    s32 xOffset, zOffset;
    savedZ = z;
    frame = (u8)incomingFrame;
    existing = gActorAttachmentManager;
    if (!existing) {
        manager = ActorAttachment_CreateManager();
        if (manager)
            goto gotManager;
    failed:
        return 0;
    } else
        manager = existing;
gotManager:
    if (!follow)
        goto failed;
    step = Div(total, 3);
    slot = ActorAttachment_AllocateSlotHeader(manager);
    {
        struct ActorAttachmentPartSprite *low = slot->parts;
        asm("" : "+r"(low));
        parts = low;
    }
    if (!slot)
        goto failed;
    /* Reserve the later CpuSet fill source before the geometry output locals. */
    asm("" : "=m"(clear));
    {
        register u8 *frameLow asm("r4") = &slot->frame;
        u8 *actionLow;
        register u8 *kindLow asm("r1");
        u32 zero = 0;
        *frameLow = frame;
        actionLow = &slot->action;
        *actionLow = action;
        slot->value = value;
        slot->flags31 = 1;
        kindLow = &slot->kind;
        *kindLow = 3;
        slot->unknown34 = zero;
        slot->update = savedUpdate;
        slot->position.y = zero;
        slot->position.z = savedZ;
        i = 0;
        framePointer = frameLow;
        actionPointer = actionLow;
        kindPointer = kindLow;
        {
            register u8 *low asm("r3") = actionLow + 6;
            asm("" : "+r"(low));
            phasePointer = low;
        }
        {
            register s32 *low asm("r0") = slot->counters;
            asm("" : "+r"(low));
            counterPointer = low;
        }
        {
            s16 *low = (s16 *)(kindLow + 4);
            thresholdPointer = low;
        }
        {
            u32 offset = 0x40;
            register s16 *low asm("r2");
            asm("" : "+r"(offset));
            low = (s16 *)(offset + (u32)slot);
            asm("" : "+r"(low));
            timerPointer = low;
        }
        {
            register u8 *low asm("r3") = phasePointer + 6;
            asm("" : "+r"(low));
            resourcePointer = low;
        }
        {
            register u8 *low asm("r0") = (u8 *)counterPointer - 0x14;
            asm("" : "+r"(low));
            maskPointer = low;
        }
        {
            u32 offset = 0x48;
            register u8 *low asm("r1");
            asm("" : "+r"(offset));
            low = (u8 *)(offset + (u32)slot);
            asm("" : "+r"(low));
            activePointer = low;
        }
    }
    {
        u8 *status = slot->status;
        u8 *phase;
        s16 *threshold;
        s32 *counter;
        register u32 zero asm("r2") = 0;
        phase = phasePointer;
        threshold = thresholdPointer;
        counter = counterPointer;
        do {
            status[i] = zero;
            phase[i] = zero;
            *counter++ = zero;
            *threshold++ = zero;
            i++;
        } while (i <= 2);
    }
    {
        u32 zero;
        u32 zero1;
        register s16 *timer asm("r2");
        zero1 = 0;
        asm("" : "+r"(zero1));
        zero = 0;
        timer = timerPointer;
        *timer = zero;
        {
            s16 half = zero;
            slot->jitter[0].z = half;
            slot->jitter[0].y = half;
            slot->jitter[0].x = half;
        }
        {
            u32 resource = 3;
            register u8 *low asm("r3");
            low = resourcePointer;
            asm("" : "+r"(low));
            *low = resource;
        }
        {
            register struct ContactPosition *low asm("r6") = anchor;
            asm("" : "+r"(low));
            slot->anchor = low;
        }
        {
            u8 *low = maskPointer;
            asm("" : "+r"(low));
            *low = zero1;
        }
        {
            register u8 *low asm("r2") = activePointer;
            asm("" : "+r"(low));
            *low = zero1;
        }
        slot->source = (void *)sourceWord;
        {
            register u32 offset asm("r3") = 0x134;
            register struct ActorAttachmentFollowView **dst asm("r0");
            asm("" : "+r"(offset));
            dst = (void *)((u32)slot + offset);
            {
                register struct ActorAttachmentFollowView *low asm("r6") = follow;
                asm("" : "+r"(low));
                *dst = low;
            }
        }
    }
    partIndex = 0;
    if (partIndex < (s32)partCount)
        goto partLoop;
    goto done;
partLoop: {
    register struct ActorAttachmentPartSprite *part asm("r6");
    struct ActorAttachmentFollowView **followPointer;
    const void *resource;
    void *sequence;
    s32 index = partIndex;
    s32 scale = index * 4;
    s32 shift;
    u32 kind;
    {
        register s32 *base asm("r6") = counterPointer;
        register s32 *dst asm("r1");
        asm("" : "+r"(scale), "+r"(base));
        dst = (void *)((u32)base + scale);
        shift = index * 16;
        {
            s32 result = shift + index;
            *dst = result;
        }
    }
    {
        u8 *low = phasePointer;
        u32 zero;
        low += partIndex;
        zero = 0;
        *low = zero;
        {
            register s32 index asm("r1") = partIndex;
            u32 offset;
            register s16 *base asm("r6");
            register s16 *dst asm("r0");
            asm("" : "+r"(index));
            offset = index * 2;
            base = thresholdPointer;
            asm("" : "+r"(base));
            dst = (void *)((u32)base + offset);
            {
                register s32 interval asm("r6") = step;
                s32 product;
                asm("" : "+r"(interval));
                product = partIndex;
                product *= interval;
                *dst = product;
            }
        }
        zOffset = zero;
        xOffset = zero;
    }
    {
        s32 index = partIndex;
        register u32 offset asm("r0");
        offset = (scale + index) * 8 + 0x8C;
        part = (void *)((u32)slot + offset);
    }
    {
        u8 *low = kindPointer;
        kind = *low;
    }
    scaledIndex = scale;
    sequenceOffset = shift;
    switch (kind) {
        case 2: {
            register struct ActorAttachmentManagerView *low asm("r3") = manager;
            asm("" : "+r"(low));
            resource = low->partResource;
            break;
        }
        case 3: {
            struct ActorAttachmentManagerView *low = manager;
            resource = low->partResource;
            break;
        }
        default:
            resource = 0;
            break;
    }
    ParticleSprite_Initialize(part, resource, 0);
    {
        u8 *lowResource = resourcePointer;
        u32 resource;
        u8 *lowAction;
        u32 action;
        register u32 offset asm("r3");
        resource = *lowResource;
        lowAction = actionPointer;
        action = *lowAction;
        offset = 0x134;
        asm("" : "+r"(offset));
        followPointer = (void *)((u32)slot + offset);
        asm("" : "+r"(followPointer));
        ActorAttachment_GetActionOffset(resource, action, (*followPointer)->direction, &xOffset,
                                        &zOffset, slot->source);
    }
    Effect_SetOrigin(part, xOffset, zOffset);
    {
        u8 *low = actionPointer;
        if (*low == 255)
            slot->offset = (*followPointer)->offset;
        else {
            struct ContactPosition *offset = &slot->offset;
            clear = 0;
            {
                u32 *source = &clear;
                asm("" : "+r"(source));
                CpuSet(source, offset, 0x05000002);
            }
        }
    }
    part->flags |= 1;
    part->unknownC[4] = 127;
    part->unknownC[3] = 1;
    {
        s32 indexOffset = sequenceOffset;
        register u8 *base asm("r0");
        register u32 offset asm("r3");
        register u16 *dst asm("r1");
        u32 invalid;
        base = (u8 *)slot + indexOffset;
        offset = 0x10E;
        asm("" : "+r"(offset));
        dst = (void *)((u32)base + offset);
        invalid = 255;
        *dst = invalid;
    }
    {
        register u8 *low asm("r6") = kindPointer;
        u32 kind;
        asm("" : "+r"(low));
        kind = *low;
        switch (kind) {
            case 0: {
                struct ActorAttachmentManagerResources *low = (void *)manager;
                asm("" : "+r"(low));
                sequence = low->sequenceA;
                break;
            }
            case 2:
            case 3: {
                register struct ActorAttachmentManagerResources *low asm("r1") = (void *)manager;
                asm("" : "+r"(low));
                sequence = low->sequenceB;
                break;
            }
            case 1:
            case 4:
            case 5:
            default:
                sequence = 0;
                break;
        }
    }
    {
        u32 offset = 0x104;
        struct DisplaySequenceState *state;
        register u8 *lowFrame asm("r6");
        offset += sequenceOffset;
        state = (void *)((u32)slot + offset);
        lowFrame = framePointer;
        asm("" : "+r"(lowFrame));
        Anim_SetSequence(state, sequence, *lowFrame, 0, 0);
    }
    {
        struct ActorAttachmentPartSprite *dst =
            (void *)((scaledIndex + partIndex) * 8 + (u32)parts);
        register struct ContactPosition *low asm("r3") = anchor;
        struct ContactPosition p;
        asm("" : "+r"(low));
        p = *low;
        dst->position = p;
    }
    {
        u32 one = 1;
        register s32 index asm("r6") = partIndex;
        asm("" : "+r"(one), "+r"(index));
        *maskPointer |= one << index;
    }
    {
        register u32 one asm("r3") = 1;
        asm("" : "+r"(one));
        partIndex += one;
    }
    {
        register s32 count asm("r6") = partCount;
        asm("" : "+r"(count));
        if (partIndex >= count)
            goto done;
    }
    goto partLoop;
}
done:
    return slot;
}
