#include "global.h"
#ifdef NONMATCHING
// fuzzy: 95.9%
// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_manager.h"
/* Unsupported kind127/frame0 is reached by49258 and4AB40. It forwards the
 * final parts-release result. On frame127, unsupported kinds preserve the
 * original slot. These defined defaults differ in register allocation only. */
struct ActorAttachmentSlotView *
ActorAttachment_UpdateFromDescriptor(struct ActorAttachmentSlotView *slot,
                                     struct ActorAttachmentFollowView *descriptor, u32 inputKind,
                                     u32 inputFrame, s16 *value, u32 inputCount, s32 z, s32 y)
{
    register u32 kind asm("r8");
    register u32 frame asm("r6");
    register u32 count asm("r9");
    u32 action;
    struct ActorAttachmentSlotView *fallback;
    {
        register struct ActorAttachmentSlotView *original asm("r0") = slot;
        asm("" : "+r"(original));
        {
            register u32 rawCount asm("r1") = inputCount;
            inputKind = (u8)inputKind;
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
                    return original;
            }
        }
    }
    slot = ActorAttachment_ReleaseResource(slot, 0, descriptor->identity);
    slot = ActorAttachment_ReleaseResource(slot, 1, descriptor->identity);
    slot = ActorAttachment_ReleaseResource(slot, 2, descriptor->identity);
    slot = ActorAttachment_ReleaseResource(slot, 4, descriptor->identity);
    slot = ActorAttachment_ReleaseResource(slot, 5, descriptor->identity);
    fallback = ActorAttachment_ReleasePartsResource(slot, 3, descriptor->identity);
    {
        register u32 mask asm("r2") = 4;
        register u32 bits asm("r1");
        asm("" : "+r"(mask));
        bits = descriptor->flags & mask;
        action = 0;
        if (bits)
            action = 255;
    }
    {
        register u32 selectedKind asm("r1") = kind;
        asm("" : "+r"(selectedKind));
        switch (selectedKind) {
            case 0:
                return ActorAttachment_CreateSpriteSlot(
                    frame, &descriptor->anchor, descriptor->identity, y, z, action, 0, descriptor);
            case 1:
                return ActorAttachment_CreateValueSpriteSlot(frame, &descriptor->anchor,
                                                             descriptor->identity, y, z, action,
                                                             value, (s16)count, 0, 0, descriptor);
            case 2:
                return ActorAttachment_CreateSinglePartSlot(
                    1, &descriptor->anchor, descriptor->identity, z, action, descriptor);
            case 3: {
                s16 *parameter = value;
                return ActorAttachment_CreateThreePartSlot(7, &descriptor->anchor,
                                                           descriptor->identity, z, action,
                                                           parameter, (s16)count, descriptor, 0);
            }
            case 4:
                return ActorAttachment_CreateFixedSpriteSlot(
                    &descriptor->anchor, descriptor->identity, y, z, action, 0, descriptor);
            case 5:
                return ActorAttachment_CreateAlternateSpriteSlot(
                    &descriptor->anchor, descriptor->identity, y, z, action, 0, descriptor);
            default:
                return fallback;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", ActorAttachment_UpdateFromDescriptor);
#endif
