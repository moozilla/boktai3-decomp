#include "global.h"
#include "ram.h"
#include "serial.h"
#include "gba/io_reg.h"

#define SERIAL_QUEUE_TAIL(index)                                                                   \
    do {                                                                                           \
        (index) = (gSerialPacketReadIndex + gSerialQueuedPacketCount) & 3;                         \
    } while (0)

static inline void CopyQueued(u16 *out)
{
    out[0] = gSerialPacketQueue[gSerialPacketReadIndex][0];
    out[1] = gSerialPacketQueue[gSerialPacketReadIndex][1];
    out[2] = gSerialPacketQueue[gSerialPacketReadIndex][2];
    out[3] = gSerialPacketQueue[gSerialPacketReadIndex][3];
}
static inline void QueuePacket(const u16 *packet, u16 index)
{
    gSerialPacketQueue[index][0] = packet[0];
    gSerialPacketQueue[index][1] = packet[1];
    gSerialPacketQueue[index][2] = packet[2];
    gSerialPacketQueue[index][3] = packet[3];
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
    u16 nextRead;
    s32 writeSlot;

    (void)REG_SIOMULTI0;
    gSerialTransferStatus = 0;
    control = REG_SIOCNT;
    if ((control & 0x40) || ((control & 0xb0) == 0x80))
        gSerialTransferStatus = -2;
    else {
        u32 bits = 0;
        u16 *receive = &gSerialReceivePackets[(1 - gSerialReceiveIndex) * 4];
        u16 *latest = &gSerialLatestPackets[(1 - gSerialLatestPacketIndex) * 4];
        u32 word = REG_SIOMULTI0;
        u16 first = word;
        latest[0] = word;
        if (!(word & 0x8000))
            bits = word;
        word = REG_SIOMULTI1;
        latest[1] = word;
        if (!(word & 0x8000))
            bits |= word;
        word = REG_SIOMULTI2;
        latest[2] = word;
        if (!(word & 0x8000))
            bits |= word;
        word = REG_SIOMULTI3;
        latest[3] = word;
        if (!(word & 0x8000))
            bits |= word;
        if (!(bits & 0x4000)) {
            if (gSerialReceivePending) {
                SERIAL_QUEUE_TAIL(writeSlot);
                gSerialPacketQueue[writeSlot][0] = first;
                gSerialPacketQueue[writeSlot][1] = latest[1];
                gSerialPacketQueue[writeSlot][2] = latest[2];
                gSerialPacketQueue[writeSlot][3] = latest[3];
                ++gSerialQueuedPacketCount;
                gSerialActiveSendWord = 0x4000;
            } else {
                if (gSerialQueuedPacketCount) {
                    u16(*serialValue)[4] = gSerialPacketQueue;
                    CopyQueued(receive);
                    nextRead = gSerialPacketReadIndex;
                    nextRead++;
                    nextRead &= 3;
                    gSerialPacketReadIndex = nextRead;
                    {
                        u16 savedDepth[1] = {gSerialQueuedPacketCount};
                        --gSerialQueuedPacketCount;
                        SERIAL_QUEUE_TAIL(writeSlot);
                        QueuePacket(latest, writeSlot);
                        gSerialQueuedPacketCount = savedDepth[0];
                    }
                } else {
                    receive[0] = first;
                    receive[1] = latest[1];
                    receive[2] = latest[2];
                    receive[3] = latest[3];
                }
                gSerialActiveSendWord = gSerialQueuedSendWord;
                gSerialQueuedSendWord = 0x4000;
                gSerialReceiveIndex = 1 - gSerialReceiveIndex;
                gSerialReceivePending = 1;
            }
        } else if (!gSerialReceivePending && gSerialQueuedPacketCount) {
            CopyQueued(receive);
            gSerialPacketReadIndex = (gSerialPacketReadIndex + 1) & 3;
            --gSerialQueuedPacketCount;
            AdvanceSend();
            gSerialReceiveIndex = 1 - gSerialReceiveIndex;
            gSerialReceivePending = 1;
        } else
            AdvanceSend();
        REG_SIOMLT_SEND = gSerialActiveSendWord;
        gSerialConnectionRole = (REG_SIOCNT >> 4) & 3;
        gSerialLatestPacketIndex = 1 - gSerialLatestPacketIndex;
    }
    INTR_CHECK = 0x80;
}
