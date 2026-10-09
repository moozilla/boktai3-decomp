/* Adapted from pret/pokeemerald 731ad5bfd6e6f265508d0efcca0ba42f9dcf5881, LIBRFU_VERSION 1024.
 * Reconstructed SDK source retains upstream licensing status; see THIRD_PARTY.md and docs/RFU.md. */
// CFLAGS: -O2 -mthumb-interwork
#define AgbRFU_SoftReset sub_08244010
#define STWI_init sub_08244834
#define STWI_init_Callback_M sub_08244138
#define STWI_init_Callback_S sub_08244144
#define STWI_init_all sub_08243EFC
#define STWI_init_timer sub_08243FD4
#define STWI_intr_timer sub_082446F8
#define STWI_poll_CommandEnd sub_08244174
#define STWI_read_status sub_082440D8
#define STWI_reset_ClockCounter sub_08244A00
#define STWI_restart_Command sub_08244980
#define STWI_send_CPR_EndREQ sub_082446B0
#define STWI_send_CPR_PollingREQ sub_0824468C
#define STWI_send_CPR_StartREQ sub_08244650
#define STWI_send_CP_EndREQ sub_08244438
#define STWI_send_CP_PollingREQ sub_08244414
#define STWI_send_CP_StartREQ sub_082443E4
#define STWI_send_ConfigStatusREQ sub_08244250
#define STWI_send_DataReadyAndChangeREQ sub_08244534
#define STWI_send_DataRxREQ sub_082444EC
#define STWI_send_DataTxAndChangeREQ sub_082444A4
#define STWI_send_DataTxREQ sub_0824445C
#define STWI_send_DisconnectREQ sub_082445E8
#define STWI_send_DisconnectedAndChangeREQ sub_08244584
#define STWI_send_GameConfigREQ sub_08244274
#define STWI_send_LinkStatusREQ sub_082441C0
#define STWI_send_MS_ChangeREQ sub_08244510
#define STWI_send_ResetREQ sub_0824419C
#define STWI_send_ResumeRetransmitAndChangeREQ sub_082445C4
#define STWI_send_SC_EndREQ sub_08244354
#define STWI_send_SC_PollingREQ sub_08244330
#define STWI_send_SC_StartREQ sub_0824430C
#define STWI_send_SP_EndREQ sub_082443C0
#define STWI_send_SP_PollingREQ sub_0824439C
#define STWI_send_SP_StartREQ sub_08244378
#define STWI_send_SlotStatusREQ sub_0824422C
#define STWI_send_StopModeREQ sub_082446D4
#define STWI_send_SystemConfigREQ sub_082442CC
#define STWI_send_SystemStatusREQ sub_08244208
#define STWI_send_TestModeREQ sub_08244618
#define STWI_send_VersionStatusREQ sub_082441E4
#define STWI_set_Callback_ID sub_08244168
#define STWI_set_Callback_M sub_08244150
#define STWI_set_Callback_S sub_0824415C
#define STWI_set_MS_mode sub_082440C4
#define STWI_set_timer sub_0824475C
#define STWI_start_Command sub_0824490C
#define STWI_stop_timer sub_08244808
#define rfu_CB_CHILD_pollConnectRecovery sub_08246824
#define rfu_CB_configGameData sub_082459F8
#define rfu_CB_defaultCallback sub_08245670
#define rfu_CB_disconnect sub_082466D4
#define rfu_CB_pollAndEndSearchChild sub_08245B98
#define rfu_CB_pollConnectParent sub_08245F6C
#define rfu_CB_pollSearchParent sub_08245DA8
#define rfu_CB_recvData sub_0824784C
#define rfu_CB_reset sub_082458E0
#define rfu_CB_sendData sub_08247370
#define rfu_CB_sendData2 sub_08247408
#define rfu_CB_sendData3 sub_08247418
#define rfu_CB_startSearchChild sub_08245AE4
#define rfu_CB_startSearchParent sub_08245D70
#define rfu_CB_stopMode sub_08245864
#define rfu_CHILD_getConnectRecoveryStatus sub_082468C8
#define rfu_MBOOT_CHILD_inheritanceLinkStatus sub_08245744
#define rfu_NI_CHILD_setSendGameName sub_08246D30
#define rfu_NI_checkCommFailCounter sub_082481D0
#define rfu_NI_setSendData sub_08246CC8
#define rfu_NI_stopReceivingData sub_082470D4
#define rfu_REQBN_softReset_and_checkID sub_08245890
#define rfu_REQBN_watchLink sub_082461D8
#define rfu_REQ_CHILD_endConnectRecovery sub_082468F8
#define rfu_REQ_CHILD_pollConnectRecovery sub_08246810
#define rfu_REQ_CHILD_startConnectRecovery sub_082467B4
#define rfu_REQ_PARENT_resumeRetransmitAndChange sub_0824555C
#define rfu_REQ_RFUStatus sub_082456F4
#define rfu_REQ_changeMasterSlave sub_0824693C
#define rfu_REQ_configGameData sub_0824596C
#define rfu_REQ_configSystem sub_08245904
#define rfu_REQ_disconnect sub_08246644
#define rfu_REQ_endConnectParent sub_082460C0
#define rfu_REQ_endSearchChild sub_08245B84
#define rfu_REQ_endSearchParent sub_08245DCC
#define rfu_REQ_noise sub_0824826C
#define rfu_REQ_pollConnectParent sub_08245F58
#define rfu_REQ_pollSearchChild sub_08245B70
#define rfu_REQ_pollSearchParent sub_08245D94
#define rfu_REQ_recvData sub_0824780C
#define rfu_REQ_reset sub_082458CC
#define rfu_REQ_sendData sub_0824722C
#define rfu_REQ_startConnectParent sub_08245EF0
#define rfu_REQ_startSearchChild sub_08245A94
#define rfu_REQ_startSearchParent sub_08245D5C
#define rfu_REQ_stopMode sub_082457BC
#define rfu_STC_CHILD_analyzeRecvPacket sub_082479D0
#define rfu_STC_NI_constructLLSF sub_08247578
#define rfu_STC_NI_initSlot_asRecvControllData sub_08248088
#define rfu_STC_NI_initSlot_asRecvDataEntity sub_08248118
#define rfu_STC_NI_receive_Receiver sub_08247F0C
#define rfu_STC_NI_receive_Sender sub_08247D20
#define rfu_STC_PARENT_analyzeRecvPacket sub_08247938
#define rfu_STC_REQ_callback sub_08245630
#define rfu_STC_UNI_constructLLSF sub_08247738
#define rfu_STC_UNI_receive sub_08247C6C
#define rfu_STC_analyzeLLSF sub_08247A28
#define rfu_STC_clearAPIVariables sub_082454D4
#define rfu_STC_clearLinkStatus sub_08245B0C
#define rfu_STC_fastCopy sub_0824690C
#define rfu_STC_readChildList sub_08245C20
#define rfu_STC_readParentCandidateList sub_08245DE0
#define rfu_STC_releaseFrame sub_08246A40
#define rfu_STC_removeLinkData sub_08246594
#define rfu_STC_setSendData_org sub_08246D64
#define rfu_UNI_PARENT_getDRAC_ACK sub_08245570
#define rfu_UNI_changeAndReadySendData sub_0824714C
#define rfu_UNI_clearRecvNewDataFlag sub_0824720C
#define rfu_UNI_readySendData sub_082471E0
#define rfu_UNI_setSendData sub_08246CF4
#define rfu_changeSendTarget sub_08246F6C
#define rfu_clearAllSlot sub_082469A0
#define rfu_clearSlot sub_08246AAC
#define rfu_constructSendLLFrame sub_08247440
#define rfu_enableREQCallback sub_08245604
#define rfu_getConnectParentStatus sub_0824608C
#define rfu_getMasterSlave sub_08246968
#define rfu_getRFUStatus sub_08245708
#define rfu_getSTWIRecvBuffer sub_082455CC
#define rfu_initializeAPI sub_08245398
#define rfu_setMSCCallback sub_082455DC
#define rfu_setREQCallback sub_082455E8
#define rfu_setRecvBuffer sub_08246C68
#define rfu_setTimerInterrupt sub_082455B8
#define rfu_syncVBlank sub_082460F8
#define rfu_waitREQComplete sub_082456E0
#define AgbRFU_checkID sub_08248284
#define Sio32IDInit sub_08248338
#define Sio32IDIntr sub_082484A0
#define Sio32IDMain sub_082483AC
#define IntrSIO32 sub_08244A38
#define gSTWIStatus gUnk_03006A50
#define gRfuLinkStatus gUnk_03006A80
#define gRfuStatic gUnk_03006A84
#define gRfuFixed gUnk_03006A88
#define gRfuSlotStatusNI gUnk_03006A70
#define gRfuSlotStatusUNI gUnk_03006A60
#define gRfuSIO32Id gUnk_03006A90
#define llsf_struct gUnk_08602CD0
#define str_checkMbootLL gUnk_08602CFC
#define Sio32ConnectionData gUnk_08602D08
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;
typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;
typedef float f32;
typedef double f64;
typedef u8 bool8;
typedef u16 bool16;
typedef u32 bool32;
typedef vu8 vbool8;
typedef vu16 vbool16;
typedef vu32 vbool32;
struct BgCnt
{
    u16 priority:2;
    u16 charBaseBlock:2;
    u16 dsCharBaseBlock:2;
    u16 mosaic:1;
    u16 palettes:1;
    u16 screenBaseBlock:5;
    u16 areaOverflowMode:1;
    u16 screenSize:2;
};
typedef volatile struct BgCnt vBgCnt;
struct PlttData
{
    u16 r:5;
    u16 g:5;
    u16 b:5;
    u16 unused_15:1;
};
struct OamData
{
             u32 y:8;
             u32 affineMode:2;
             u32 objMode:2;
             u32 mosaic:1;
             u32 bpp:1;
             u32 shape:2;
             u32 x:9;
             u32 matrixNum:5;
             u32 size:2;
             u16 tileNum:10;
             u16 priority:2;
             u16 paletteNum:4;
             u16 affineParam;
};
struct BgAffineSrcData
{
    s32 texX;
    s32 texY;
    s16 scrX;
    s16 scrY;
    s16 sx;
    s16 sy;
    u16 alpha;
};
struct BgAffineDstData
{
    s16 pa;
    s16 pb;
    s16 pc;
    s16 pd;
    s32 dx;
    s32 dy;
};
struct ObjAffineSrcData
{
    s16 xScale;
    s16 yScale;
    u16 rotation;
};
struct SioMultiCnt
{
    u16 baudRate:2;
    u16 si:1;
    u16 sd:1;
    u16 id:2;
    u16 error:1;
    u16 enable:1;
    u16 unused_11_8:4;
    u16 mode:2;
    u16 intrEnable:1;
    u16 unused_15:1;
    u16 data;
};
asm(".include \"asm/macros.inc\"");
void CpuSet(const void *src, void *dest, u32 control);
void CpuFastSet(const void *src, void *dest, u32 control);
void SoftReset(u32 resetFlags);
void RegisterRamReset(u32 resetFlags);
void VBlankIntrWait(void);
u16 Sqrt(u32 num);
u16 ArcTan2(s16 x, s16 y);
void CpuSet(const void *src, void *dest, u32 control);
void CpuFastSet(const void *src, void *dest, u32 control);
void BgAffineSet(struct BgAffineSrcData *src, struct BgAffineDstData *dest, s32 count);
void ObjAffineSet(struct ObjAffineSrcData *src, void *dest, s32 count, s32 offset);
void LZ77UnCompWram(const u32 *src, void *dest);
void LZ77UnCompVram(const u32 *src, void *dest);
void RLUnCompWram(const u32 *src, void *dest);
void RLUnCompVram(const u32 *src, void *dest);
int MultiBoot(struct MultiBootParam *mp);
s32 Div(s32 num, s32 denom);
typedef unsigned long uintptr_t;
typedef void (*IntrFunc)(void);
struct RfuPacket8
{
    u8 data[0x74];
};
struct RfuPacket32
{
    u32 command;
    u32 data[0x1C];
};
union RfuPacket
{
    struct RfuPacket32 rfuPacket32;
    struct RfuPacket8 rfuPacket8;
};
struct STWIStatus
{
    vs32 state;
    u8 reqLength;
    u8 reqNext;
    u8 reqActiveCommand;
    u8 ackLength;
    u8 ackNext;
    u8 ackActiveCommand;
    u8 timerSelect;
    u8 unk_b;
    u32 timerState;
    vu8 timerActive;
    u8 unk_11;
    vu16 error;
    vu8 msMode;
    u8 recoveryCount;
    u8 unk_16;
    u8 unk_17;
    void (*callbackM)();
    void (*callbackS)(u16);
    void (*callbackID)(void);
    union RfuPacket *txPacket;
    union RfuPacket *rxPacket;
    vu8 sending;
};
struct RfuIntrStruct
{
    union RfuPacket rxPacketAlloc;
    union RfuPacket txPacketAlloc;
    u8 __attribute__((aligned(2))) block1[0x960];
    struct STWIStatus block2;
};
struct UNISend
{
    u16 state;
    u8 dataReadyFlag;
    u8 bmSlot;
    u16 payloadSize;
    const void *src;
};
struct UNIRecv
{
    u16 state;
    u16 errorCode;
    u16 dataSize;
    u8 newDataFlag;
    u8 dataBlockFlag;
};
struct RfuSlotStatusUNI
{
    struct UNISend send;
    struct UNIRecv recv;
    void *recvBuffer;
    u32 recvBufferSize;
};
struct NIComm
{
    u16 state;
    u16 failCounter;
    const u8 *now_p[4];
    u32 remainSize;
    u16 errorCode;
    u8 bmSlot;
    u8 recvAckFlag[4];
    u8 ack;
    u8 phase;
    u8 n[4];
    const void *src;
    u8 bmSlotOrg;
    u8 dataType;
    u16 payloadSize;
    u32 dataSize;
};
struct RfuSlotStatusNI
{
    struct NIComm send;
    struct NIComm recv;
    void *recvBuffer;
    u32 recvBufferSize;
};
struct RfuTgtData
{
    u16 id;
    u8 slot;
    u8 mbootFlag;
    u16 serialNo;
    u8 gname[13 + 2];
    u8 uname[8 + 1];
};
struct RfuLinkStatus
{
    u8 parentChild;
    u8 connCount;
    u8 connSlotFlag;
    u8 linkLossSlotFlag;
    u8 sendSlotNIFlag;
    u8 recvSlotNIFlag;
    u8 sendSlotUNIFlag;
    u8 getNameFlag;
    u8 findParentCount;
    u8 watchInterval;
    u8 strength[4];
    vu8 LLFReadyFlag;
    u8 remainLLFrameSizeParent;
    u8 remainLLFrameSizeChild[4];
    struct RfuTgtData partner[4];
    struct RfuTgtData my;
};
struct RfuFixed
{
    void (*reqCallback)(u16, u16);
    void (*fastCopyPtr)(const u8 **, u8 **, s32);
    u16 fastCopyBuffer[24];
    u32 fastCopyBuffer2[12];
    u32 LLFBuffer[29];
    struct RfuIntrStruct *STWIBuffer;
};
struct RfuStatic
{
    u8 flags;
    u8 NIEndRecvFlag;
    u8 recvRenewalFlag;
    u8 commExistFlag;
    u8 recvErrorFlag;
    u8 recoveryBmSlot;
    u8 nowWatchInterval;
    u8 nullFrameCount;
    u8 emberCount;
    u8 SCStartFlag;
    u8 linkEmergencyFlag[4];
    u8 lsFixedCount[4];
    u16 cidBak[4];
    u16 linkEmergencyLimit;
    u16 reqResult;
    u16 tryPid;
    u16 watchdogTimer;
    u32 totalPacketSize;
};
extern struct STWIStatus *gSTWIStatus;
extern struct RfuLinkStatus *gRfuLinkStatus;
extern struct RfuStatic *gRfuStatic;
extern struct RfuFixed *gRfuFixed;
extern struct RfuSlotStatusNI *gRfuSlotStatusNI[4];
extern struct RfuSlotStatusUNI *gRfuSlotStatusUNI[4];
s32 AgbRFU_checkID(u8 maxTries);
u16 rfu_initializeAPI(u32 *APIBuffer, u16 buffByteSize, IntrFunc *sioIntrTable_p, bool8 copyInterruptToRam);
void rfu_setTimerInterrupt(u8 timerNo, IntrFunc *timerIntrTable_p);
u16 rfu_syncVBlank(void);
void rfu_setREQCallback(void (*callback)(u16 reqCommandId, u16 reqResult));
u16 rfu_waitREQComplete(void);
u32 rfu_REQBN_softReset_and_checkID(void);
void rfu_REQ_reset(void);
void rfu_REQ_stopMode(void);
void rfu_REQ_configSystem(u16 availSlotFlag, u8 maxMFrame, u8 mcTimer);
void rfu_REQ_configGameData(u8 mbootFlag, u16 serialNo, const u8 *gname, const u8 *uname);
void rfu_REQ_startSearchChild(void);
void rfu_REQ_pollSearchChild(void);
void rfu_REQ_endSearchChild(void);
void rfu_REQ_startSearchParent(void);
void rfu_REQ_pollSearchParent(void);
void rfu_REQ_endSearchParent(void);
void rfu_REQ_startConnectParent(u16 pid);
void rfu_REQ_pollConnectParent(void);
void rfu_REQ_endConnectParent(void);
u16 rfu_getConnectParentStatus(u8 *status, u8 *connectSlotNo);
void rfu_REQ_CHILD_startConnectRecovery(u8 bmRecoverySlot);
void rfu_REQ_CHILD_pollConnectRecovery(void);
void rfu_REQ_CHILD_endConnectRecovery(void);
u16 rfu_CHILD_getConnectRecoveryStatus(u8 *status);
u16 rfu_REQBN_watchLink(u16 reqCommandId, u8 *bmLinkLossSlot, u8 *linkLossReason, u8 *parentBmLinkRecoverySlot);
void rfu_REQ_disconnect(u8 bmDisconnectSlot);
void rfu_REQ_changeMasterSlave(void);
bool8 rfu_getMasterSlave(void);
void rfu_setMSCCallback(void (*callback)(u16 reqCommandId));
void rfu_clearAllSlot(void);
u16 rfu_clearSlot(u8 connTypeFlag, u8 slotStatusIndex);
u16 rfu_setRecvBuffer(u8 connType, u8 slotNo, void *buffer, u32 buffSize);
u16 rfu_UNI_setSendData(u8 bmSendSlot, const void *src, u8 size);
void rfu_UNI_readySendData(u8 slotStatusIndex);
u16 rfu_UNI_changeAndReadySendData(u8 slotStatusIndex, const void *src, u8 size);
u16 rfu_UNI_PARENT_getDRAC_ACK(u8 *ackFlag);
void rfu_UNI_clearRecvNewDataFlag(u8 slotStatusIndex);
u16 rfu_NI_setSendData(u8 bmSendSlot, u8 subFrameSize, const void *src, u32 size);
u16 rfu_NI_CHILD_setSendGameName(u8 slotNo, u8 subFrameSize);
u16 rfu_NI_stopReceivingData(u8 slotStatusIndex);
u16 rfu_changeSendTarget(u8 connType, u8 slotStatusIndex, u8 bmNewTgtSlot);
void rfu_REQ_sendData(bool8 clockChangeFlag);
void rfu_REQ_PARENT_resumeRetransmitAndChange(void);
void rfu_REQ_recvData(void);
u16 rfu_MBOOT_CHILD_inheritanceLinkStatus(void);
u8 *rfu_getSTWIRecvBuffer(void);
void rfu_REQ_RFUStatus(void);
u16 rfu_getRFUStatus(u8 *rfuState);
void rfu_REQ_noise(void);
void IntrSIO32(void);
void STWI_init_all(struct RfuIntrStruct *interruptStruct, IntrFunc *interrupt, bool8 copyInterruptToRam);
void STWI_set_MS_mode(u8 mode);
void STWI_init_Callback_M(void);
void STWI_init_Callback_S(void);
void STWI_set_Callback_M(void *callbackM);
void STWI_set_Callback_S(void (*callbackS)(u16));
void STWI_init_timer(IntrFunc *interrupt, s32 timerSelect);
void AgbRFU_SoftReset(void);
void STWI_set_Callback_ID(void (*func)(void));
u16 STWI_read_status(u8 index);
u16 STWI_poll_CommandEnd(void);
void STWI_send_DataRxREQ(void);
void STWI_send_MS_ChangeREQ(void);
void STWI_send_StopModeREQ(void);
void STWI_send_SystemStatusREQ(void);
void STWI_send_GameConfigREQ(const u8 *serial_gname, const u8 *uname);
void STWI_send_ResetREQ(void);
void STWI_send_LinkStatusREQ(void);
void STWI_send_VersionStatusREQ(void);
void STWI_send_SlotStatusREQ(void);
void STWI_send_ConfigStatusREQ(void);
void STWI_send_ResumeRetransmitAndChangeREQ(void);
void STWI_send_SystemConfigREQ(u16 availSlotFlag, u8 maxMFrame, u8 mcTimer);
void STWI_send_SC_StartREQ(void);
void STWI_send_SC_PollingREQ(void);
void STWI_send_SC_EndREQ(void);
void STWI_send_SP_StartREQ(void);
void STWI_send_SP_PollingREQ(void);
void STWI_send_SP_EndREQ(void);
void STWI_send_CP_StartREQ(u16 unk1);
void STWI_send_CP_PollingREQ(void);
void STWI_send_CP_EndREQ(void);
void STWI_send_DataTxREQ(const void *in, u8 size);
void STWI_send_DataTxAndChangeREQ(const void *in, u8 size);
void STWI_send_DataReadyAndChangeREQ(u8 unk);
void STWI_send_DisconnectedAndChangeREQ(u8 unk0, u8 unk1);
void STWI_send_DisconnectREQ(u8 unk);
void STWI_send_TestModeREQ(u8 unk0, u8 unk1);
void STWI_send_CPR_StartREQ(u16 unk0, u16 unk1, u8 unk2);
void STWI_send_CPR_PollingREQ(void);
void STWI_send_CPR_EndREQ(void);

