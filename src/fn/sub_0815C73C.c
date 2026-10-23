#include "global.h"
void *ActorAttachment_UpdateFromDescriptor(void *, void *, u32, u32, void *, s32, s32, s32);
void Sound_PlaySong(u32);
#define B(o)   (*(u8 *)(p + (o)))
#define H(o)   (*(u16 *)(p + (o)))
#define ATTACH (*(void **)(p + 0xB04))
#define START(n, sound)                                                                            \
    B(0xB08) = 0;                                                                                  \
    H(0xB0A) = 0;                                                                                  \
    ATTACH = ActorAttachment_UpdateFromDescriptor(ATTACH, p + 0x24, B(0xB08), n, 0, 0, 0, 80);     \
    Sound_PlaySong(sound)
#define EMIT(mode)                                                                                 \
    B(0xB08) = mode;                                                                               \
    H(0xB0A) = value;                                                                              \
    ATTACH = ActorAttachment_UpdateFromDescriptor(ATTACH, p + 0x24, B(0xB08), 0, p + 0xB0A,        \
                                                  (s16)arg, 150, 230)
#ifdef NONMATCHING
// fuzzy: 73.0%
// confidence=low: selects player attachment descriptors and associated sound effects.
#define Player_ConfigureAttachmentEffect sub_0815C73C
void Player_ConfigureAttachmentEffect(u8 *p, u32 action, s32 value, s32 arg)
{
    switch (action) {
        case 0:
            START(1, 0x1B1);
            break;
        case 1:
            START(3, 0x1EC);
            break;
        case 2:
            START(0, 0x1EC);
            break;
        case 3:
            START(2, 0x1ED);
            break;
        case 4:
            EMIT(1);
            break;
        case 5:
            EMIT(3);
            break;
        case 6:
            B(0xB08) = 5;
            H(0xB0A) = 0;
            ATTACH =
                ActorAttachment_UpdateFromDescriptor(ATTACH, p + 0x24, B(0xB08), 0, 0, 0, 150, 230);
            break;
        case 7:
            B(0xB08) = 4;
            H(0xB0A) = 0;
            ATTACH =
                ActorAttachment_UpdateFromDescriptor(ATTACH, p + 0x24, B(0xB08), 0, 0, 0, 150, 230);
            break;
        case 8:
            H(0xB0A) = 0;
            ATTACH =
                ActorAttachment_UpdateFromDescriptor(ATTACH, p + 0x24, B(0xB08), 127, 0, 0, 0, 80);
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0815C73C);
#endif
