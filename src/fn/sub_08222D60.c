// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
#ifdef NONMATCHING
// fuzzy: 90.3%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
extern u16 gSerialActiveSendWord, gSerialQueuedSendWord, gSerialQueuedPacketCount,
    gSerialPacketReadIndex;
extern u16 gSerialPacketQueue[];
#define IO(a) (*(volatile u16 *)(a))
static inline void CopyQueued(u16 *out)
{
    u16 *q = gSerialPacketQueue;
    out[0] = q[gSerialPacketReadIndex * 4];
    out[1] = *(gSerialPacketReadIndex * 4 + &gSerialPacketQueue[1]);
    out[2] = *(gSerialPacketReadIndex * 4 + &gSerialPacketQueue[2]);
    out[3] = *(gSerialPacketReadIndex * 4 + &gSerialPacketQueue[3]);
}
static inline void QueuePacket(u16 *in, u32 pos)
{
    u16 *q = gSerialPacketQueue;
    q[pos * 4] = in[0];
    *(pos * 4 + &gSerialPacketQueue[1]) = in[1];
    *(pos * 4 + &gSerialPacketQueue[2]) = in[2];
    *(pos * 4 + &gSerialPacketQueue[3]) = in[3];
}
static inline void CopyPacket(u16 *out, u16 *in)
{
    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
    out[3] = in[3];
}
static inline void AdvanceSend(void)
{
    if (gSerialActiveSendWord & 0x4000) {
        gSerialActiveSendWord = gSerialQueuedSendWord;
        gSerialQueuedSendWord = 0x4000;
    }
}
void Serial_ServiceInterrupt(void)
{
    u32 control;
    typeof(gSerialPacketReadIndex) t1;
    u32 k2 = 2;
    *((volatile u16 *)0x04000120);
    gSerialTransferStatus = 0;
    control = *((volatile u16 *)0x04000128);
    if ((control & 0x40) || ((control & 0xb0) == 0x80))
        gSerialTransferStatus = -k2;
    else {
        u32 bits = 0;
        u16 *receive = &gSerialReceivePackets[(1 - gSerialReceiveIndex) * 4];
        u16 *latest = &gSerialLatestPackets[(1 - gSerialLatestPacketIndex) * 4];
        u32 word = *((volatile u16 *)0x04000120);
        register u32 first asm("r8") = word;
        latest[0] = word;
        if (!(word & 0x8000))
            bits = word;
        word = *((volatile u16 *)0x04000122);
        latest[1] = word;
        if (!(word & 0x8000))
            bits |= word;
        word = *((volatile u16 *)0x04000124);
        latest[k2] = word;
        if (!(word & 0x8000))
            bits |= word;
        word = *((volatile u16 *)0x04000126);
        latest[3] = word;
        if (!(word & 0x8000))
            bits |= word;
        if (!(bits & 0x4000)) {
            if (gSerialReceivePending) {
                u32 pos = (gSerialPacketReadIndex + gSerialQueuedPacketCount) & 3;
                gSerialPacketQueue[pos * 4] = first;
                *((pos * 4) + (&gSerialPacketQueue[1])) = latest[1];
                *((pos * 4) + (&gSerialPacketQueue[k2])) = latest[k2];
                *((pos * 4) + (&gSerialPacketQueue[3])) = latest[3];
                ++gSerialQueuedPacketCount;
                gSerialActiveSendWord = 0x4000;
            } else {
                if (gSerialQueuedPacketCount) {
                    volatile u16 count;
                    CopyQueued(receive);
                    {
                        u8 k1 = 3;
                        gSerialPacketReadIndex = (t1 = (gSerialPacketReadIndex + 1) & k1);
                    }
                    count = gSerialQueuedPacketCount;
                    --gSerialQueuedPacketCount;
                    QueuePacket(latest, (t1 + gSerialQueuedPacketCount) & 3);
                    gSerialQueuedPacketCount = count;
                } else {
                    receive[0] = first;
                    receive[1] = latest[1];
                    receive[k2] = latest[k2];
                    receive[3] = latest[3];
                }
                gSerialActiveSendWord = gSerialQueuedSendWord;
                gSerialQueuedSendWord = 0x4000;
                gSerialReceiveIndex = 1 - gSerialReceiveIndex;
                gSerialReceivePending = 1;
            }
        } else if ((!gSerialReceivePending) && gSerialQueuedPacketCount) {
            CopyQueued(receive);
            gSerialPacketReadIndex = (gSerialPacketReadIndex + 1) & 3;
            --gSerialQueuedPacketCount;
            AdvanceSend();
            gSerialReceiveIndex = 1 - gSerialReceiveIndex;
            gSerialReceivePending = 1;
        } else
            AdvanceSend();
        *((volatile u16 *)0x0400012A) = gSerialActiveSendWord;
        gSerialConnectionRole = ((*((volatile u16 *)0x04000128)) >> 4) & 3;
        gSerialLatestPacketIndex = 1 - gSerialLatestPacketIndex;
    }
    *((volatile u16 *)0x03007FF8) = 0x80;
}

#else
INCLUDE_ASM("asm/nonmatching", Serial_ServiceInterrupt);
#endif
