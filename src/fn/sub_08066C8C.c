#include "actor_attachment_manager.h"
s32 ActorAttachment_IsUnavailable(struct ActorAttachmentSlotView *slot)
{
    if (gActorAttachmentManager && slot && slot->flags31)
        return 0;
    return 1;
}
