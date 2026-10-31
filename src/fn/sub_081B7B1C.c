#include "global.h"
#include "render.h"

struct GrowthHazardManager {
    u8 unk_00[0x18];
    u8 count;
    u8 pending;
    u8 flash[2];
    u16 palette[2][16];
};

extern const u32 gUnk_08612154[];
void Sound_PlaySong(u32);

static inline u16 GrowthHazard_ReadColor(const u16 *color)
{
    return *color;
}
static inline u32 GrowthHazard_ReadBank(const u32 *bank)
{
    return *bank;
}

static inline u32 GrowthHazard_MaskChannel(u32 value, const u32 *mask)
{
    return value & *mask;
}
// Starts 32-tick palette flashes and blends their last 15 ticks.
void GrowthHazard_UpdateSharedFlash(struct GrowthHazardManager *manager)
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
                CpuSet(gPaletteBankData + 0x1AA0, manager->palette[channel], 0x04000008);
            else if (manager->flash[channel] > 15) {
                const u32 *banks = gUnk_08612154;
                CpuSet(gPaletteBankData + GrowthHazard_ReadBank(banks + channel) * 32,
                       manager->palette[channel], 0x04000008);
            } else {
                s32 flash = manager->flash[channel];
                s32 inverse = 16 - flash;
                const u32 *banks = gUnk_08612154;
                u16 *from = (u16 *)(gPaletteBankData + GrowthHazard_ReadBank(banks + channel) * 32);
                u16 *to = (u16 *)(gPaletteBankData + 0x1AA0);
                s32 colorIndex;
                u32 channelMask;
                colorIndex = 0;
                channelMask = 31;
                do {
                    u32 fromColor = GrowthHazard_ReadColor(from);
                    s32 fromRed = 31;
                    s32 fromGreen;
                    u32 toColor;
                    s32 toRed;
                    s32 toGreen;
                    s32 red, green, blue;
                    fromRed &= fromColor;
                    fromColor <<= 16;
                    fromGreen = fromColor >> 21;
                    fromGreen = GrowthHazard_MaskChannel(fromGreen, &channelMask);
                    fromColor >>= 26;
                    fromColor = GrowthHazard_MaskChannel(fromColor, &channelMask);
                    toColor = GrowthHazard_ReadColor(to);
                    toRed = 31;
                    toRed &= toColor;
                    toColor <<= 16;
                    toGreen = toColor >> 21;
                    toGreen = GrowthHazard_MaskChannel(toGreen, &channelMask);
                    toColor >>= 26;
                    toColor = GrowthHazard_MaskChannel(toColor, &channelMask);
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
