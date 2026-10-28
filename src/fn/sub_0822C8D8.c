#include "global.h"
#include "engine.h"
#include "runtime.h"
extern u8 *gUnk_020004B4;
extern u16 gUnk_03005260[];

u16 sub_0822C8D8(u8 *p)
{
    u16 *keys;
    if (gEngineFlags & 0x800) {
        s32 i, k;
        if (gUnk_020004B4 == NULL) {
            k = 1;
            i = -k;
        } else
            i = *(s16 *)(gUnk_020004B4 + 0x18);
        keys = &gUnk_03005260[i * 4];
    } else
        keys = gUnk_03005260;
    if (keys[0] & 1) {
        if (keys[0] & 0x200) {
            if (!(++*(u16 *)(p + 0x22) & 1) && gSolarSimulatedIntensity < 0x8c)
                gSolarSimulatedIntensity++;
        } else if (keys[0] & 0x100) {
            if (!(++*(u16 *)(p + 0x22) & 1) && gSolarSimulatedIntensity != 0)
                gSolarSimulatedIntensity--;
        }
    }
    return gSolarSimulatedIntensity;
}
