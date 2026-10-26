#include "global.h"
#include "ram.h"

extern s32 gUnk_03001690, gUnk_03001698, gUnk_030016A0, gUnk_030016A4;
extern u8 gUnk_030016A8[64], gUnk_030016E8[64];
extern s32 gUnk_03005310;
extern u16 gUnk_0300530C;

/* Timer 3 handler for the multiplayer serial link: reads the three peer
   words, queues received data and sends the next queued byte. */
void sub_082225FC(void)
{
    u16 a = *(vu16 *)0x04000122;
    u32 b = *(vu16 *)0x04000124;
    u32 c = *(vu16 *)0x04000126;
    u32 send = 0;
    gUnk_0300530C = a;
    if ((*(vu16 *)0x04000128 & 0xC0) || b != 0xFFFF || c != b) {
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
    }
    *(vu16 *)0x04000128 |= 0x80;
    *(vu16 *)0x0400010C = 0xF5D8;
    gUnk_03005310++;
}
