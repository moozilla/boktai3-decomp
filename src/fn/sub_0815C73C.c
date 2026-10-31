#include "player.h"
void *ActorAttachment_UpdateFromDescriptor(void *, void *, u32, u32, void *, s32, s32, s32);
void Sound_PlaySong(u32);
// confidence=low: selects player attachment descriptors and associated sound effects.
static inline void Attachment_SetValue(u16 *dest, s32 value)
{
    *dest = value;
}

static inline void Attachment_SetMode(u8 *dest, s32 value)
{
    *dest = value;
}

void Player_ConfigureAttachmentEffect(struct Player *p, u32 action, s32 value, s32 arg)
{
    switch (action) {
        case 0:
            Attachment_SetMode(&p->kindTwoAttachmentDescriptor, 0);
            Attachment_SetValue(&p->kindTwoAttachmentCounter, 0);
            p->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                p->kindTwoAttachment, &p->positionalEffectGroup, p->kindTwoAttachmentDescriptor, 1,
                0, 0, 0, 80);
            Sound_PlaySong(0x1B1);
            break;

        case 1:
            Attachment_SetMode(&p->kindTwoAttachmentDescriptor, 0);
            Attachment_SetValue(&p->kindTwoAttachmentCounter, 0);
            p->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                p->kindTwoAttachment, &p->positionalEffectGroup, p->kindTwoAttachmentDescriptor, 3,
                0, 0, 0, 80);
            Sound_PlaySong(0x1EC);
            break;

        case 2:
            Attachment_SetMode(&p->kindTwoAttachmentDescriptor, 0);
            Attachment_SetValue(&p->kindTwoAttachmentCounter, 0);
            p->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                p->kindTwoAttachment, &p->positionalEffectGroup, p->kindTwoAttachmentDescriptor, 0,
                0, 0, 0, 80);
            Sound_PlaySong(0x1EC);
            break;

        case 3:
            Attachment_SetMode(&p->kindTwoAttachmentDescriptor, 0);
            Attachment_SetValue(&p->kindTwoAttachmentCounter, 0);
            p->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                p->kindTwoAttachment, &p->positionalEffectGroup, p->kindTwoAttachmentDescriptor, 2,
                0, 0, 0, 80);
            Sound_PlaySong(0x1ED);
            break;

        case 4:
            Attachment_SetMode(&p->kindTwoAttachmentDescriptor, 1);
            Attachment_SetValue(&p->kindTwoAttachmentCounter, value);
            p->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                p->kindTwoAttachment, &p->positionalEffectGroup, p->kindTwoAttachmentDescriptor, 0,
                &p->kindTwoAttachmentCounter, (s16)arg, 150, 230);
            break;

        case 5:
            Attachment_SetMode(&p->kindTwoAttachmentDescriptor, 3);
            Attachment_SetValue(&p->kindTwoAttachmentCounter, value);
            p->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                p->kindTwoAttachment, &p->positionalEffectGroup, p->kindTwoAttachmentDescriptor, 0,
                &p->kindTwoAttachmentCounter, (s16)arg, 150, 230);
            break;

        case 6:
            p->kindTwoAttachmentDescriptor = 5;
            Attachment_SetValue(&p->kindTwoAttachmentCounter, 0);
            p->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                p->kindTwoAttachment, &p->positionalEffectGroup, p->kindTwoAttachmentDescriptor, 0,
                0, 0, 150, 230);
            break;

        case 7:
            p->kindTwoAttachmentDescriptor = 4;
            Attachment_SetValue(&p->kindTwoAttachmentCounter, 0);
            p->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                p->kindTwoAttachment, &p->positionalEffectGroup, p->kindTwoAttachmentDescriptor, 0,
                0, 0, 150, 230);
            break;

        case 8:
            Attachment_SetValue(&p->kindTwoAttachmentCounter, 0);
            p->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                p->kindTwoAttachment, &p->positionalEffectGroup, p->kindTwoAttachmentDescriptor,
                127, 0, 0, 0, 80);
            break;
    }
}
