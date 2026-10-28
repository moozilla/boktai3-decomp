#include "actor_attachment_manager.h"
void ActorAttachment_LoadManagerResources(struct ActorAttachmentManagerResources *);
s32 ActorAttachment_InitManager(struct ActorAttachmentManagerView *manager)
{
    manager->occupiedMask = 0;
    ActorAttachment_LoadManagerResources((struct ActorAttachmentManagerResources *)manager);
    gActorAttachmentManager = manager;
    return 0;
}
