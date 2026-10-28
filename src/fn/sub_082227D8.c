#include "global.h"
#include "ram.h"
#include "serial.h"

/* Serial interrupt handler for the multiplayer link: reads the peer words,
   queues received data, sends the next queued byte and latches the role. */
void sub_082227D8(void)
{
    u16 a = *(vu16 *)0x04000120;
    u32 b = *(vu16 *)0x04000124;
    u32 c = *(vu16 *)0x04000126;
    u32 send = 0;
    gSerialMultiplayer0Word = a;
    if ((*(vu16 *)0x04000128 & 0x40) || b != 0xFFFF || c != b) {
        if (b != 0xFFFF || c != b)
            gSerialTransferStatus = -7;
        else
            gSerialTransferStatus = -8;
    } else {
        if ((a & 0x2000) && a != c) {
            gSerialByteReceiveBuffer[gSerialByteReceiveWrite] = a;
            gSerialByteReceiveWrite = (gSerialByteReceiveWrite + 1) & 0x3F;
            gSerialByteReceiveCount++;
        }
        if (gSerialByteReceiveCount > 0x3E)
            send |= 0x4000;
        if (gSerialByteSendCount > 0 && !(a & 0x4000)) {
            send |= gSerialByteSendBuffer[gSerialByteSendRead];
            gSerialByteSendRead = (gSerialByteSendRead + 1) & 0x3F;
            gSerialByteSendCount--;
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
    gSerialPeer0Interrupts++;
}
