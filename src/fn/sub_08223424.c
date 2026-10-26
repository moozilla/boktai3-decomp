#define Interrupts_DisableMaster Serial_DisableGlobalInterrupts
#define Interrupts_EnableMaster  Serial_EnableGlobalInterrupts
#include "global.h"
#include "ram.h"
struct SerialPacket {
    u16 words[4];
};
void Serial_DisableGlobalInterrupts(void);
void Serial_EnableGlobalInterrupts(void);
s32 Serial_ReceivePacket(u16 *out)
{
    s32 result;
    if (gSerialTransferStatus < 0)
        return gSerialTransferStatus;
    Serial_DisableGlobalInterrupts();
    if (gSerialReceivePending) {
        u16 *packets = gSerialReceivePackets;
        out[0] = packets[gSerialReceiveIndex * 4];
        out[1] = *(gSerialReceiveIndex * 4 + &gSerialReceivePackets[1]);
        out[2] = *(gSerialReceiveIndex * 4 + &gSerialReceivePackets[2]);
        out[3] = *(gSerialReceiveIndex * 4 + &gSerialReceivePackets[3]);
        gSerialReceivePending = 0;
        result = gSerialConnectionRole;
    } else
        result = -3;
    Serial_EnableGlobalInterrupts();
    return result;
}
