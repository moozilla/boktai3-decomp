#include "global.h"
#include "scene/actor/actor_attachment_manager.h"

struct ActorAttachmentSlotView *
ActorAttachment_AllocateSlotHeader(struct ActorAttachmentManagerView *manager)
{
    struct ActorAttachmentSlotView *slot = ActorAttachment_AllocateSlot(manager);

    if (!slot)
        return NULL;
    return slot;
}
