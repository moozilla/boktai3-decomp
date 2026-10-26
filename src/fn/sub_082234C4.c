#define Interrupts_DisableMaster Serial_DisableGlobalInterrupts
#define Interrupts_EnableMaster  Serial_EnableGlobalInterrupts
#include "global.h"
#include "ram.h"
void Serial_DisableGlobalInterrupts(void);
void Serial_EnableGlobalInterrupts(void);
s32 Serial_ReadLatestPacket(u16 *out)
{
    s32 status = gSerialTransferStatus;
    if (status < 0 && status > -3) {
        out[0] = 0xFFFF;
        out[1] = 0xFFFF;
        out[2] = 0xFFFF;
        out[3] = 0xFFFF;
        return status;
    }
    Serial_DisableGlobalInterrupts();
    {
        u16 *packets = gSerialLatestPackets;
        out[0] = packets[gSerialLatestPacketIndex * 4];
        out[1] = *(gSerialLatestPacketIndex * 4 + &gSerialLatestPackets[1]);
        out[2] = *(gSerialLatestPacketIndex * 4 + &gSerialLatestPackets[2]);
        out[3] = *(gSerialLatestPacketIndex * 4 + &gSerialLatestPackets[3]);
    }
    Serial_EnableGlobalInterrupts();
    return gSerialConnectionRole;
}
