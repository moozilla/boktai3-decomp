#include "global.h"
#define ActorAttachment_PlayEffect sub_0817AB2C

struct AttachmentEffectActor {
    u8 unk0[0x48];
    u8 descriptor[0x3D8];
    void *attachment;
    u8 descriptorIndex;
};
void *ActorAttachment_UpdateFromDescriptor(void *, void *, int, int, void *, int, int, int);
void Sound_PlaySong(u32);
void sub_0817AB2C(struct AttachmentEffectActor *actor, s32 effect)
{
    switch (effect) {
    case 1:
        actor->attachment = ActorAttachment_UpdateFromDescriptor(actor->attachment, actor->descriptor, actor->descriptorIndex, 1, 0, 0, 0, 0x50);
        Sound_PlaySong(0x1B1);
        break;
    case 3:
        actor->attachment = ActorAttachment_UpdateFromDescriptor(actor->attachment, actor->descriptor, actor->descriptorIndex, 3, 0, 0, 0, 0x50);
        Sound_PlaySong(0x1EC);
        break;
    case 2:
        actor->attachment = ActorAttachment_UpdateFromDescriptor(actor->attachment, actor->descriptor, actor->descriptorIndex, 2, 0, 0, 0, 0x50);
        Sound_PlaySong(0x1ED);
        break;
    }
}
