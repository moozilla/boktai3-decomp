#include "global.h"
#include "ram.h"
extern s32 gUnk_0300169C;
extern s32 gUnk_030016A4;
extern u8 gUnk_030016E8[64];
void sub_0822253C(void);
void sub_08222548(void);

s32 sub_08222B64(u8 *data, s32 length)
{
    s32 head, used, i;
    if (gSerialTransferStatus < 0)
        return gSerialTransferStatus;
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
