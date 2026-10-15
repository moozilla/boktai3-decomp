#include "actor_attachment_manager.h"
#define ActorAttachment_CreatePartsSlotWithKind sub_08066AB4
struct ActorAttachmentSlotView *
ActorAttachment_CreatePartsSlotWithKind(u8 frame, struct ContactPosition *anchor, void *update,
                                        s32 z, u32 action, s16 *value, s32 count, u32 kind,
                                        struct ActorAttachmentFollowView *follow, u32 unused)
{
    return sub_08066474(frame, anchor, update, z, action, value, count, kind, follow, unused);
}
