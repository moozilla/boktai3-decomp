// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_manager.h"
static inline u8 AlternateMode(void)
{
    u32 bit = 0x80;
    if (gGameState->flags868 & bit)
        return TRUE;
    return FALSE;
}
struct ActorAttachmentSlotView *
ActorAttachment_CreateValueSpriteSlot(u32 incomingFrame, struct ContactPosition *anchor,
                                      void *update, s32 y, s32 z, u32 action, s16 *value,
                                      s32 incomingCount, u32 emitBurst, void *source,
                                      struct ActorAttachmentFollowView *follow)
{
    register struct ActorAttachmentManagerView *manager asm("r8");
    struct ActorAttachmentManagerView *existing;
    u32 frame;
    s32 savedY = y;
    u32 countLow = incomingCount;
    u32 thresholdCount;
    register u8 *framePointer asm("r6");
    u8 *actionLow;
    u8 *actionPointer;
    u8 *resourcePointer;
    u8 *activePointer;
    struct ContactSwarmSprite *spritePointer;
    struct DisplaySequenceState *sequencePointer;
    u16 *sequenceFlagPointer;
    s32 currentValue;
    s16 *thresholdLow;
    register s16 *thresholdPointer asm("r5");
    register u8 *kindPointer asm("r9");
    register struct ActorAttachmentFollowView **followPointer asm("r10");
    u8 *activeLow;
    struct ActorAttachmentSlotView *slot;
    struct ContactSwarmSprite *sprite;
    void *graphics, *sequence;
    s32 xOffset, zOffset;
    struct ContactPosition burstPositions[3];
    register s32 i asm("r4");
    u32 found;
    asm("" : "+r"(savedY), "+r"(countLow));
    frame = (u8)incomingFrame;
    countLow = (u16)countLow;
    thresholdCount = countLow;
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
    if (!slot)
        goto failed;
    asm("" : "+r"(slot));
    framePointer = &slot->frame;
    *framePointer = frame;
    slot->flags31 = 1;
    slot->position.y = savedY;
    actionLow = &slot->action;
    *actionLow = action;
    slot->position.z = z;
    slot->position.pad = z;
    {
        s32 threshold = Div((s16)thresholdCount, 5);
        thresholdLow = &slot->thresholds[0];
        asm("" : "+r"(thresholdLow));
        *thresholdLow = threshold;
    }
    activeLow = &slot->active48;
    *activeLow = 1;
    {
        register u32 offset asm("r3") = 0x134;
        struct ActorAttachmentFollowView **destination;
        destination = (struct ActorAttachmentFollowView **)((u32)slot + offset);
        {
            struct ActorAttachmentFollowView *target = follow;
            *destination = target;
        }
    }
    slot->value = value;
    found = 0;
    asm("" : "+r"(found));
    i = 4;
    currentValue = *slot->value;
    {
        s32 initialLimit = *thresholdLow * 4;
        actionPointer = actionLow;
        thresholdPointer = thresholdLow;
        activePointer = activeLow;
        {
            u8 *low = (u8 *)thresholdLow - 3;
            resourcePointer = low;
        }
        {
            u32 offset = 0x3E;
            register u8 *low asm("r2");
            asm("" : "+r"(offset));
            low = (u8 *)(offset + (u32)slot);
            asm("" : "+r"(low));
            kindPointer = low;
        }
        {
            register struct ContactSwarmSprite *low asm("r3") = &slot->sprite;
            asm("" : "+r"(low));
            spritePointer = low;
        }
        {
            u32 offset = 0x134;
            register struct ActorAttachmentFollowView **low asm("r1");
            asm("" : "+r"(offset));
            low = (struct ActorAttachmentFollowView **)(offset + (u32)slot);
            asm("" : "+r"(low));
            followPointer = low;
        }
        {
            u32 offset = 0x10E;
            register u16 *low asm("r2");
            asm("" : "+r"(offset));
            low = (u16 *)((u32)slot + offset);
            asm("" : "+r"(low));
            sequenceFlagPointer = low;
        }
        {
            u32 offset = 0x104;
            register struct DisplaySequenceState *low asm("r3");
            asm("" : "+r"(offset));
            low = (struct DisplaySequenceState *)((u32)slot + offset);
            asm("" : "+r"(low));
            sequencePointer = low;
        }
        if (currentValue >= initialLimit) {
            *framePointer = 0;
        } else {
            while (--i > 0) {
                if (*slot->value >= *thresholdPointer * i) {
                    {
                        register u32 four asm("r0") = 4;
                        *framePointer = four - i;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found)
                *framePointer = 3;
        }
    }
    {
        void *lowUpdate = update;
        slot->update = lowUpdate;
    }
    {
        u8 *kindLow;
        {
            register u32 zero asm("r1") = 0;
            u32 one;
            register u8 *lowResource asm("r3");
            one = 1;
            lowResource = resourcePointer;
            asm("" : "+r"(lowResource));
            *lowResource = one;
            kindLow = kindPointer;
            *kindLow = one;
            slot->anchor = anchor;
            slot->source = source;
            zOffset = zero;
            xOffset = zero;
        }
        sprite = spritePointer;
        {
            s32 kind = *kindLow;
            s32 copy;
            copy = kind;
            if (kind == 1)
                goto graphicsB;
            if (kind > 1)
                goto graphicsRange;
            if (kind == 0)
                goto graphicsA;
            goto graphicsNone;
        graphicsRange:
            switch (copy) {
                case 4:
                case 5:
                    goto graphicsB;
                default:
                    goto graphicsNone;
            }
        graphicsA:
            graphics = ((struct ActorAttachmentManagerResources *)manager)->graphicsA;
            goto graphicsReady;
        graphicsB:
            graphics = ((struct ActorAttachmentManagerResources *)manager)->graphicsB;
            goto graphicsReady;
        graphicsNone:
            graphics = 0;
        graphicsReady:;
        }
    }
    Sprite_Init(sprite, graphics, 0);
    {
        u8 *lowAction = actionPointer;
        if (*lowAction == 255) {
            register struct ActorAttachmentFollowView **low asm("r2") = followPointer;
            slot->offset = (*low)->offset;
        } else {
            register u8 *lowResource asm("r3") = resourcePointer;
            register u8 *lowAction asm("r2");
            register struct ActorAttachmentFollowView **lowFollow asm("r3");
            u32 resource;
            asm("" : "+r"(lowResource));
            resource = *lowResource;
            lowAction = actionPointer;
            asm("" : "+r"(lowAction));
            {
                u32 action = *lowAction;
                lowFollow = followPointer;
                ActorAttachment_GetActionOffset(resource, action, (*lowFollow)->direction, &xOffset,
                                                &zOffset, slot->source);
            }
            slot->offset.x = xOffset;
            slot->offset.y = 0;
            slot->offset.z = zOffset;
        }
    }
    sprite->flags |= 1;
    if (AlternateMode())
        sprite->flags |= 0x10;
    sprite->priority = 1;
    {
        u32 invalid = 255;
        register u16 *low asm("r3");
        low = sequenceFlagPointer;
        asm("" : "+r"(low));
        *low = invalid;
    }
    {
        register u8 *lowKind asm("r1") = kindPointer;
        switch (*lowKind) {
            case 0: {
                register struct ActorAttachmentManagerResources *low asm("r2") =
                    (struct ActorAttachmentManagerResources *)manager;
                sequence = low->sequenceA;
                break;
            }
            case 2:
            case 3: {
                register struct ActorAttachmentManagerResources *low asm("r3") =
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
    }
    Anim_SetSequence(sequencePointer, sequence, *framePointer, 0, 0);
    {
        register struct ContactPosition *lowAnchor asm("r2") = anchor;
        struct ContactPosition position;
        register struct ContactSwarmSprite *destination asm("r3");
        asm("" : "+r"(lowAnchor));
        position = *lowAnchor;
        destination = spritePointer;
        asm("" : "+r"(destination));
        destination->position = position;
    }
    {
        u32 one = 1;
        register u8 *low asm("r1");
        low = activePointer;
        asm("" : "+r"(low));
        *low = one;
    }
    if (!(gEngineFlags & 0x100) && emitBurst == 1) {
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 offset = gRandomIndex * 2;
            register const u16 *table asm("r2");
            table = gUnk_0203B400;
            Mod(*(u16 *)(offset + (u32)table), 3);
        }
        {
            struct ContactPosition *position = burstPositions;
            i = 2;
            do {
                position->x = slot->anchor->x;
                position->y = slot->anchor->y + z;
                position->z = slot->anchor->z;
                position++;
                i--;
            } while (i >= 0);
        }
    }
    return slot;
}
