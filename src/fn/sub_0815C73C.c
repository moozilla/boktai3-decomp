#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
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
// confidence=low: selects player attachment descriptors and associated sound effects.
#define Player_ConfigureAttachmentEffect sub_0815C73C
static inline u32 Arg1(u32 k_, u8 *p)
{
    return p + k_;
}

void Player_ConfigureAttachmentEffect(u8 *p, u32 action, s32 value, s32 arg)
{
    switch (action) {
        case 0:
            *((u8 *)(p + 0xB08)) = 0;
            *((u16 *)(p + 0xB0A)) = 0;
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 1, 0, 0, 0, 80);
            Sound_PlaySong(0x1B1);
            break;

        case 1:
            *((u8 *)(p + 0xB08)) = 0;
            *((u16 *)(p + 0xB0A)) = 0;
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 3, 0, 0, 0, 80);
            Sound_PlaySong(0x1EC);
            break;

        case 2:
            *((u8 *)(p + 0xB08)) = 0;
            *((u16 *)(p + 0xB0A)) = 0;
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 0, 0, 0, 0, 80);
            Sound_PlaySong(0x1EC);
            break;

        case 3:
            *((u8 *)(p + 0xB08)) = 0;
            *((u16 *)(p + 0xB0A)) = 0;
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 2, 0, 0, 0, 80);
            Sound_PlaySong(0x1ED);
            break;

        case 4:
            *((u8 *)(p + 0xB08)) = 1;
            *((u16 *)(p + 0xB0A)) = value;
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 0, p + 0xB0A, (s16)arg,
                150, 230);
            break;

        case 5:
            *((u8 *)(p + 0xB08)) = 3;
            *((u16 *)(p + 0xB0A)) = value;
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 0, p + 0xB0A, (s16)arg,
                150, 230);
            break;

        case 6:
            *((u8 *)(p + 0xB08)) = 5;
            {
                u16 *d1 = (u16 *)(p + 0xB04);
                {
                    u8 k1 = 0;
                    d1[3] = k1;
                }
            }
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 0, 0, 0, 150, 230);
            break;

        case 7:
            *((u8 *)(p + 0xB08)) = 4;
            *((u16 *)(p + 0xB0A)) = 0;
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)(p + 0xB08)), 0, 0, 0, 150, 230);
            break;

        case 8:
            *((u16 *)(p + 0xB0A)) = 0;
            *((void **)(p + 0xB04)) = ActorAttachment_UpdateFromDescriptor(
                *((void **)(p + 0xB04)), p + 0x24, *((u8 *)Arg1(0xB08, p)), 127, 0, 0, 0, 80);
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0815C73C);
#endif
