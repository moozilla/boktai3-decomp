#include "global.h"
#include "ram.h"
#include "serial.h"
void sub_0822253C(void);
void sub_08222548(void);

s32 sub_08222B64(u8 *data, s32 length)
{
    s32 head, used, i;
    if (gSerialTransferStatus < 0)
        return gSerialTransferStatus;
    sub_0822253C();
    head = gSerialByteSendWrite;
    used = gSerialByteSendCount;
    sub_08222548();
    used++;
    used += length;
    if (used <= 63) {
        gSerialByteSendBuffer[head] = length;
        head = (head + 1) & 63;
        for (i = 0; i < length; i++) {
            gSerialByteSendBuffer[head] = *data++;
            head = (head + 1) & 63;
        }
        sub_0822253C();
        gSerialByteSendWrite = head;
        gSerialByteSendCount += length + 1;
        sub_08222548();
        return length;
    }
    return 0;
}
