#define Interrupts_DisableMaster Serial_DisableGlobalInterrupts
#define Interrupts_EnableMaster  Serial_EnableGlobalInterrupts
#define gSerialTransferStatus    gUnk_03005324
#define gSerialConnectionRole    gUnk_0300531C
#define gSerialReceivePending    gUnk_03001730
#define gSerialReceiveIndex      gUnk_0300172C
#define gSerialReceivePackets    gUnk_03005370
#include "global.h"
struct SerialPacket {
    u16 words[4];
};
extern s32 gUnk_03005324, gUnk_0300531C;
extern u32 gUnk_03001730, gUnk_0300172C;
extern u16 gUnk_03005370[];
void Serial_DisableGlobalInterrupts(void);
void Serial_EnableGlobalInterrupts(void);
s32 Serial_ReceivePacket(u16 *out)
{
    s32 result;
    if (gUnk_03005324 < 0)
        return gUnk_03005324;
    Serial_DisableGlobalInterrupts();
    if (gUnk_03001730) {
        u16 *packets = gUnk_03005370;
        out[0] = packets[gUnk_0300172C * 4];
        out[1] = *(gUnk_0300172C * 4 + &gUnk_03005370[1]);
        out[2] = *(gUnk_0300172C * 4 + &gUnk_03005370[2]);
        out[3] = *(gUnk_0300172C * 4 + &gUnk_03005370[3]);
        gUnk_03001730 = 0;
        result = gUnk_0300531C;
    } else
        result = -3;
    Serial_EnableGlobalInterrupts();
    return result;
}
