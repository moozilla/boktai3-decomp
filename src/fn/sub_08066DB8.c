#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_manager.h"
/* Unsupported kinds preserve the original slot on release frames and return
 * the final parts-release result on other frames. */
struct ActorAttachmentSlotView *ActorAttachment_UpdateFromDescriptor(
    struct ActorAttachmentSlotView *slot, struct ActorAttachmentFollowView *descriptor,
    u8 inputKind, u32 inputFrame, s16 *volatile value, u32 inputCount, s32 z, s32 y)
{
    u32 kind;
    u32 frame;
    u32 count;
    u32 action;
    register struct ActorAttachmentSlotView *result asm("r0") = slot;
    {
        {
            u32 rawCount = inputCount;
            kind = inputKind;
            frame = (u8)inputFrame;
            count = (u16)rawCount;
        }
        if (!descriptor)
            return 0;
        if (frame == 127) {
            if (!slot)
                return 0;
            switch (inputKind) {
                case 0:
                    return ActorAttachment_ReleaseResource(slot, 0, descriptor->identity);
                case 1:
                    return ActorAttachment_ReleaseResource(slot, 1, descriptor->identity);
                case 3:
                    return ActorAttachment_ReleasePartsResource(slot, 3, descriptor->identity);
                case 2:
                    return ActorAttachment_ReleaseResource(slot, 2, descriptor->identity);
                case 4:
                    return ActorAttachment_ReleaseResource(slot, 4, descriptor->identity);
                case 5:
                    return ActorAttachment_ReleaseResource(slot, 5, descriptor->identity);
                default:
                    return result;
            }
        }
    }
    slot = ActorAttachment_ReleaseResource(slot, 0, descriptor->identity);
    slot = ActorAttachment_ReleaseResource(slot, 1, descriptor->identity);
    slot = ActorAttachment_ReleaseResource(slot, 2, descriptor->identity);
    slot = ActorAttachment_ReleaseResource(slot, 4, descriptor->identity);
    slot = ActorAttachment_ReleaseResource(slot, 5, descriptor->identity);
    result = ActorAttachment_ReleasePartsResource(slot, 3, descriptor->identity);
    {
        u32 mask = 4;
        u32 bits;
        bits = descriptor->flags & mask;
        action = 0;
        if (bits)
            action = 255;
    }
    {
        u32 selectedKind = kind;
        switch (selectedKind) {
            case 0:
                return ActorAttachment_CreateSpriteSlot(
                    frame, &descriptor->anchor, descriptor->identity, y, z, action, 0, descriptor);
            case 1:
                return ActorAttachment_CreateValueSpriteSlot(
                    frame, &descriptor->anchor, descriptor->identity, y, z, action, ({
                        register s16 *spriteValue asm("r3") = value;
                        spriteValue;
                    }),
                    (s16)count, 0, 0, descriptor);
            case 2:
                return ActorAttachment_CreateSinglePartSlot(
                    1, &descriptor->anchor, descriptor->identity, z, action, descriptor);
            case 3: {
                /* Part attachments reuse the frame temporary for the value pointer. */
                return ActorAttachment_CreateThreePartSlot(
                    7, &descriptor->anchor, descriptor->identity, z, action,
                    (s16 *)(frame = (u32)value), (s16)count, descriptor, 0);
            }
            case 4:
                return ActorAttachment_CreateFixedSpriteSlot(
                    &descriptor->anchor, descriptor->identity, y, z, action, 0, descriptor);
            case 5:
                return ActorAttachment_CreateAlternateSpriteSlot(
                    &descriptor->anchor, descriptor->identity, y, z, action, 0, descriptor);
            default:
                return result;
        }
    }
}
