#define Interrupts_DisableMaster Serial_DisableGlobalInterrupts
#define Interrupts_EnableMaster  Serial_EnableGlobalInterrupts
#define gSerialTransferStatus    gUnk_03005324
#define gSerialConnectionRole    gUnk_0300531C
#define gSerialLatestPacketIndex gUnk_03001734
#define gSerialLatestPackets     gUnk_03005340
#include "global.h"
extern s32 gUnk_03005324, gUnk_0300531C;
extern u32 gUnk_03001734;
extern u16 gUnk_03005340[];
void Serial_DisableGlobalInterrupts(void);
void Serial_EnableGlobalInterrupts(void);
s32 Serial_ReadLatestPacket(u16 *out)
{
    s32 status = gUnk_03005324;
    if (status < 0 && status > -3) {
        out[0] = 0xFFFF;
        out[1] = 0xFFFF;
        out[2] = 0xFFFF;
        out[3] = 0xFFFF;
        return status;
    }
    Serial_DisableGlobalInterrupts();
    {
        u16 *packets = gUnk_03005340;
        out[0] = packets[gUnk_03001734 * 4];
        out[1] = *(gUnk_03001734 * 4 + &gUnk_03005340[1]);
        out[2] = *(gUnk_03001734 * 4 + &gUnk_03005340[2]);
        out[3] = *(gUnk_03001734 * 4 + &gUnk_03005340[3]);
    }
    Serial_EnableGlobalInterrupts();
    return gUnk_0300531C;
}
