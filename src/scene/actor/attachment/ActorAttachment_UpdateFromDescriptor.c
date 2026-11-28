#include "global.h"
#include "scene/actor/actor_attachment_manager.h"
#include "engine/word_flags.h"
/* Unsupported kinds fall through without an explicit return, preserving the
 * value left in r0 by the original routine. */
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
        }
    } else {
        slot = ActorAttachment_ReleaseResource(slot, 0, descriptor->identity);
        slot = ActorAttachment_ReleaseResource(slot, 1, descriptor->identity);
        slot = ActorAttachment_ReleaseResource(slot, 2, descriptor->identity);
        slot = ActorAttachment_ReleaseResource(slot, 4, descriptor->identity);
        slot = ActorAttachment_ReleaseResource(slot, 5, descriptor->identity);
        ActorAttachment_ReleasePartsResource(slot, 3, descriptor->identity);
        action = Flags_MaskedWord(4, &descriptor->flags) ? 255 : 0;
        switch (kind) {
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
            case 3:
                return ActorAttachment_CreateThreePartSlot(7, &descriptor->anchor,
                                                           descriptor->identity, z, action, value,
                                                           (s16)count, descriptor, 0);
            case 4:
                return ActorAttachment_CreateFixedSpriteSlot(
                    &descriptor->anchor, descriptor->identity, y, z, action, 0, descriptor);
            case 5:
                return ActorAttachment_CreateAlternateSpriteSlot(
                    &descriptor->anchor, descriptor->identity, y, z, action, 0, descriptor);
        }
    }
}
