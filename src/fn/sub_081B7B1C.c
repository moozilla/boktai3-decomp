#include "global.h"

typedef struct GrowthHazardManager {
    u8 unk_00[0x18];
    u8 count;
    u8 pending;
    u8 flash[2];
    u16 palette[2][16];
} Obj;

extern u8 *gUnk_030042E4;
extern const u32 gUnk_08612154[];
void Sound_PlaySong(u32);

#ifdef NONMATCHING
// fuzzy: 96.3%
// scope shape 8 (differences once low registers are set aside)
// Starts 32-tick palette flashes and blends their last 15 ticks.
void GrowthHazard_UpdateSharedFlash(Obj *manager)
{
    s32 channel;
    if (manager->pending) {
        if (manager->pending & 1) {
            Sound_PlaySong(0x1A9);
            manager->flash[0] = 32;
        }
        if (manager->pending & 2) {
            Sound_PlaySong(0x13E);
            manager->flash[1] = 32;
        }
        manager->pending = 0;
    }
    for (channel = 0; channel < 2; channel++) {
        if (manager->flash[channel]) {
            manager->flash[channel]--;
            if (manager->flash[channel] == 0)
                CpuSet(gUnk_030042E4 + 0x1AA0, manager->palette[channel], 0x04000008);
            else if (manager->flash[channel] > 15) {
                const u32 *banks = gUnk_08612154;
                CpuSet(gUnk_030042E4 + banks[channel] * 32, manager->palette[channel], 0x04000008);
            } else {
                s32 flash = manager->flash[channel];
                s32 inverse = 16 - flash;
                const u32 *banks = gUnk_08612154;
                u16 *from = (u16 *)(gUnk_030042E4 + banks[channel] * 32);
                u16 *to = (u16 *)(gUnk_030042E4 + 0x1AA0);
                s32 colorIndex;
                u32 channelMask;
                colorIndex = 0;
                channelMask = 31;
                do {
                    u32 fromColor = *from;
                    s32 fromRed = 31;
                    s32 fromGreen;
                    u32 toColor;
                    s32 toRed;
                    s32 toGreen;
                    s32 red, green, blue;
                    fromRed &= fromColor;
                    fromColor <<= 16;
                    fromGreen = fromColor >> 21;
                    fromGreen &= channelMask;
                    fromColor >>= 26;
                    fromColor &= channelMask;
                    toColor = *to;
                    toRed = 31;
                    toRed &= toColor;
                    toColor <<= 16;
                    toGreen = toColor >> 21;
                    toGreen &= channelMask;
                    toColor >>= 26;
                    toColor &= channelMask;
                    red = (fromRed * flash + toRed * inverse) >> 4;
                    green = (fromGreen * flash + toGreen * inverse) >> 4;
                    blue = ((s32)fromColor * flash + (s32)toColor * inverse) >> 4;
                    manager->palette[channel][colorIndex] = (blue << 10) | (green << 5) | red;
                    from++;
                    to++;
                } while (++colorIndex < 16);
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", GrowthHazard_UpdateSharedFlash);
#endif
