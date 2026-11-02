#include "hit_switch.h"

static inline u32 HitSwitch_MaskChannel(u32 value, const u32 *mask)
{
    return value & *mask;
}

void HitSwitch_UpdateSharedFlash(struct HitSwitchManager *manager)
{
    if (manager->hitPending) {
        Sound_PlaySong(425);
        manager->flash = 32;
        manager->hitPending = 0;
    }
    if (manager->flash) {
        manager->flash--;
        if (manager->flash == 0)
            CpuSet(gPaletteBankData + 0x1AA0, manager->palette, 0x04000008);
        else if (manager->flash > 15)
            CpuSet(gPaletteBankData + 0x1AC0, manager->palette, 0x04000008);
        else {
            s32 flash = manager->flash;
            s32 inverse = 16 - flash;
            u16 *from = (u16 *)(gPaletteBankData + 0x1AC0);
            u16 *to = (u16 *)(gPaletteBankData + 0x1AA0);
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
                fromGreen = HitSwitch_MaskChannel(fromGreen, &channelMask);
                fromColor >>= 26;
                fromColor = HitSwitch_MaskChannel(fromColor, &channelMask);
                toColor = *to;
                toRed = 31;
                toRed &= toColor;
                toColor <<= 16;
                toGreen = toColor >> 21;
                toGreen = HitSwitch_MaskChannel(toGreen, &channelMask);
                toColor >>= 26;
                toColor = HitSwitch_MaskChannel(toColor, &channelMask);
                red = (fromRed * flash + toRed * inverse) >> 4;
                green = (fromGreen * flash + toGreen * inverse) >> 4;
                blue = ((s32)fromColor * flash + (s32)toColor * inverse) >> 4;
                manager->palette[colorIndex] = (blue << 10) | (green << 5) | red;
                from++;
                to++;
            } while (++colorIndex < 16);
        }
    }
}
