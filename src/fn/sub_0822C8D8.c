#include "global.h"
#include "engine.h"
#include "runtime.h"
#include "input.h"
extern u8 *gUnk_020004B4;

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
        keys = &gPadState[i * 4];
    } else
        keys = gPadState;
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
