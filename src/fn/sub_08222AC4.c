#include "global.h"
#include "ram.h"
#include "serial.h"
void sub_0822253C(void);
void sub_08222548(void);

s32 sub_08222AC4(u8 *data)
{
    s32 tail, used, length, i;
    if (gSerialTransferStatus < 0)
        return gSerialTransferStatus;
    sub_0822253C();
    tail = gSerialByteReceiveRead;
    used = gSerialByteReceiveCount;
    sub_08222548();
    if (used > 0) {
        length = gSerialByteReceiveBuffer[tail];
        tail = (tail + 1) & 63;
        if (length > 63) {
            gSerialTransferStatus = -6;
            return -6;
        }
        if (length + 1 <= used) {
            for (i = 0; i < length; i++) {
                *data++ = gSerialByteReceiveBuffer[tail];
                tail = (tail + 1) & 63;
            }
            sub_0822253C();
            gSerialByteReceiveCount -= length + 1;
            gSerialByteReceiveRead = tail;
            sub_08222548();
            return length;
        }
    }
    return 0;
}
