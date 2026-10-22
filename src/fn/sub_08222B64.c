#include "global.h"
extern s32 gUnk_0300169C;
extern s32 gUnk_030016A4;
extern u8 gUnk_030016E8[64];
extern s32 gUnk_03005324;
void sub_0822253C(void);
void sub_08222548(void);

s32 sub_08222B64(u8 *data, s32 length)
{
    s32 head, used, i;
    if (gUnk_03005324 < 0)
        return gUnk_03005324;
    sub_0822253C();
    head = gUnk_0300169C;
    used = gUnk_030016A4;
    sub_08222548();
    used++;
    used += length;
    if (used <= 63) {
        gUnk_030016E8[head] = length;
        head = (head + 1) & 63;
        for (i = 0; i < length; i++) {
            gUnk_030016E8[head] = *data++;
            head = (head + 1) & 63;
        }
        sub_0822253C();
        gUnk_0300169C = head;
        gUnk_030016A4 += length + 1;
        sub_08222548();
        return length;
    }
    return 0;
}
