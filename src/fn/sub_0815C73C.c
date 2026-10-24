#include "global.h"
void *ActorAttachment_UpdateFromDescriptor(void *, void *, u32, u32, void *, s32, s32, s32);
void Sound_PlaySong(u32);
// confidence=low: selects player attachment descriptors and associated sound effects.
#define Player_ConfigureAttachmentEffect sub_0815C73C
static inline void Attachment_SetValue(u16 *dest, s32 value)
{
    *dest = value;
}

struct AttachmentModeField {
    u8 mode;
};

static inline void Attachment_SetMode(u8 *dest, s32 value)
{
    *dest = value;
}

void Player_ConfigureAttachmentEffect(u8 *p, u32 action, s32 value, s32 arg)
{
    switch (action) {
        case 0:
            Attachment_SetMode(p + 0xB08, 0);
            Attachment_SetValue((u16 *)(p + 0xB0A), 0);
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 1, 0, 0, 0, 80);
            Sound_PlaySong(0x1B1);
            break;

        case 1:
            Attachment_SetMode(p + 0xB08, 0);
            Attachment_SetValue((u16 *)(p + 0xB0A), 0);
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 3, 0, 0, 0, 80);
            Sound_PlaySong(0x1EC);
            break;

        case 2:
            Attachment_SetMode(p + 0xB08, 0);
            Attachment_SetValue((u16 *)(p + 0xB0A), 0);
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 0, 0, 0, 0, 80);
            Sound_PlaySong(0x1EC);
            break;

        case 3:
            Attachment_SetMode(p + 0xB08, 0);
            Attachment_SetValue((u16 *)(p + 0xB0A), 0);
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 2, 0, 0, 0, 80);
            Sound_PlaySong(0x1ED);
            break;

        case 4:
            Attachment_SetMode(p + 0xB08, 1);
            Attachment_SetValue((u16 *)(p + 0xB0A), value);
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 0, p + 0xB0A, (s16)arg,
                150, 230);
            break;

        case 5:
            Attachment_SetMode(p + 0xB08, 3);
            Attachment_SetValue((u16 *)(p + 0xB0A), value);
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 0, p + 0xB0A, (s16)arg,
                150, 230);
            break;

        case 6:
            ((struct AttachmentModeField *)(p + 0xB08))->mode = 5;
            Attachment_SetValue((u16 *)(p + 0xB0A), 0);
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 0, 0, 0, 150, 230);
            break;

        case 7:
            ((struct AttachmentModeField *)(p + 0xB08))->mode = 4;
            Attachment_SetValue((u16 *)(p + 0xB0A), 0);
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 0, 0, 0, 150, 230);
            break;

        case 8:
            Attachment_SetValue((u16 *)(p + 0xB0A), 0);
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 127, 0, 0, 0, 80);
            break;
    }
}
