#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
void *ActorAttachment_UpdateFromDescriptor(void *, void *, u32, u32, u32, u32, u32, u32);
void Sound_PlaySong(u32);
// confidence=low: updates the actor's attachment effect for a given kind.
void Ratatosk_PlayAttachmentEffect(struct Ratatosk *p, u32 kind, s32 a, s32 b, u32 e, u32 f)
{
    switch (kind) {
        case 0:
            p->attachment =
                ActorAttachment_UpdateFromDescriptor(p->attachment, &p->body, 0, 1, 0, 0, e, f);
            Sound_PlaySong(0x1b1);
            break;
        case 2:
            p->attachment =
                ActorAttachment_UpdateFromDescriptor(p->attachment, &p->body, 0, 0, 0, 0, e, f);
            Sound_PlaySong(0x1ec);
            break;
        case 1:
            p->attachment =
                ActorAttachment_UpdateFromDescriptor(p->attachment, &p->body, 0, 3, 0, 0, e, f);
            Sound_PlaySong(0x1ec);
            break;
        case 3:
            p->attachment =
                ActorAttachment_UpdateFromDescriptor(p->attachment, &p->body, 0, 2, 0, 0, e, f);
            Sound_PlaySong(0x1ed);
            break;
        case 4:
            p->attachmentParams[0] = a;
            p->attachmentParams[1] = b;
            p->attachment = ActorAttachment_UpdateFromDescriptor(p->attachment, &p->body, 1, 0,
                                                                 (u32)p->attachmentParams,
                                                                 p->attachmentParams[1], e, f);
            break;
        case 5:
            p->attachmentParams[0] = a;
            p->attachmentParams[1] = b;
            p->attachment = ActorAttachment_UpdateFromDescriptor(p->attachment, &p->body, 3, 0,
                                                                 (u32)p->attachmentParams,
                                                                 p->attachmentParams[1], e, f);
            break;
        case 6:
            p->attachment =
                ActorAttachment_UpdateFromDescriptor(p->attachment, &p->body, 5, 0, 0, 0, e, f);
            break;
        case 7:
            p->attachment =
                ActorAttachment_UpdateFromDescriptor(p->attachment, &p->body, 4, 0, 0, 0, e, f);
            break;
        case 8:
            break;
    }
}
