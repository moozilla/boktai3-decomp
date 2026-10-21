#include "global.h"
#define Ratatosk_PlayAttachmentEffect sub_081F6A14
struct Actor {
    u8 pad0[0x48];
    u8 body[0x104];
    void *handle;
    s16 a, b;
};
void *ActorAttachment_UpdateFromDescriptor(void *, void *, u32, u32, u32, u32, u32, u32);
void Sound_PlaySong(u32);
// confidence=low: updates the actor's attachment effect for a given kind.
void Ratatosk_PlayAttachmentEffect(struct Actor *p, u32 kind, s32 a, s32 b, u32 e, u32 f)
{
    switch (kind) {
        case 0:
            p->handle = ActorAttachment_UpdateFromDescriptor(p->handle, p->body, 0, 1, 0, 0, e, f);
            Sound_PlaySong(0x1b1);
            break;
        case 2:
            p->handle = ActorAttachment_UpdateFromDescriptor(p->handle, p->body, 0, 0, 0, 0, e, f);
            Sound_PlaySong(0x1ec);
            break;
        case 1:
            p->handle = ActorAttachment_UpdateFromDescriptor(p->handle, p->body, 0, 3, 0, 0, e, f);
            Sound_PlaySong(0x1ec);
            break;
        case 3:
            p->handle = ActorAttachment_UpdateFromDescriptor(p->handle, p->body, 0, 2, 0, 0, e, f);
            Sound_PlaySong(0x1ed);
            break;
        case 4:
            p->a = a;
            p->b = b;
            p->handle = ActorAttachment_UpdateFromDescriptor(p->handle, p->body, 1, 0, (u32)&p->a,
                                                             p->b, e, f);
            break;
        case 5:
            p->a = a;
            p->b = b;
            p->handle = ActorAttachment_UpdateFromDescriptor(p->handle, p->body, 3, 0, (u32)&p->a,
                                                             p->b, e, f);
            break;
        case 6:
            p->handle = ActorAttachment_UpdateFromDescriptor(p->handle, p->body, 5, 0, 0, 0, e, f);
            break;
        case 7:
            p->handle = ActorAttachment_UpdateFromDescriptor(p->handle, p->body, 4, 0, 0, 0, e, f);
            break;
        case 8:
            break;
    }
}
