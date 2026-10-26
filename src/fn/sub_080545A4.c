// MATCHING_CONCESSION: register-pinning
#include "hit_switch.h"
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
            s32 inverse;
            u16 *destination;
            s32 flash = manager->flash;
            u16 *from;
            u16 *to;
            s32 i;
            register u32 mask asm("r12");
            u8 *bank;
            inverse = 16 - flash;
            bank = gPaletteBankData;
            {
                u16 *address = (u16 *)(bank + 0x1AC0);
                from = address;
            }
            {
                register u32 offset asm("r5") = 0x1AA0;
                asm("" : "+r"(offset));
                offset += (u32)bank;
                to = (u16 *)offset;
            }
            {
                s32 start = 0;
                i = start;
            }
            destination = manager->palette;
            {
                u32 value = 31;
                mask = value;
            }
            do {
                u16 *read = from;
                register u32 a asm("r3");
                s32 ar;
                s32 ag;
                u32 b;
                register s32 br asm("r0");
                register s32 bg asm("r4");
                s32 r, g, blue;
                a = *read;
                ar = a & 31;
                a <<= 16;
                ag = a >> 21;
                {
                    register u32 channelMask asm("r5") = mask;
                    ag &= channelMask;
                    a = (a >> 26) & channelMask;
                    b = *to;
                    br = b & 31;
                    b <<= 16;
                    bg = (b >> 21) & channelMask;
                    b = (b >> 26) & channelMask;
                }
                r = (ar * flash + br * inverse) >> 4;
                g = (ag * flash + bg * inverse) >> 4;
                blue = ((s32)a * flash + (s32)b * inverse) >> 4;
                {
                    s32 index = i;
                    u16 *out;
                    u16 *base = destination;
                    out = base + index;
                    *out = (blue << 10) | (g << 5) | r;
                }
                {
                    u32 step = 2;
                    from = (u16 *)((u8 *)from + step);
                    to = (u16 *)((u8 *)to + step);
                }
                {
                    register u32 one asm("r1") = 1;
                    i += one;
                }
                {
                    s32 count = i;
                    if (count > 15)
                        break;
                }
            } while (1);
        }
    }
}
