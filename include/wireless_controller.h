#ifndef GUARD_WIRELESS_CONTROLLER_H
#define GUARD_WIRELESS_CONTROLLER_H

#include "task.h"

struct WirelessPeerRoster {
    u8 pending, sent, slots, count;
    u8 names[5][10];
    u16 ids[5];
};

struct WirelessParentInputPacket {
    u8 sequence, count, slots, unknown03;
    u16 input, childInput[4];
    u8 unknown0E[2];
};

struct WirelessChildInputPacket {
    u8 sequence, unknown01;
    u16 input;
};

enum WirelessOperation {
    WIRELESS_OP_PEER_HANDSHAKE = 7,
    WIRELESS_OP_WAIT_PARENT_TRANSFER = 9,
    WIRELESS_OP_RESET_AFTER_ERROR = 28,
    WIRELESS_OP_HANDLE_SEND_ERROR = 29,
    WIRELESS_OP_REQUEST_ADAPTER_STOP = 30,
};

/* The RFU controller task. Field names inferred from the session handlers
 * are tentative. The Engine tag is also used by the singleton declaration. */
typedef struct Engine {
    struct Task task;
    u8 managerReady, connectedSlots, previousConnectedSlots, refreshRequested;
    u8 operationStage, shutdownRequested, unknown1E;
    s8 localPeerIndex;
    u8 sendSlot, parentSlot;
    u16 niFlags;
    s8 peerScroll;
    u8 selectedSlots, availableSlots, availablePeerCount, availablePeerIndices[8];
    u8 isParent, inputReady, operationEntered, status, sessionMode, lostSlots, sendReady;
    u8 previousRosterSlots;
    u16 operation, unknown3A;
    u8 adapterDelay, resetRequested, receivedInput, disconnectMode;
    u16 timer;
    u8 unknown42, discoveredPeerCount;
    u32 operationFrame, frameCounter;
    void *connectionScript;
    u8 gameName[11], gameIdLow, gameIdHigh, unknown5D[3], userName[8], unknown68[4];
    u16 gameId;
    u8 unknown6E[2];
    struct WirelessParentInputPacket parentSend;
    struct WirelessChildInputPacket parentReceive[4], childSend;
    struct WirelessParentInputPacket childReceive;
    u32 exchangeValue;
    u8 exchangeWork[16], niSend[12], niReceive[12];
    u8 lastSequence[4], repeatedSequence[4], missedSequence[4], aheadSequence[4];
    u32 ackWaitFrames;
    u8 sendStalls, receiveStalls, receivedSlots, acknowledgedSlots;
    u8 waitingForAck, retransmitDone;
    u16 previousSentInput, pendingInput, localInput, inputHistory[5];
    u8 inputUpdated, unknownFB, unknownFC[4];
    u16 inputChanges[5];
    u8 unknown10A[2];
    u32 exchangeKey;
    u16 exchangeInput[5];
    u8 unknown11A[2], rfuConfig[24];
    struct WirelessPeerRoster roster;
    u8 packetQueue[0x204];
    void *onSend, *onReceive, *onSendFailure, *onReceiveFailure;
    void *gameNameScript;
} WirelessController;

static inline void WirelessController_SetOperation(WirelessController *controller, u16 operation)
{
    controller->operationStage = 0;
    controller->operation = operation;
    controller->operationFrame = 0;
    controller->operationEntered = 1;
}

/* Both the manager event and the send-completion callback report parent acknowledgements. */
#define WIRELESS_RECORD_PARENT_ACK(controller, flags, expectedSlots)                               \
    do {                                                                                           \
        u8 *seen = &(controller)->acknowledgedSlots;                                               \
        int bits = ((flags) << 1) | *seen;                                                         \
        int expected;                                                                              \
        (controller)->acknowledgedSlots = bits;                                                    \
        expected = (expectedSlots);                                                                \
        if ((bits & expected) == expected) {                                                       \
            (controller)->waitingForAck = 0;                                                       \
            (controller)->sendReady = 0;                                                           \
        } else {                                                                                   \
            (controller)->waitingForAck = 1;                                                       \
            (controller)->ackWaitFrames++;                                                         \
        }                                                                                          \
    } while (0)

#endif
