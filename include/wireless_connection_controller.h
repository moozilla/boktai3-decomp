#ifndef GUARD_WIRELESS_CONNECTION_CONTROLLER_H
#define GUARD_WIRELESS_CONNECTION_CONTROLLER_H

#include "engine.h"
#define gRfuLinkStatus    gUnk_03006A80
#define gRfuSlotStatusNI  gUnk_03006A70
#define gRfuSlotStatusUNI gUnk_03006A60
/* The game used an int-width declaration here; the SDK itself narrows to u8. */
#define rfu_clearSlot rfu_clearSlot_sdk
#include "rfu_compat.h"
#undef rfu_clearSlot
u16 rfu_clearSlot(int, int);

/* Observed 0x38C-byte singleton. Names describe current interpretations;
 * unknown fields and packet extents are provisional. UNI/NI buffers are wire
 * bytes, not additional partial views of the controller. */
struct WirelessController {
    struct Task task; /* 0x00..0x17 */
    u8 adapterInitialized, connectedSlots, previousConnectedSlots;
    u8 peerRefreshRequested, advanceRequested, destroyRequested;
    u8 connectionState, participantIndex, sendSlot, parentSlot;
    u16 handshakeFlags;
    s8 peerScroll;
    u8 selectedPeer, availableSlots, peerCount, peerIds[8];
    u8 active, inputReady, operationEntered, status, sessionMode, lostSlots, sendReady,
        previousParticipantMask;
    u16 operation, operationValue;
    u8 setupDelay, resetRequested, inputBlocked, disconnectMode;
    u16 operationTimer;
    u8 unk42, unk43;
    u32 operationTicks, frameCounter;
    u32 sessionStartScript; /* 0x4C */
    u8 gameName[11], gameIdLow, gameIdHigh, unk5D[3], userName[9], unk69[3];
    u16 gameId;
    u8 unk6E[2];
    u8 parentUniPacket[16], parentUniReceive[16];
    u8 childUniPacket[20]; /* four-byte child send record, then 16-byte parent receive record */
    u32 handshakeNumber;
    u8 handshakeWork[16];
    u8 niSend[12], niReceive[12];
    u8 previousSequence[4], duplicatePackets[4], stalePackets[4], missedPackets[4];
    u32 ackWaitFrames;
    u8 receiveAheadCount, sendAheadCount, receivedSlots, acknowledgedSlots, awaitingAck,
        slotsChanged;
    u16 previousInput, localInput, savedLocalInput, inputHistory[5];
    u8 inputChanged, resyncRequested, unkFC[4];
    u16 inputDiff[5];
    u8 unk10A[2];
    u32 exchangeKey;
    u16 exchangedInput[5];
    u8 unk11A[2], linkConfiguration[0x18];
    u8 peerRoster[64];     /* 0x134: flags, participant mask/count, names, IDs */
    u8 packetQueue[0x204]; /* 0x174 */
    void *onSend, *onReceive, *onSendFailure, *onReceiveFailure;
    void *identityScript; /* 0x388 */
};

/* RFU manager event storage, shared by byte notifications and NI/UNI events. */
union WirelessControllerEvent {
    u8 bytes[0x18];
    struct {
        u8 unk0[0x14];
        u16 code;
        u16 value;
    } fields;
};
extern struct WirelessController *gUnk_02000484;
extern union WirelessControllerEvent gUnk_03005390;

