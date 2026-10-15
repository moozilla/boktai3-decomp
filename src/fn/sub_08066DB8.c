#include "actor_attachment_manager.h"
struct ActorAttachmentSlotView *
ActorAttachment_UpdateFromDescriptor(struct ActorAttachmentSlotView *slot,
                                     struct ActorAttachmentFollowView *descriptor, u8 kind,
                                     u8 frame, s16 *value, u16 count, s32 z, s32 y)
{
    u32 action;
    if (!descriptor)
        return 0;
    if (frame == 127) {
        if (!slot)
            return 0;
        switch (kind) {
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
                goto done;
        }
    }
    slot = ActorAttachment_ReleaseResource(slot, 0, descriptor->identity);
    slot = ActorAttachment_ReleaseResource(slot, 1, descriptor->identity);
    slot = ActorAttachment_ReleaseResource(slot, 2, descriptor->identity);
    slot = ActorAttachment_ReleaseResource(slot, 4, descriptor->identity);
    slot = ActorAttachment_ReleaseResource(slot, 5, descriptor->identity);
    ActorAttachment_ReleasePartsResource(slot, 3, descriptor->identity);
    {
        u32 mask = 4;
        u32 bits;
        bits = descriptor->flags & mask;
        action = 0;
        if (bits)
            action = 255;
    }
    {
        switch (kind) {
            case 0:
                return ActorAttachment_CreateSpriteSlot(
                    frame, &descriptor->anchor, descriptor->identity, y, z, action, 0, descriptor);
            case 1:
                return sub_08065FD0(frame, &descriptor->anchor, descriptor->identity, y, z, action,
                                    value, (s16)count, 0, 0, descriptor);
            case 2:
                return sub_08066278(1, &descriptor->anchor, descriptor->identity, z, action,
                                    descriptor);
            case 3: {
                s16 *parameter = value;
                return ActorAttachment_CreateThreePartSlot(7, &descriptor->anchor,
                                                           descriptor->identity, z, action,
                                                           parameter, (s16)count, descriptor, 0);
            }
            case 4:
                return sub_080666D8(&descriptor->anchor, descriptor->identity, y, z, action, 0,
                                    descriptor);
            case 5:
                return sub_080668B0(&descriptor->anchor, descriptor->identity, y, z, action, 0,
                                    descriptor);
            default:
                break;
        }
    }
done:;
}
