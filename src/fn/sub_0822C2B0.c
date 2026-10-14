#include "global.h"
extern u32 gUnk_030053F4;
extern u8 *gUnk_02000580;
extern u8 *gUnk_02000710;
extern u16 gUnk_02000528;
s32 sub_0822C238(s32);
s32 sub_0822C290(s32);
s32 sub_0822C2A0(s32);
static inline s32 modeIs(s32 m)
{
    u16 mode = gUnk_02000528;
    s32 r = 0;
    if (mode == m)
        r = 1;
    return r;
}
s32 sub_0822C2B0(s32 v)
{
    if (gUnk_030053F4 & 0x800)
        return v;
    if (gUnk_02000580) {
        u32 *flags = (u32 *)(gUnk_02000580 + 0x444);
        u32 bit = 0x40000;
        if (*flags & bit) {
            s32 band = sub_0822C238(v) * 2;
            if (band > 10)
                band = 10;
            v = sub_0822C290(band);
        }
    }
    if (*(s16 *)(gUnk_02000710 + 0x610)) {
        if (sub_0822C238(v) <= 1)
            v = sub_0822C2A0(2);
    } else if (*(s16 *)(gUnk_02000710 + 0x612)) {
        if (sub_0822C238(v) > 2)
            v = sub_0822C290(2);
    }
    if (modeIs(1)) {
        s32 band = sub_0822C238(v) + 4;
        if (band > 10)
            band = 10;
        v = sub_0822C290(band);
    } else if (modeIs(2))
        v = 0;
    return v;
}
