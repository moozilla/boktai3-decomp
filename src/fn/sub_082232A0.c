#include "global.h"
#include "ram.h"
#include "serial.h"
void sub_08223044(void);
extern u16 gUnk_03005330, gUnk_03005380;
extern u16 gUnk_03005350[][4];
void SerialLink_InitMultiplayer(void)
{
    s32 i, j;
    sub_08223044();
    gSerialReceivePending = 0;
    gSerialActiveSendWord = 0x4000;
    gSerialQueuedSendWord = 0x4000;
    gSerialReceivePackets[0] = 65535;
    gSerialReceivePackets[1] = -1;
    gSerialReceivePackets[2] = -1;
    gSerialReceivePackets[3] = -1;
    gSerialReceivePackets[4] = -1;
    gSerialReceivePackets[5] = -1;
    gSerialReceivePackets[6] = -1;
    gSerialReceivePackets[7] = -1;
    gSerialReceiveIndex = 0;
    gSerialLatestPackets[0] = -1;
    gSerialLatestPackets[1] = -1;
    gSerialLatestPackets[2] = -1;
    gSerialLatestPackets[3] = -1;
    gSerialLatestPackets[4] = -1;
    gSerialLatestPackets[5] = -1;
    gSerialLatestPackets[6] = -1;
    gSerialLatestPackets[7] = -1;
    gSerialLatestPacketIndex = 0;
    gUnk_03005330 = 0;
    gUnk_03005380 = 0;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            gUnk_03005350[i][j] |= -1;
}
