// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_manager.h"
void ParticleSprite_Initialize(void *, const void *, s32);
void Effect_SetOrigin(void *, s32, s32);
struct ActorAttachmentSlotView *
ActorAttachment_CreateSinglePartSlot(u32 incomingFrame, struct ContactPosition *anchor,
                                     void *update, s32 z, u32 action,
                                     struct ActorAttachmentFollowView *follow)
{
    struct ActorAttachmentManagerView *manager;
    u32 frame;
    register s32 savedZ asm("r9");
    struct ActorAttachmentManagerView *existing;
    struct ActorAttachmentSlotView *slot;
    struct ActorAttachmentPartSprite *part;
    register struct ActorAttachmentPartSprite *workingPart asm("r10");
    struct ActorAttachmentPartSprite *renderPart;
    u8 *frameLow;
    u8 *actionLow;
    register u8 *kindLow asm("r1");
    u8 *actionPointer;
    u8 *partMaskPointer;
    u8 *kindPointer;
    s32 *xPointer;
    u8 *activePointer;
    s32 *zPointer;
    struct DisplaySequenceState *sequencePointer;
    u16 *sequenceFlagPointer;
    struct ActorAttachmentFollowView **followPointer;
    u8 *framePointer;
    u8 *resourcePointer;
    register s16 *timerPointer asm("r12");
    const void *resource;
    void *sequence;
    u32 clear;
    s32 xOffset, zOffset;
    s32 i;
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
    slot = ActorAttachment_AllocateSlotHeader(manager);
    {
        struct ActorAttachmentPartSprite *low = slot->parts;
        asm("" : "+r"(low));
        part = low;
    }
    if (!slot)
        goto failed;
    frameLow = &slot->frame;
    {
        u32 zero = 0;
        *frameLow = frame;
        actionLow = &slot->action;
        *actionLow = action;
        slot->flags31 = 1;
        kindLow = &slot->kind;
        *kindLow = 2;
        slot->unknown34 = zero;
        slot->update = update;
        slot->position.y = zero;
    }
    {
        register s32 low asm("r2") = savedZ;
        slot->position.z = low;
    }
    i = 0;
    {
        register struct ActorAttachmentPartSprite *low asm("r0") = part;
        asm("" : "+r"(low));
        workingPart = low;
    }
    framePointer = frameLow;
    actionPointer = actionLow;
    kindPointer = kindLow;
    {
        u32 offset = 0x40;
        register s16 *low asm("r1");
        asm("" : "+r"(offset));
        low = (s16 *)(offset + (u32)slot);
        asm("" : "+r"(low));
        timerPointer = low;
    }
    {
        u32 offset = 0x3F;
        register u8 *low asm("r2");
        asm("" : "+r"(offset));
        low = (u8 *)(offset + (u32)slot);
        asm("" : "+r"(low));
        resourcePointer = low;
    }
    {
        u32 offset = 308;
        register struct ActorAttachmentFollowView **low asm("r3");
        asm("" : "+r"(offset));
        low = (struct ActorAttachmentFollowView **)((u8 *)slot + offset);
        asm("" : "+r"(low));
        followPointer = low;
    }
    /* Reserve the later fill source before the geometry output locals. */
    asm("" : "=m"(clear));
    /* Integer address temporaries preserve the original two-step stack addresses;
     * each reconstructed pointer still points to its output local. */
    {
        u32 base = (u32)&xOffset - 0xC;
        s32 *low;
        asm("" : "+&r"(base));
        low = (s32 *)(base + 0xC);
        xPointer = low;
    }
    {
        register u32 base asm("r1") = (u32)&zOffset - 0x10;
        s32 *low;
        asm("" : "+&r"(base));
        low = (s32 *)(base + 0x10);
        zPointer = low;
    }
    {
        u32 offset = 270;
        register u16 *low asm("r2");
        asm("" : "+r"(offset));
        low = (u16 *)((u8 *)slot + offset);
        asm("" : "+r"(low));
        sequenceFlagPointer = low;
    }
    {
        u32 offset = 260;
        register struct DisplaySequenceState *low asm("r3");
        asm("" : "+r"(offset));
        low = (struct DisplaySequenceState *)((u8 *)slot + offset);
        asm("" : "+r"(low));
        sequencePointer = low;
    }
    {
        register u8 *low asm("r0") = &slot->partMask;
        asm("" : "+r"(low));
        partMaskPointer = low;
    }
    {
        register u8 *low asm("r1") = &slot->active48;
        asm("" : "+r"(low));
        activePointer = low;
    }
    {
        u8 *status = slot->status;
        s32 zero = 0;
        u8 *phase;
        s16 *threshold;
        s32 *counter;
        phase = slot->phase;
        threshold = (s16 *)(activePointer - 6);
        counter = slot->counters;
        do {
            status[i] = zero;
            phase[i] = zero;
            *counter++ = zero;
            *threshold++ = zero;
            i++;
        } while (i <= 2);
    }
    {
        register u32 zero asm("r0") = 0;
        register s16 *timerLow asm("r2");
        timerLow = timerPointer;
        *timerLow = zero;
        {
            register s16 half asm("r0") = zero;
            asm("" : "+r"(half));
            slot->jitter[0].z = half;
            slot->jitter[0].y = half;
            slot->jitter[0].x = half;
        }
        {
            u32 resource = 2;
            register u8 *low asm("r3");
            low = resourcePointer;
            asm("" : "+r"(low));
            *low = resource;
        }
        slot->source = (void *)zero;
        {
            register u32 offset asm("r2") = 0x134;
            register struct ActorAttachmentFollowView **low asm("r1");
            asm("" : "+r"(offset));
            low = (struct ActorAttachmentFollowView **)((u8 *)slot + offset);
            {
                register struct ActorAttachmentFollowView *target asm("r3") = follow;
                asm("" : : "r"(target));
                *low = target;
            }
        }
        zOffset = zero;
        xOffset = zero;
    }
    renderPart = workingPart;
    {
        register u8 *lowKind asm("r1") = kindPointer;
        u32 kind;
        asm("" : "+r"(lowKind));
        kind = *lowKind;
        switch (kind) {
            case 2: {
                register struct ActorAttachmentManagerView *low asm("r2") = manager;
                asm("" : "+r"(low));
                resource = low->partResource;
                break;
            }
            case 3: {
                register struct ActorAttachmentManagerView *low asm("r3") = manager;
                asm("" : "+r"(low));
                resource = low->partResource;
                break;
            }
            default:
                resource = 0;
                break;
        }
    }
    ParticleSprite_Initialize(renderPart, resource, 0);
    {
        register u8 *lowResource asm("r1") = resourcePointer;
        asm("" : "+r"(lowResource));
        ActorAttachment_GetActionOffset(*lowResource, *actionPointer, (*followPointer)->direction,
                                        xPointer, zPointer, slot->source);
    }
    Effect_SetOrigin(renderPart, xOffset, zOffset);
    if (*actionPointer == 255) {
        slot->offset = (*followPointer)->offset;
    } else {
        struct ContactPosition *offset = &slot->offset;
        clear = 0;
        {
            u32 *source = &clear;
            asm("" : "+r"(source));
            CpuSet(source, offset, 0x05000002);
        }
    }
    renderPart->flags |= 1;
    renderPart->unknownC[4] = 127;
    renderPart->unknownC[3] = 1;
    *sequenceFlagPointer = 255;
    switch (*kindPointer) {
        case 0: {
            struct ActorAttachmentManagerResources *low =
                (struct ActorAttachmentManagerResources *)manager;
            sequence = low->sequenceA;
            break;
        }
        case 2:
        case 3: {
            struct ActorAttachmentManagerResources *low =
                (struct ActorAttachmentManagerResources *)manager;
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
    {
        u8 *lowFrame = framePointer;
        Anim_SetSequence(sequencePointer, sequence, *lowFrame, 0, 0);
    }
    {
        struct ContactPosition *lowAnchor = anchor;
        struct ContactPosition position;
        struct ActorAttachmentPartSprite *lowPart;
        slot->anchor = lowAnchor;
        position = *lowAnchor;
        lowPart = part;
        lowPart->position = position;
    }
    {
        u32 one = 1;
        u8 *maskLow;
        maskLow = partMaskPointer;
        *maskLow = one;
    }
    {
        u8 *activeLow = activePointer;
        *activeLow = 0;
    }

    return slot;
}
