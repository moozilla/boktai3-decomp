#include "global.h"
#include "ram.h"
void sub_08223044(void);
extern u32 gUnk_03005314;
extern u16 gUnk_0300172A, gUnk_03001728, gUnk_03005330, gUnk_03005380;
extern u16 gUnk_03005350[][4];
s32 sub_08223180(void)
{
    s32 i, j;
    gUnk_03005314 = 1;
    sub_08223044();
    gSerialReceivePending = 0;
    gUnk_0300172A = 0x4000;
    gUnk_03001728 = 0x4000;
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
    return 0;
}
