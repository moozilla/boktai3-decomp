#include "actor_attachment_manager.h"
struct ActorAttachmentSlotView *
ActorAttachment_CreateThreePartSlot(u8 frame, struct ContactPosition *anchor, void *update, s32 z,
                                    u32 action, s16 *value, s32 count,
                                    struct ActorAttachmentFollowView *follow, u32 unused)
{
    return sub_08066474(frame, anchor, update, z, action, value, count, 3, follow, unused);
}
