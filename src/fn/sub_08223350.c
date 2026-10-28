#include "global.h"
#include "ram.h"
#include "serial.h"
void Serial_ResetMultiplayer_082230AC(void);
void SerialLink_ResetMultiplayer(void)
{
    s32 i, j;
    Serial_ResetMultiplayer_082230AC();
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
    gSerialQueuedPacketCount = 0;
    gSerialPacketReadIndex = 0;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            gSerialPacketQueue[i][j] |= -1;
}
