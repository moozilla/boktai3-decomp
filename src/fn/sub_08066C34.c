#include "actor_attachment_manager.h"
s32 ActorAttachment_InitManager(struct ActorAttachmentManagerView *);
s32 ActorAttachment_TickManager(struct ActorAttachmentManagerView *);
s32 ActorAttachment_DestroyManager(struct ActorAttachmentManagerView *);
struct ActorAttachmentManagerView *ActorAttachment_CreateManager(void)
{
    struct ActorAttachmentManagerView *manager;
    if (gActorAttachmentManager)
        return gActorAttachmentManager;
    manager = Task_Create(9, 0xA20);
    if (manager) {
        Task_SetCallbacks(manager, (void (*)(void *))ActorAttachment_TickManager,
                          (void (*)(void *))ActorAttachment_DestroyManager);
        if (ActorAttachment_InitManager(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}
