#ifndef GUARD_SERIAL_H
#define GUARD_SERIAL_H

#include "global.h"

/* RAM objects; each row of symbols/ram.csv with header include/serial.h. */
/* 0x03001690 */
extern s32 gSerialByteReceiveWrite; /* Write cursor of the 64-byte serial receive ring */
/* 0x03001694 */
extern s32 gSerialByteReceiveRead; /* Read cursor of the 64-byte serial receive ring */
/* 0x03001698 */
extern s32 gSerialByteReceiveCount; /* Queued receive-byte count */
/* 0x0300169C */
extern s32 gSerialByteSendWrite; /* Write cursor of the 64-byte serial send ring */
/* 0x030016A0 */
extern s32 gSerialByteSendRead; /* Read cursor of the 64-byte serial send ring */
/* 0x030016A4 */
extern s32 gSerialByteSendCount; /* Queued send-byte count */
/* 0x030016A8 */
extern u8 gSerialByteReceiveBuffer
    [64]; /* 64-byte receive ring; independent from the adjacent send ring */
/* 0x030016E8 */
extern u8 gSerialByteSendBuffer
    [64]; /* 64-byte send ring with its own literal base; prior interior classification was adjacency */
/* 0x03001728 */
extern u16
    gSerialQueuedSendWord; /* Queued multiplayer send word, moved into the active word when ready */
/* 0x0300172A */
extern u16 gSerialActiveSendWord; /* Current multiplayer send word, with protocol status bits */
/* 0x03001738 */
extern u8 gRfuApiBuffer[0xE64]; /* RFU API workspace passed to rfu_initializeAPI with size 0xE64 */
/* 0x03003A14 */
extern u8 gRfuIrqSlots
    []; /* Interior view of interrupt dispatch table entry 1, passed as RFU handler slots; not an independent allocation */
/* 0x0300530C */
extern u16
    gSerialMultiplayer1Word; /* Last word sampled from SIOMULTI1 by the byte-transfer interrupt */
/* 0x03005310 */
extern s32
    gSerialPeer1Interrupts; /* Counter advanced by the SIOMULTI1 byte-transfer handler; tentative name */
/* 0x03005314 */
extern u32
    gSerialInitialized; /* Set to 1 by Serial_InitMultiplayer; no semantic reader observed here */
/* 0x03005318 */
extern s32
    gSerialPeer0Interrupts; /* Counter advanced by the SIOMULTI0 byte-transfer handler; tentative name */
/* 0x03005320 */
extern u16
    gSerialMultiplayer0Word; /* Last word sampled from SIOMULTI0 by the byte-transfer interrupt */
/* 0x03005330 */
extern u16 gSerialQueuedPacketCount; /* Number of queued four-halfword multiplayer packets */
/* 0x03005350 */
extern u16 gSerialPacketQueue
    [4][4]; /* Four queued packets of four halfwords, with ring indices masked by 3 */
/* 0x03005380 */
extern u16
    gSerialPacketReadIndex; /* Current read index in the four-entry multiplayer packet ring */

#endif
