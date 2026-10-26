#include "global.h"
#include "ram.h"
extern s32 gUnk_03001694;
extern s32 gUnk_03001698;
extern u8 gUnk_030016A8[64];
void sub_0822253C(void);
void sub_08222548(void);

s32 sub_08222AC4(u8 *data)
{
    s32 tail, used, length, i;
    if (gSerialTransferStatus < 0)
        return gSerialTransferStatus;
    sub_0822253C();
    tail = gUnk_03001694;
    used = gUnk_03001698;
    sub_08222548();
    if (used > 0) {
        length = gUnk_030016A8[tail];
        tail = (tail + 1) & 63;
        if (length > 63) {
            gSerialTransferStatus = -6;
            return -6;
        }
        if (length + 1 <= used) {
            for (i = 0; i < length; i++) {
                *data++ = gUnk_030016A8[tail];
                tail = (tail + 1) & 63;
            }
            sub_0822253C();
            gUnk_03001698 -= length + 1;
            gUnk_03001694 = tail;
            sub_08222548();
            return length;
        }
    }
    return 0;
}
