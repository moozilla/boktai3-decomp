#include "actor_attachment_manager.h"
#define ActorAttachment_IsUnavailable sub_08066C8C
s32 ActorAttachment_IsUnavailable(struct ActorAttachmentSlotView *slot)
{
    if (gUnk_02000124 && slot && slot->active)
        return 0;
    return 1;
}