u32 WirelessController_GetInstance(void);
s32 WirelessController_IsCreated(void);
void WirelessController_ResetSession(struct WirelessController *p);
void WirelessController_ResetPeerExchange(struct WirelessController *p);
u32 WirelessController_SetLocalInput(struct WirelessController *p, u16 v);
int WirelessController_RestoreLocalInput(struct WirelessController *p);
int WirelessController_PollInputExchange(struct WirelessController *p);
u32 WirelessController_ConsumePeerRefreshRequest(struct WirelessController *p);
u32 WirelessController_ConsumeAdvanceRequest(struct WirelessController *p);
void WirelessController_RebuildAvailablePeerList(struct WirelessController *p);
int WirelessController_BuildPeerRosterPacket(struct WirelessController *p);
int WirelessController_UpdatePeerRosterSend(struct WirelessController *p);
void WirelessController_PollPeerRosterSend(struct WirelessController *p);
void WirelessController_OnLinkEvent(u8 command);
void WirelessController_OnReceiveRequest(void);
void sub_080349FC(void);
u32 WirelessController_ClearOperationEntryFlag(struct WirelessController *p);
int WirelessController_InitLinkCallbacks(struct WirelessController *p);
int WirelessController_UpdateAdapterSetup(struct WirelessController *p);
int WirelessController_StartConfiguredLink(struct WirelessController *p);
u32 Wireless_ClearIdleEntry(struct WirelessController *p);
int Wireless_StartChildSearch(struct WirelessController *p);
int Wireless_StartParentSearch(struct WirelessController *p);
int Wireless_UpdatePeerHandshake(struct WirelessController *p);
int Wireless_SelectParentFromInput(struct WirelessController *p);
int Wireless_WaitForParentTransfer(struct WirelessController *p);
int Wireless_UpdateDisconnectTimeout(struct WirelessController *p);
int Wireless_UpdateSearchRestartDelay(struct WirelessController *p);
int Wireless_DisconnectAllSlots(struct WirelessController *p);
int Wireless_UpdateParentSearchDelay(struct WirelessController *p);
int Wireless_UpdateChildHandshake(struct WirelessController *p);
u32 Wireless_ClearFirstHandshakeWaitEntry(struct WirelessController *p);
u32 Wireless_ClearSecondHandshakeWaitEntry(struct WirelessController *p);
u32 Wireless_ClearThirdHandshakeWaitEntry(struct WirelessController *p);
u32 Wireless_ClearFourthHandshakeWaitEntry(struct WirelessController *p);
int Wireless_WaitForReset(struct WirelessController *p);
int Wireless_ResetAfterError(struct WirelessController *p);
int Wireless_HandleSendError(struct WirelessController *p);
u32 Wireless_RequestAdapterStop(struct WirelessController *p);
int Wireless_RequestDisconnect(struct WirelessController *p);
u32 Wireless_ResetControllerAndSlots(struct WirelessController *p);
void Wireless_UpdateUniAcknowledgedSlots(struct WirelessController *p);
int Wireless_ReceiveChildUniPacket(struct WirelessController *p);
void Wireless_ProcessChildUniExchange(struct WirelessController *p);
void Wireless_ResetRemoteInputHistory(struct WirelessController *p);
void Wireless_ReceiveParentUniPackets(struct WirelessController *argument);
void Wireless_BuildParentUniPacket(struct WirelessController *argument);
void Wireless_RemoveConnectedSlots(struct WirelessController *p, u8 slots);
void Wireless_OnParentUniSessionEvent(u8 command);
void Wireless_OnChildUniSessionEvent(u8 command);
void Wireless_OnParentUniSendComplete(void);
void Wireless_OnChildUniClockEvent(u16 v);
int Wireless_PublishLocalInput(struct WirelessController *p);
int Wireless_PublishLocalInputAlternate(struct WirelessController *p);
void Wireless_OnParentNiSessionEvent(u8 command);
void Wireless_OnParentNiSendComplete(void);
void Wireless_BeginParentNiSession(struct WirelessController *p);
int Wireless_TryStartParentNiSend(struct WirelessController *p);
void Wireless_PollParentNiSend(struct WirelessController *p);
int Wireless_EnterParentNiTransferWait(struct WirelessController *p);
int Wireless_PollParentNiReceive(struct WirelessController *p);
u32 Wireless_ClearParentTransferEntryFlag(struct WirelessController *p);
void Wireless_OnChildTransferEvent(u8 command);
void Wireless_OnChildTransferReceiveRequest(void);
void Wireless_BeginChildTransfer(struct WirelessController *p);
int Wireless_ExchangeChildNiPacket(struct WirelessController *argument);
void Wireless_PollChildNiSend(struct WirelessController *p);
int Wireless_EnterChildTransferWait(struct WirelessController *p);
u32 Wireless_ClearChildTransferEntryFlag(struct WirelessController *p);
u32 Wireless_ClearChildTransferWaitEntryFlag(struct WirelessController *p);
int Wireless_UpdateConnectionSession(struct WirelessController *argument);
int Wireless_UpdateInactiveConnectionSession(struct WirelessController *p);
int Wireless_UpdateParentUniSession(struct WirelessController *p);
int Wireless_UpdateChildUniSession(struct WirelessController *p);
void Wireless_UpdateParentNiSession(struct WirelessController *argument);
int Wireless_UpdateChildNiSession(struct WirelessController *p);
int Wireless_UpdateControllerTask(struct WirelessController *p);
s32 Wireless_InitControllerFromScript(struct WirelessController *p);
struct WirelessController *Wireless_CreateController(void);
void Wireless_UpdateController(void);
void Wireless_RequestInputResync(void);
s32 sub_080369F0(void);
int Wireless_GetConnectedParentSlot(void);
int Wireless_IsSessionConnected(void);
u8 *Wireless_GetPeerHandshakeData(void);
u32 Wireless_IsSessionActive(void);
u8 Wireless_GetConnectedSlotMask(void);
u16 Wireless_GetOperation(void);
void Wireless_RequestDisconnectPeer(void);
void Wireless_RequestAdvanceConnection(void);
void Wireless_SetTransferCallbacks(void *sent, void *received, void *sendFailure,
                                   void *receiveFailure);
int Wireless_QueueNiPacket(u8 *packet);
s32 sub_08036BAC(void *dst);
void Wireless_SetStatus(u32 v);
u32 Wireless_GetStatus(void);
void Wireless_RequestDisconnectMode(void);
void Wireless_RequestResetConnection(void);
void Wireless_RequestDestroyController(void);
u8 *sub_08036C6C(void);
u16 Wireless_GetLocalSessionId(void);
/* Measured against loads/stores throughout 08034084..08036CA4. */
typedef char WirelessController_EventValueOffsetCheck
    [offsetof(union WirelessControllerEvent, fields.value) == 0x16 ? 1 : -1];
typedef char WirelessController_SizeCheck[sizeof(struct WirelessController) == 0x38C ? 1 : -1];
typedef char WirelessController_RosterOffsetCheck
    [offsetof(struct WirelessController, peerRoster) == 0x134 ? 1 : -1];
typedef char WirelessController_QueueOffsetCheck
    [offsetof(struct WirelessController, packetQueue) == 0x174 ? 1 : -1];
typedef char WirelessController_CallbackOffsetCheck
    [offsetof(struct WirelessController, onSend) == 0x378 ? 1 : -1];
void sub_080347D8(u8 command);
void sub_080359E4(struct WirelessController *p);
void sub_08035B98(struct WirelessController *p);
s32 sub_08035CD0(struct WirelessController *p);
#endif
