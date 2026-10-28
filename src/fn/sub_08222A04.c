#include "global.h"
#include "ram.h"
#include "serial.h"

extern s32 gUnk_03005318, gUnk_03005310;
void Serial_ProbeConnection(void);
void Serial_EnableReceiveInterrupt(void);
void Serial_EnableTimer3Polling(void);

s32 sub_08222A04(void)
{
    s32 i;
    gSerialByteReceiveWrite = gSerialByteReceiveRead = 0;
    gSerialByteSendWrite = gSerialByteSendRead = 0;
    gSerialByteReceiveCount = 0;
    gSerialByteSendCount = 0;
    for (i = 0; i <= 0x3F; i++) {
        gSerialByteReceiveBuffer[i] = 0;
        gSerialByteSendBuffer[i] = 0;
    }
    gSerialTransferStatus = 0;
    gSerialConnectionRole = -1;
    gUnk_03005310 = gUnk_03005318 = 0;
    Serial_ProbeConnection();
    if ((u32)gSerialTransferStatus >> 31) /* negative: error code */
        return gSerialTransferStatus;
    if (gSerialConnectionRole == 0) {
        Serial_EnableTimer3Polling();
        return 0;
    }
    Serial_EnableReceiveInterrupt();
    return 1;
}
