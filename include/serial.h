#ifndef GUARD_SERIAL_H
#define GUARD_SERIAL_H

#include "global.h"

/* RAM objects; each row of symbols/ram.csv with header include/serial.h. */
extern s32 gSerialByteReceiveWrite; /* Write cursor of the 64-byte serial receive ring */
extern s32 gSerialByteReceiveRead;  /* Read cursor of the 64-byte serial receive ring */
extern s32 gSerialByteReceiveCount; /* Queued receive-byte count */
extern s32 gSerialByteSendWrite;    /* Write cursor of the 64-byte serial send ring */
extern s32 gSerialByteSendRead;     /* Read cursor of the 64-byte serial send ring */
extern s32 gSerialByteSendCount;    /* Queued send-byte count */
extern u8 gSerialByteReceiveBuffer
    [64]; /* 64-byte receive ring; independent from the adjacent send ring */
extern u8 gSerialByteSendBuffer
    [64]; /* 64-byte send ring with its own literal base; prior interior classification was adjacency */
extern u16
    gSerialQueuedSendWord; /* Queued multiplayer send word, moved into the active word when ready */
extern u16 gSerialActiveSendWord; /* Current multiplayer send word, with protocol status bits */
extern u8 gRfuApiBuffer[]; /* RFU API workspace passed to rfu_initializeAPI with size 0xE64 */
extern u8 gRfuIrqSlots
    []; /* Interior view of interrupt dispatch table entry 1, passed as RFU handler slots; not an independent allocation */
extern u16
    gSerialMultiplayer1Word; /* Last word sampled from SIOMULTI1 by the byte-transfer interrupt */
extern s32
    gSerialPeer1Interrupts; /* Counter advanced by the SIOMULTI1 byte-transfer handler; tentative name */
extern u32
    gSerialInitialized; /* Set to 1 by Serial_InitMultiplayer; no semantic reader observed here */
extern s32
    gSerialPeer0Interrupts; /* Counter advanced by the SIOMULTI0 byte-transfer handler; tentative name */
extern u16
    gSerialMultiplayer0Word; /* Last word sampled from SIOMULTI0 by the byte-transfer interrupt */
extern u16 gSerialQueuedPacketCount; /* Number of queued four-halfword multiplayer packets */
extern u16 gSerialPacketQueue
    [4][4]; /* Four queued packets of four halfwords, with ring indices masked by 3 */
extern u16
    gSerialPacketReadIndex; /* Current read index in the four-entry multiplayer packet ring */

#endif
