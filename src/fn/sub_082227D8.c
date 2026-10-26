#include "global.h"
#include "ram.h"

extern s32 gUnk_03001690, gUnk_03001698, gUnk_030016A0, gUnk_030016A4;
extern u8 gUnk_030016A8[64], gUnk_030016E8[64];
extern s32 gUnk_03005318;
extern u16 gUnk_03005320;

/* Serial interrupt handler for the multiplayer link: reads the peer words,
   queues received data, sends the next queued byte and latches the role. */
void sub_082227D8(void)
{
    u16 a = *(vu16 *)0x04000120;
    u32 b = *(vu16 *)0x04000124;
    u32 c = *(vu16 *)0x04000126;
    u32 send = 0;
    gUnk_03005320 = a;
    if ((*(vu16 *)0x04000128 & 0x40) || b != 0xFFFF || c != b) {
        if (b != 0xFFFF || c != b)
            gSerialTransferStatus = -7;
        else
            gSerialTransferStatus = -8;
    } else {
        if ((a & 0x2000) && a != c) {
            gUnk_030016A8[gUnk_03001690] = a;
            gUnk_03001690 = (gUnk_03001690 + 1) & 0x3F;
            gUnk_03001698++;
        }
        if (gUnk_03001698 > 0x3E)
            send |= 0x4000;
        if (gUnk_030016A4 > 0 && !(a & 0x4000)) {
            send |= gUnk_030016E8[gUnk_030016A0];
            gUnk_030016A0 = (gUnk_030016A0 + 1) & 0x3F;
            gUnk_030016A4--;
            send |= 0x2000;
        }
        if (send == 0xFFFF)
            send = 0x8000;
        *(vu16 *)0x0400012A = send;
        if (gSerialConnectionRole == -1) {
            gSerialConnectionRole = (*(vu16 *)0x04000128 >> 4) & 3;
            if (gSerialConnectionRole != 1)
                gSerialTransferStatus = -5;
        }
    }
    gUnk_03005318++;
}
