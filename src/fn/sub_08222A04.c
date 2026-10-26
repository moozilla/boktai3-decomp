#include "global.h"
#include "ram.h"

extern s32 gUnk_03001690, gUnk_03001694, gUnk_03001698, gUnk_0300169C, gUnk_030016A0, gUnk_030016A4;
extern u8 gUnk_030016A8[64], gUnk_030016E8[64];
extern s32 gUnk_03005318, gUnk_03005310;
void Serial_ProbeConnection(void);
void Serial_EnableReceiveInterrupt(void);
void Serial_EnableTimer3Polling(void);

s32 sub_08222A04(void)
{
    s32 i;
    gUnk_03001690 = gUnk_03001694 = 0;
    gUnk_0300169C = gUnk_030016A0 = 0;
    gUnk_03001698 = 0;
    gUnk_030016A4 = 0;
    for (i = 0; i <= 0x3F; i++) {
        gUnk_030016A8[i] = 0;
        gUnk_030016E8[i] = 0;
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
