#include "actor_attachment_manager.h"
#define ActorAttachment_InitManager sub_08066C0C
void sub_08066BB8(struct ActorAttachmentManagerResources *);
s32 ActorAttachment_InitManager(struct ActorAttachmentManagerView *manager)
{
    manager->occupiedMask = 0;
    sub_08066BB8((struct ActorAttachmentManagerResources *)manager);
    gUnk_02000124 = manager;
    return 0;
}
