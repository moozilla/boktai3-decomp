#include "actor_attachment_manager.h"
#define ActorAttachment_CreateManager sub_08066C34
s32 sub_08066C0C(struct ActorAttachmentManagerView *);
s32 sub_08066AE0(struct ActorAttachmentManagerView *);
s32 sub_08066B28(struct ActorAttachmentManagerView *);
struct ActorAttachmentManagerView *ActorAttachment_CreateManager(void)
{
    struct ActorAttachmentManagerView *manager;
    if (gUnk_02000124)
        return gUnk_02000124;
    manager = Task_Create(9, 0xA20);
    if (manager) {
        Task_SetCallbacks(manager, (void (*)(void *))sub_08066AE0, (void (*)(void *))sub_08066B28);
        if (sub_08066C0C(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}