void STWI_intr_timer(void);
u16 STWI_init(u8 request);
s32 STWI_start_Command(void);
void STWI_set_timer(u8 unk);
void STWI_stop_timer(void);
s32 STWI_restart_Command(void);
s32 STWI_reset_ClockCounter(void);
extern struct STWIStatus *gSTWIStatus ;
void STWI_init_all(struct RfuIntrStruct *interruptStruct, IntrFunc *interrupt, bool8 copyInterruptToRam)
{
    if (copyInterruptToRam == 1)
    {
        *interrupt = (IntrFunc)interruptStruct->block1;
        { vu32 *dmaRegs = (vu32 *)(0x4000000 + 0xd4); dmaRegs[0] = (vu32)(&IntrSIO32); dmaRegs[1] = (vu32)(interruptStruct->block1); dmaRegs[2] = (vu32)((0x8000 | 0x0000 | 0x0000 | 0x0000 | 0x0000) << 16 | ((sizeof(interruptStruct->block1))/(16/8))); dmaRegs[2]; };
        gSTWIStatus = &interruptStruct->block2;
    }
    else
    {
        *interrupt = IntrSIO32;
        gSTWIStatus = (struct STWIStatus *)interruptStruct->block1;
    }
    gSTWIStatus->rxPacket = &interruptStruct->rxPacketAlloc;
    gSTWIStatus->txPacket = &interruptStruct->txPacketAlloc;
    gSTWIStatus->msMode = 1;
    gSTWIStatus->state = 0;
    gSTWIStatus->reqLength = 0;
    gSTWIStatus->reqNext = 0;
    gSTWIStatus->ackLength = 0;
    gSTWIStatus->ackNext = 0;
    gSTWIStatus->ackActiveCommand = 0;
    gSTWIStatus->timerState = 0;
    gSTWIStatus->timerActive = 0;
    gSTWIStatus->error = 0;
    gSTWIStatus->recoveryCount = 0;
    gSTWIStatus->sending = 0;
    (*(vu16 *)(0x4000000 + 0x134)) = 0x100;
    (*(vu16 *)(0x4000000 + 0x128)) = 0x4000 | 0x1000 | 0x0003;
    STWI_init_Callback_M();
    STWI_init_Callback_S();
    { u16 imeTemp; imeTemp = (*(vu16 *)(0x4000000 + 0x208)); (*(vu16 *)(0x4000000 + 0x208)) = 0; (*(vu16 *)(0x4000000 + 0x200)) |= (1 << 7); (*(vu16 *)(0x4000000 + 0x208)) = imeTemp; };
}
void STWI_init_timer(IntrFunc *interrupt, s32 timerSelect)
{
    *interrupt = STWI_intr_timer;
    gSTWIStatus->timerSelect = timerSelect;
    { u16 imeTemp; imeTemp = (*(vu16 *)(0x4000000 + 0x208)); (*(vu16 *)(0x4000000 + 0x208)) = 0; (*(vu16 *)(0x4000000 + 0x200)) |= (1 << 3) << gSTWIStatus->timerSelect; (*(vu16 *)(0x4000000 + 0x208)) = imeTemp; };
}
void AgbRFU_SoftReset(void)
{
    vu16 *timerL;
    vu16 *timerH;
    (*(vu16 *)(0x4000000 + 0x134)) = 0x8000;
    (*(vu16 *)(0x4000000 + 0x134)) = 0x80A0;
    timerL = &(*(vu16 *)((0x4000000 + 0x100) + ((gSTWIStatus->timerSelect) * 4)));
    timerH = &(*(vu16 *)((0x4000000 + 0x102) + ((gSTWIStatus->timerSelect) * 4)));
    *timerH = 0;
    *timerL = 0;
    *timerH = 0x80 | 0x03;
    while (*timerL <= 0x11)
        (*(vu16 *)(0x4000000 + 0x134)) = 0x80A2;
    *timerH = 3;
    (*(vu16 *)(0x4000000 + 0x134)) = 0x80A0;
    (*(vu16 *)(0x4000000 + 0x128)) = 0x4000 | 0x1000 | 0x0003;
    gSTWIStatus->state = 0;
    gSTWIStatus->reqLength = 0;
    gSTWIStatus->reqNext = 0;
    gSTWIStatus->reqActiveCommand = 0;
    gSTWIStatus->ackLength = 0;
    gSTWIStatus->ackNext = 0;
    gSTWIStatus->ackActiveCommand = 0;
    gSTWIStatus->timerState = 0;
    gSTWIStatus->timerActive = 0;
    gSTWIStatus->error = 0;
    gSTWIStatus->msMode = 1;
    gSTWIStatus->recoveryCount = 0;
    gSTWIStatus->sending = 0;
}
void STWI_set_MS_mode(u8 mode)
{
    gSTWIStatus->msMode = mode;
}
u16 STWI_read_status(u8 index)
{
    switch (index)
    {
    case 0:
        return gSTWIStatus->error;
    case 1:
        return gSTWIStatus->msMode;
    case 2:
        return gSTWIStatus->state;
    case 3:
        return gSTWIStatus->reqActiveCommand;
    default:
        return 0xFFFF;
    }
}
void STWI_init_Callback_M(void)
{
    STWI_set_Callback_M(((void *)0));
}
void STWI_init_Callback_S(void)
{
    STWI_set_Callback_S(((void *)0));
}
void STWI_set_Callback_M(void *callbackM)
{
    gSTWIStatus->callbackM = callbackM;
}
void STWI_set_Callback_S(void (*callbackS)(u16))
{
    gSTWIStatus->callbackS = callbackS;
}
void STWI_set_Callback_ID(void (*func)(void))
{
    gSTWIStatus->callbackID = func;
}
u16 STWI_poll_CommandEnd(void)
{
    while (gSTWIStatus->sending == 1)
        ;
    return gSTWIStatus->error;
}
void STWI_send_ResetREQ(void)
{
    if (!STWI_init(0x0010))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_LinkStatusREQ(void)
{
    if (!STWI_init(0x0011))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_VersionStatusREQ(void)
{
    if (!STWI_init(0x0012))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_SystemStatusREQ(void)
{
    if (!STWI_init(0x0013))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_SlotStatusREQ(void)
{
    if (!STWI_init(0x0014))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_ConfigStatusREQ(void)
{
    if (!STWI_init(0x0015))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_GameConfigREQ(const u8 *serial_gname, const u8 *uname)
{
    u8 *packetBytes;
    s32 i;
    if (!STWI_init(0x0016))
    {
        gSTWIStatus->reqLength = 6;
        packetBytes = gSTWIStatus->txPacket->rfuPacket8.data;
        packetBytes += sizeof(u32);
        *(u16 *)packetBytes = *(u16 *)serial_gname;
        packetBytes += sizeof(u16);
        serial_gname += sizeof(u16);
        for (i = 0; i < 14; ++i)
        {
            *packetBytes = *serial_gname;
            ++packetBytes;
            ++serial_gname;
        }
        for (i = 0; i < 8; ++i)
        {
            *packetBytes = *uname;
            ++packetBytes;
            ++uname;
        }
        STWI_start_Command();
    }
}
void STWI_send_SystemConfigREQ(u16 availSlotFlag, u8 maxMFrame, u8 mcTimer)
{
    if (!STWI_init(0x0017))
    {
        u8 *packetBytes;
        gSTWIStatus->reqLength = 1;
        packetBytes = gSTWIStatus->txPacket->rfuPacket8.data;
        packetBytes += sizeof(u32);
        *packetBytes++ = mcTimer;
        *packetBytes++ = maxMFrame;
        *(u16 *)packetBytes = availSlotFlag;
        STWI_start_Command();
    }
}
void STWI_send_SC_StartREQ(void)
{
    if (!STWI_init(0x0019))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_SC_PollingREQ(void)
{
    if (!STWI_init(0x001a))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_SC_EndREQ(void)
{
    if (!STWI_init(0x001b))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_SP_StartREQ(void)
{
    if (!STWI_init(0x001c))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_SP_PollingREQ(void)
{
    if (!STWI_init(0x001d))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_SP_EndREQ(void)
{
    if (!STWI_init(0x001e))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_CP_StartREQ(u16 unk1)
{
    if (!STWI_init(0x001f))
    {
        gSTWIStatus->reqLength = 1;
        gSTWIStatus->txPacket->rfuPacket32.data[0] = unk1;
        STWI_start_Command();
    }
}
void STWI_send_CP_PollingREQ(void)
{
    if (!STWI_init(0x0020))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_CP_EndREQ(void)
{
    if (!STWI_init(0x0021))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_DataTxREQ(const void *in, u8 size)
{
    if (!STWI_init(0x0024))
    {
        u8 reqLength = (size / sizeof(u32));
        if (size & (sizeof(u32) - 1))
            reqLength += 1;
        gSTWIStatus->reqLength = reqLength;
        CpuSet(in, gSTWIStatus->txPacket->rfuPacket32.data, 0x04000000 | ((gSTWIStatus->reqLength * sizeof(u32))/(32/8) & 0x1FFFFF));
        STWI_start_Command();
    }
}
void STWI_send_DataTxAndChangeREQ(const void *in, u8 size)
{
    if (!STWI_init(0x0025))
    {
        u8 reqLength = (size / sizeof(u32));
        if (size & (sizeof(u32) - 1))
            reqLength += 1;
        gSTWIStatus->reqLength = reqLength;
        CpuSet(in, gSTWIStatus->txPacket->rfuPacket32.data, 0x04000000 | ((gSTWIStatus->reqLength * sizeof(u32))/(32/8) & 0x1FFFFF));
        STWI_start_Command();
    }
}
void STWI_send_DataRxREQ(void)
{
    if (!STWI_init(0x0026))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_MS_ChangeREQ(void)
{
    if (!STWI_init(0x0027))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_DataReadyAndChangeREQ(u8 unk)
{
    if (!STWI_init(0x0028))
    {
        if (!unk)
        {
            gSTWIStatus->reqLength = 0;
        }
        else
        {
            u8 *packetBytes;
            gSTWIStatus->reqLength = 1;
            packetBytes = gSTWIStatus->txPacket->rfuPacket8.data;
            packetBytes += sizeof(u32);
            *packetBytes++ = unk;
            *packetBytes++ = 0;
            *packetBytes++ = 0;
            *packetBytes = 0;
        }
        STWI_start_Command();
    }
}
void STWI_send_DisconnectedAndChangeREQ(u8 unk0, u8 unk1)
{
    if (!STWI_init(0x0029))
    {
        u8 *packetBytes;
        gSTWIStatus->reqLength = 1;
        packetBytes = gSTWIStatus->txPacket->rfuPacket8.data;
        packetBytes += sizeof(u32);
        *packetBytes++ = unk0;
        *packetBytes++ = unk1;
        *packetBytes++ = 0;
        *packetBytes = 0;
        STWI_start_Command();
    }
}
void STWI_send_ResumeRetransmitAndChangeREQ(void)
{
    if (!STWI_init(0x0037))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_DisconnectREQ(u8 unk)
{
    if (!STWI_init(0x0030))
    {
        gSTWIStatus->reqLength = 1;
        gSTWIStatus->txPacket->rfuPacket32.data[0] = unk;
        STWI_start_Command();
    }
}
void STWI_send_TestModeREQ(u8 unk0, u8 unk1)
{
    if (!STWI_init(0x0031))
    {
        gSTWIStatus->reqLength = 1;
        gSTWIStatus->txPacket->rfuPacket32.data[0] = unk0 | (unk1 << 8);
        STWI_start_Command();
    }
}
void STWI_send_CPR_StartREQ(u16 unk0, u16 unk1, u8 unk2)
{
    u32 *packetData;
    u32 arg1;
    if (!STWI_init(0x0032))
    {
        gSTWIStatus->reqLength = 2;
        arg1 = unk1 | (unk0 << 16);
        packetData = gSTWIStatus->txPacket->rfuPacket32.data;
        packetData[0] = arg1;
        packetData[1] = unk2;
        STWI_start_Command();
    }
}
void STWI_send_CPR_PollingREQ(void)
{
    if (!STWI_init(0x0033))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_CPR_EndREQ(void)
{
    if (!STWI_init(0x0034))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_send_StopModeREQ(void)
{
    if (!STWI_init(0x003d))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}
void STWI_intr_timer(void)
{
    switch (gSTWIStatus->timerState)
    {
    case 2:
        gSTWIStatus->timerActive = 1;
        STWI_set_timer(50);
        break;
    case 1:
    case 4:
        STWI_stop_timer();
        STWI_restart_Command();
        break;
    case 3:
        gSTWIStatus->timerActive = 1;
        STWI_stop_timer();
        STWI_reset_ClockCounter();
        if (gSTWIStatus->callbackM != ((void *)0))
            gSTWIStatus->callbackM(0x00ff, 0);
        break;
    }
}
void STWI_set_timer(u8 count)
{
    vu16 *timerL = &(*(vu16 *)((0x4000000 + 0x100) + ((gSTWIStatus->timerSelect) * 4)));
    vu16 *timerH = &(*(vu16 *)((0x4000000 + 0x102) + ((gSTWIStatus->timerSelect) * 4)));
    (*(vu16 *)(0x4000000 + 0x208)) = 0;
    switch (count)
    {
    case 50:
        *timerL = 0xFCCB;
        gSTWIStatus->timerState = 1;
        break;
    case 80:
        *timerL = 0xFAE0;
        gSTWIStatus->timerState = 2;
        break;
    case 100:
        *timerL = 0xF996;
        gSTWIStatus->timerState = 3;
        break;
    case 130:
        *timerL = 0xF7AD;
        gSTWIStatus->timerState = 4;
        break;
    }
    *timerH = 0x80 | 0x40 | 0x03;
    (*(vu16 *)(0x4000000 + 0x202)) = (1 << 3) << gSTWIStatus->timerSelect;
    (*(vu16 *)(0x4000000 + 0x208)) = 1;
}
void STWI_stop_timer(void)
{
    gSTWIStatus->timerState = 0;
    (*(vu16 *)((0x4000000 + 0x100) + ((gSTWIStatus->timerSelect) * 4))) = 0;
    (*(vu16 *)((0x4000000 + 0x102) + ((gSTWIStatus->timerSelect) * 4))) = 0;
}
u16 STWI_init(u8 request)
{
    if (!(*(vu16 *)(0x4000000 + 0x208)))
    {
        gSTWIStatus->error = (0x0000 | 0x0006);
        if (gSTWIStatus->callbackM != ((void *)0))
            gSTWIStatus->callbackM(request, gSTWIStatus->error);
        return 1;
    }
    else if (gSTWIStatus->sending == 1)
    {
        gSTWIStatus->error = (0x0000 | 0x0002);
        gSTWIStatus->sending = 0;
        if (gSTWIStatus->callbackM != ((void *)0))
            gSTWIStatus->callbackM(request, gSTWIStatus->error);
        return 1;
    }
    else if (gSTWIStatus->msMode == 0)
    {
        gSTWIStatus->error = (0x0000 | 0x0004);
        if (gSTWIStatus->callbackM != ((void *)0))
            gSTWIStatus->callbackM(request, gSTWIStatus->error, gSTWIStatus);
        return 1;
    }
    else
    {
        gSTWIStatus->sending = 1;
        gSTWIStatus->reqActiveCommand = request;
        gSTWIStatus->state = 0;
        gSTWIStatus->reqLength = 0;
        gSTWIStatus->reqNext = 0;
        gSTWIStatus->ackLength = 0;
        gSTWIStatus->ackNext = 0;
        gSTWIStatus->ackActiveCommand = 0;
        gSTWIStatus->timerState = 0;
        gSTWIStatus->timerActive = 0;
        gSTWIStatus->error = 0;
        gSTWIStatus->recoveryCount = 0;
        (*(vu16 *)(0x4000000 + 0x134)) = 0x100;
        (*(vu16 *)(0x4000000 + 0x128)) = 0x4000 | 0x1000 | 0x0003;
        return 0;
    }
}
s32 STWI_start_Command(void)
{
    u16 imeTemp;
    *(u32 *)gSTWIStatus->txPacket->rfuPacket8.data = 0x99660000 | (gSTWIStatus->reqLength << 8) | gSTWIStatus->reqActiveCommand;
    (*(vu32 *)(0x4000000 + 0x120)) = gSTWIStatus->txPacket->rfuPacket32.command;
    gSTWIStatus->state = 0;
    gSTWIStatus->reqNext = 1;
    imeTemp = (*(vu16 *)(0x4000000 + 0x208));
    (*(vu16 *)(0x4000000 + 0x208)) = 0;
    (*(vu16 *)(0x4000000 + 0x200)) |= ((1 << 3) << gSTWIStatus->timerSelect);
    (*(vu16 *)(0x4000000 + 0x200)) |= (1 << 7);
    (*(vu16 *)(0x4000000 + 0x208)) = imeTemp;
    (*(vu16 *)(0x4000000 + 0x128)) = 0x4000 | 0x1000 | 0x0080 | 0x0003;
    return 0;
}
s32 STWI_restart_Command(void)
{
    if (gSTWIStatus->recoveryCount < 2)
    {
        ++gSTWIStatus->recoveryCount;
        STWI_start_Command();
    }
    else
    {
        if (gSTWIStatus->reqActiveCommand == 0x0027 || gSTWIStatus->reqActiveCommand == 0x0025 || gSTWIStatus->reqActiveCommand == 0x0035 || gSTWIStatus->reqActiveCommand == 0x0037)
        {
            gSTWIStatus->error = (0x0000 | 0x0001);
            gSTWIStatus->sending = 0;
            if (gSTWIStatus->callbackM != ((void *)0))
                gSTWIStatus->callbackM(gSTWIStatus->reqActiveCommand, gSTWIStatus->error);
        }
        else
        {
            gSTWIStatus->error = (0x0000 | 0x0001);
            gSTWIStatus->sending = 0;
            if (gSTWIStatus->callbackM != ((void *)0))
                gSTWIStatus->callbackM(gSTWIStatus->reqActiveCommand, gSTWIStatus->error);
            gSTWIStatus->state = 4;
        }
    }
    return 0;
}
s32 STWI_reset_ClockCounter(void)
{
    gSTWIStatus->state = 5;
    gSTWIStatus->reqLength = 0;
    gSTWIStatus->reqNext = 0;
    (*(vu32 *)(0x4000000 + 0x120)) = (1 << 31);
    (*(vu16 *)(0x4000000 + 0x128)) = 0;
    (*(vu16 *)(0x4000000 + 0x128)) = 0x4000 | 0x1000 | 0x0003;
    (*(vu16 *)(0x4000000 + 0x128)) = (0x4000 | 0x1000 | 0x0003) + 0x7F;
    return 0;
}
