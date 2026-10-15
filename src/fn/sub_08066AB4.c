#include "actor_attachment_manager.h"
#define ActorAttachment_CreatePartsSlotWithCount sub_08066AB4
struct ActorAttachmentSlotView *
ActorAttachment_CreatePartsSlotWithCount(u8 frame, struct ContactPosition *anchor, void *update,
                                         s32 z, u32 action, s16 *value, s32 count, u32 partCount,
                                         struct ActorAttachmentFollowView *follow, u32 unused)
{
    return sub_08066474(frame, anchor, update, z, action, value, count, partCount, follow, unused);
}
