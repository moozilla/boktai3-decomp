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
struct LLSFStruct
{
    u8 frameSize;
    u8 recvFirstShift;
    u8 connSlotFlagShift;
    u8 slotStateShift;
    u8 ackShift;
    u8 phaseShift;
    u8 nShift;
    u8 recvFirstMask;
    u8 connSlotFlagMask;
    u8 slotStateMask;
    u8 ackMask;
    u8 phaseMask;
    u8 nMask;
    u16 framesMask;
};
struct RfuLocalStruct
{
    u8 recvFirst;
    u8 connSlotFlag;
    u8 slotState;
    u8 ack;
    u8 phase;
    u8 n;
    u16 frame;
};
void rfu_CB_defaultCallback(u8 reqCommand, u16 reqResult);
void rfu_CB_reset(u8 reqCommand, u16 reqResult);
void rfu_CB_configGameData(u8 reqCommand, u16 reqResult);
void rfu_CB_stopMode(u8 reqCommand, u16 reqResult);
void rfu_CB_startSearchChild(u8 reqCommand, u16 reqResult);
void rfu_CB_pollAndEndSearchChild(u8 reqCommand, u16 reqResult);
void rfu_CB_startSearchParent(u8 reqCommand, u16 reqResult);
void rfu_CB_pollSearchParent(u8 reqCommand, u16 reqResult);
void rfu_CB_pollConnectParent(u8 reqCommand, u16 reqResult);
void rfu_CB_pollConnectParent(u8 reqCommand, u16 reqResult);
void rfu_CB_disconnect(u8 reqCommand, u16 reqResult);
void rfu_CB_CHILD_pollConnectRecovery(u8 reqCommand, u16 reqResult);
void rfu_CB_sendData(__attribute__((unused)) u8 reqCommand, u16 reqResult);
void rfu_CB_sendData2(__attribute__((unused)) u8 reqCommand, u16 reqResult);
void rfu_CB_sendData3(u8 reqCommand, u16 reqResult);
void rfu_CB_recvData(u8 reqCommand, u16 reqResult);
void rfu_enableREQCallback(bool8 enable);
void rfu_STC_clearAPIVariables(void);
void rfu_STC_readChildList(void);
void rfu_STC_readParentCandidateList(void);
void rfu_STC_REQ_callback(u8 reqCommand, u16 reqResult);
void rfu_STC_removeLinkData(u8, u8);
void rfu_STC_fastCopy(const u8 **, u8 **, s32);
void rfu_STC_clearLinkStatus(u8);
void rfu_NI_checkCommFailCounter(void);
u16 rfu_STC_setSendData_org(u8, u8, u8, const void *, u32);
void rfu_constructSendLLFrame(void);
u16 rfu_STC_NI_constructLLSF(u8, u8 **, struct NIComm *);
u16 rfu_STC_UNI_constructLLSF(u8, u8 **);
void rfu_STC_PARENT_analyzeRecvPacket(void);
void rfu_STC_CHILD_analyzeRecvPacket(void);
u16 rfu_STC_analyzeLLSF(u8, const u8 *, u16);
void rfu_STC_UNI_receive(u8, const struct RfuLocalStruct *, const u8 *);
void rfu_STC_NI_receive_Receiver(u8, const struct RfuLocalStruct *, const u8 *);
void rfu_STC_NI_receive_Sender(u8, u8, const struct RfuLocalStruct *, __attribute__((unused)) const u8 *);
void rfu_STC_NI_initSlot_asRecvDataEntity(u8, struct NIComm *);
void rfu_STC_NI_initSlot_asRecvControllData(u8, struct NIComm *);
extern struct RfuSlotStatusUNI *gRfuSlotStatusUNI[4] ;
extern struct RfuSlotStatusNI *gRfuSlotStatusNI[4] ;
extern struct RfuLinkStatus *gRfuLinkStatus ;
extern struct RfuStatic *gRfuStatic ;
extern struct RfuFixed *gRfuFixed ;
extern const struct LLSFStruct llsf_struct[2];
extern const char version_string[];
extern const char str_checkMbootLL[];
u16 rfu_initializeAPI(u32 *APIBuffer, u16 buffByteSize, IntrFunc *sioIntrTable_p, bool8 copyInterruptToRam)
{
    u16 i;
    u16 *dst;
    const u16 *src;
    u16 buffByteSizeMax;
    if (((uintptr_t)APIBuffer & 0xF000000) == 0x02000000 && copyInterruptToRam)
        return 0x0002;
    if ((u32)APIBuffer & 3)
        return 0x0002;
    if (copyInterruptToRam)
    {
        buffByteSizeMax = 0x0e64;
        if (buffByteSize < buffByteSizeMax)
            return 0x0001;
    }
    if (!copyInterruptToRam)
    {
        buffByteSizeMax = 0x0504;
        if (buffByteSize < buffByteSizeMax)
            return 0x0001;
    }
    gRfuLinkStatus = (void *)APIBuffer + 0;
    gRfuStatic = (void *)APIBuffer + 0xb4;
    gRfuFixed = (void *)APIBuffer + 0xdc;
    gRfuSlotStatusNI[0] = (void *)APIBuffer + 0x1bc;
    gRfuSlotStatusUNI[0] = (void *)APIBuffer + 0x37c;
    for (i = 1; i < 4; ++i)
    {
        gRfuSlotStatusNI[i] = &gRfuSlotStatusNI[i - 1][1];
        gRfuSlotStatusUNI[i] = &gRfuSlotStatusUNI[i - 1][1];
    }
    gRfuFixed->STWIBuffer = (struct RfuIntrStruct *)&gRfuSlotStatusUNI[3][1];
    STWI_init_all((struct RfuIntrStruct *)&gRfuSlotStatusUNI[3][1], sioIntrTable_p, copyInterruptToRam);
    rfu_STC_clearAPIVariables();
    for (i = 0; i < 4; ++i)
    {
        gRfuSlotStatusNI[i]->recvBuffer = ((void *)0);
        gRfuSlotStatusNI[i]->recvBufferSize = 0;
        gRfuSlotStatusUNI[i]->recvBuffer = ((void *)0);
        gRfuSlotStatusUNI[i]->recvBufferSize = 0;
    }
    src = (const u16 *)((uintptr_t)&rfu_STC_fastCopy & ~1);
    dst = gRfuFixed->fastCopyBuffer;
    buffByteSizeMax = ((void *)rfu_REQ_changeMasterSlave - (void *)rfu_STC_fastCopy) / sizeof(u16);
    while (buffByteSizeMax-- != 0)
        *dst++ = *src++;
    gRfuFixed->fastCopyPtr = (void *)gRfuFixed->fastCopyBuffer + 1;
    return 0;
}
void rfu_STC_clearAPIVariables(void)
{
    u16 IMEBackup = (*(vu16 *)(0x4000000 + 0x208));
    u8 i, flags;
    (*(vu16 *)(0x4000000 + 0x208)) = 0;
    flags = gRfuStatic->flags;
    { vu16 tmp = (vu16)(0); CpuSet((void *)&tmp, gRfuStatic, 0x00000000 | 0x01000000 | ((sizeof(struct RfuStatic))/(16/8) & 0x1FFFFF)); };
    gRfuStatic->flags = flags & 8;
    { vu16 tmp = (vu16)(0); CpuSet((void *)&tmp, gRfuLinkStatus, 0x00000000 | 0x01000000 | ((sizeof(struct RfuLinkStatus))/(16/8) & 0x1FFFFF)); };
    gRfuLinkStatus->watchInterval = 4;
    gRfuStatic->nowWatchInterval = 0;
    gRfuLinkStatus->parentChild = 0xff;
    rfu_clearAllSlot();
    gRfuStatic->SCStartFlag = 0;
    for (i = 0; i < 4; ++i)
        gRfuStatic->cidBak[i] = 0;
    (*(vu16 *)(0x4000000 + 0x208)) = IMEBackup;
}
void rfu_REQ_PARENT_resumeRetransmitAndChange(void)
{
    STWI_set_Callback_M(rfu_STC_REQ_callback);
    STWI_send_ResumeRetransmitAndChangeREQ();
}
u16 rfu_UNI_PARENT_getDRAC_ACK(u8 *ackFlag)
{
    u8 *buf;
    *ackFlag = 0;
    if (gRfuLinkStatus->parentChild != 0x01)
        return (0x0300 | 0x0000);
    buf = rfu_getSTWIRecvBuffer();
    switch (*buf)
    {
    case 40:
    case 54:
        if (buf[1] == 0)
            *ackFlag = gRfuLinkStatus->connSlotFlag;
        else
            *ackFlag = buf[4];
        return 0;
    default:
        return (0x0000 | 0x0010);
    }
}
void rfu_setTimerInterrupt(u8 timerNo, IntrFunc *timerIntrTable_p)
{
    STWI_init_timer(timerIntrTable_p, timerNo);
}
u8 *rfu_getSTWIRecvBuffer(void)
{
    return (u8 *)gRfuFixed->STWIBuffer;
}
void rfu_setMSCCallback(void (*callback)(u16 reqCommandId))
{
    STWI_set_Callback_S(callback);
}
void rfu_setREQCallback(void (*callback)(u16 reqCommandId, u16 reqResult))
{
    gRfuFixed->reqCallback = callback;
    rfu_enableREQCallback(callback != ((void *)0));
}
void rfu_enableREQCallback(bool8 enable)
{
    if (enable)
        gRfuStatic->flags |= 8;
    else
        gRfuStatic->flags &= 0xF7;
}
void rfu_STC_REQ_callback(u8 reqCommand, u16 reqResult)
{
    STWI_set_Callback_M(rfu_CB_defaultCallback);
    gRfuStatic->reqResult = reqResult;
    if (gRfuStatic->flags & 8)
        gRfuFixed->reqCallback(reqCommand, reqResult);
}
void rfu_CB_defaultCallback(u8 reqCommand, u16 reqResult)
{
    s32 bmSlotFlags;
    u8 i;
    if (reqCommand == 0x00ff)
    {
        if (gRfuStatic->flags & 8)
            gRfuFixed->reqCallback(reqCommand, reqResult);
        bmSlotFlags = gRfuLinkStatus->connSlotFlag | gRfuLinkStatus->linkLossSlotFlag;
        for (i = 0; i < 4; ++i)
            if ((bmSlotFlags >> i) & 1)
                rfu_STC_removeLinkData(i, 1);
        gRfuLinkStatus->parentChild = 0xff;
    }
}
u16 rfu_waitREQComplete(void)
{
    STWI_poll_CommandEnd();
    return gRfuStatic->reqResult;
}
void rfu_REQ_RFUStatus(void)
{
    STWI_set_Callback_M(rfu_STC_REQ_callback);
    STWI_send_SystemStatusREQ();
}
u16 rfu_getRFUStatus(u8 *rfuState)
{
    if (gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[0] != 0x93)
        return (0x0000 | 0x0010);
    if (STWI_poll_CommandEnd() == 0)
        *rfuState = gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[7];
    else
        *rfuState = 0xFF;
    return 0;
}
u16 rfu_MBOOT_CHILD_inheritanceLinkStatus(void)
{
    const char *s1 = str_checkMbootLL;
    char *s2 = (char *)(0x03000000 + 0xF0);
    u16 checksum;
    u16 *mb_buff_iwram_p;
    u8 i;
    while (*s1 != '\0')
        if (*s1++ != *s2++)
            return 1;
    mb_buff_iwram_p = (u16 *)0x03000000;
    checksum = 0;
    for (i = 0; i < 180/2; ++i)
        checksum += *mb_buff_iwram_p++;
    if (checksum != *(u16 *)(0x03000000 + 0xFA))
        return 1;
    CpuSet((u16 *)0x03000000, gRfuLinkStatus, 0x00000000 | ((sizeof(struct RfuLinkStatus))/(16/8) & 0x1FFFFF));
    gRfuStatic->flags |= 0x80;
    return 0;
}
void rfu_REQ_stopMode(void)
{
    vu32 *timerReg;
    if ((*(vu16 *)(0x4000000 + 0x208)) == 0)
    {
        rfu_STC_REQ_callback(0x003d, 6);
        gSTWIStatus->error = (0x0000 | 0x0006);
    }
    else
    {
        AgbRFU_SoftReset();
        rfu_STC_clearAPIVariables();
        if (AgbRFU_checkID(8) == 0x00008001)
        {
            timerReg = &(*(vu32 *)((0x4000000 + 0x100) + ((gSTWIStatus->timerSelect) * 4)));
            *timerReg = 0;
            *timerReg = (0x80 | 0x03) << 16;
            while (*timerReg << 16 < 262 << 16)
                ;
            *timerReg = 0;
            STWI_set_Callback_M(rfu_CB_stopMode);
            STWI_send_StopModeREQ();
        }
        else
        {
            (*(vu16 *)(0x4000000 + 0x128)) = 0x2000;
            rfu_STC_REQ_callback(0x003d, 0);
        }
    }
}
void rfu_CB_stopMode(u8 reqCommand, u16 reqResult)
{
    if (reqResult == 0)
        (*(vu16 *)(0x4000000 + 0x128)) = 0x2000;
    rfu_STC_REQ_callback(reqCommand, reqResult);
}
u32 rfu_REQBN_softReset_and_checkID(void)
{
    u32 id;
    if ((*(vu16 *)(0x4000000 + 0x208)) == 0)
        return 0xffffffff;
    AgbRFU_SoftReset();
    rfu_STC_clearAPIVariables();
    if ((id = AgbRFU_checkID(30)) == 0)
        (*(vu16 *)(0x4000000 + 0x128)) = 0x2000;
    return id;
}
void rfu_REQ_reset(void)
{
    STWI_set_Callback_M(rfu_CB_reset);
    STWI_send_ResetREQ();
}
void rfu_CB_reset(u8 reqCommand, u16 reqResult)
{
    if (reqResult == 0)
        rfu_STC_clearAPIVariables();
    rfu_STC_REQ_callback(reqCommand, reqResult);
}
void rfu_REQ_configSystem(u16 availSlotFlag, u8 maxMFrame, u8 mcTimer)
{
    STWI_set_Callback_M(rfu_STC_REQ_callback);
    STWI_send_SystemConfigREQ((availSlotFlag & 0x0003) | 0x3C, maxMFrame, mcTimer);
    if (mcTimer == 0)
    {
        gRfuStatic->linkEmergencyLimit = 1;
    }
    else
    {
        u16 IMEBackup = (*(vu16 *)(0x4000000 + 0x208));
        (*(vu16 *)(0x4000000 + 0x208)) = 0;
        gRfuStatic->linkEmergencyLimit = Div(600, mcTimer);
        (*(vu16 *)(0x4000000 + 0x208)) = IMEBackup;
    }
}
void rfu_REQ_configGameData(u8 mbootFlag, u16 serialNo, const u8 *gname, const u8 *uname)
{
    u8 packet[16];
    u8 i;
    u8 check_sum;
    const u8 *gnameBackup = gname;
    const u8 *unameBackup;
    packet[0] = serialNo;
    packet[1] = serialNo >> 8;
    if (mbootFlag != 0)
        packet[1] = (serialNo >> 8) | 0x80;
    for (i = 2; i < 15; ++i)
        packet[i] = *gname++;
    check_sum = 0;
    unameBackup = uname;
    for (i = 0; i < 8; ++i)
    {
        check_sum += *unameBackup++;
        check_sum += *gnameBackup++;
    }
    packet[15] = ~check_sum;
    if (mbootFlag != 0)
        packet[14] = 0;
    STWI_set_Callback_M(rfu_CB_configGameData);
    STWI_send_GameConfigREQ(packet, uname);
}
void rfu_CB_configGameData(u8 reqCommand, u16 reqResult)
{
    s32 serialNo;
    u8 *gname_uname_p;
    u8 i;
    u8 *packet_p;
    if (reqResult == 0)
    {
        packet_p = gSTWIStatus->txPacket->rfuPacket8.data;
        serialNo = gRfuLinkStatus->my.serialNo = packet_p[4];
        gRfuLinkStatus->my.serialNo = (packet_p[5] << 8) | serialNo;
        gname_uname_p = &packet_p[6];
        if (gRfuLinkStatus->my.serialNo & 0x8000)
        {
            gRfuLinkStatus->my.serialNo = gRfuLinkStatus->my.serialNo ^ 0x8000;
            gRfuLinkStatus->my.mbootFlag = 1;
        }
        else
        {
            gRfuLinkStatus->my.mbootFlag = 0;
        }
        for (i = 0; i < 13; ++i)
            gRfuLinkStatus->my.gname[i] = *gname_uname_p++;
        ++gname_uname_p;
        for (i = 0; i < 7 + 1; ++i)
            gRfuLinkStatus->my.uname[i] = *gname_uname_p++;
    }
    rfu_STC_REQ_callback(reqCommand, reqResult);
}
void rfu_REQ_startSearchChild(void)
{
    u16 result;
    STWI_set_Callback_M(rfu_CB_defaultCallback);
    STWI_send_SystemStatusREQ();
    result = STWI_poll_CommandEnd();
    if (result == 0)
    {
        if (gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[7] == 0)
            rfu_STC_clearLinkStatus(0x01);
    }
    else
    {
        rfu_STC_REQ_callback(0x0019, result);
    }
    STWI_set_Callback_M(rfu_CB_startSearchChild);
    STWI_send_SC_StartREQ();
}
void rfu_CB_startSearchChild(u8 reqCommand, u16 reqResult)
{
    if (reqResult == 0)
        gRfuStatic->SCStartFlag = 1;
    rfu_STC_REQ_callback(reqCommand, reqResult);
}
void rfu_STC_clearLinkStatus(u8 parentChild)
{
    u8 i;
    rfu_clearAllSlot();
    if (parentChild != 0x00)
    {
        { vu16 tmp = (vu16)(0); CpuSet((void *)&tmp, gRfuLinkStatus->partner, 0x00000000 | 0x01000000 | ((sizeof(gRfuLinkStatus->partner))/(16/8) & 0x1FFFFF)); };
        gRfuLinkStatus->findParentCount = 0;
    }
    for (i = 0; i < 4; ++i)
        gRfuLinkStatus->strength[i] = 0;
    gRfuLinkStatus->connCount = 0;
    gRfuLinkStatus->connSlotFlag = 0;
    gRfuLinkStatus->linkLossSlotFlag = 0;
    gRfuLinkStatus->getNameFlag = 0;
}
void rfu_REQ_pollSearchChild(void)
{
    STWI_set_Callback_M(rfu_CB_pollAndEndSearchChild);
    STWI_send_SC_PollingREQ();
}
void rfu_REQ_endSearchChild(void)
{
    STWI_set_Callback_M(rfu_CB_pollAndEndSearchChild);
    STWI_send_SC_EndREQ();
}
void rfu_CB_pollAndEndSearchChild(u8 reqCommand, u16 reqResult)
{
    if (reqResult == 0)
        rfu_STC_readChildList();
    if (reqCommand == 0x001a)
    {
        if (gRfuLinkStatus->my.id == 0)
        {
            STWI_set_Callback_M(rfu_CB_defaultCallback);
            STWI_send_SystemStatusREQ();
            if (STWI_poll_CommandEnd() == 0)
                gRfuLinkStatus->my.id = *(u16 *)&gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket32.data[0];
        }
    }
    else if (reqCommand == 0x001b)
    {
        if (gRfuLinkStatus->parentChild == 0xff)
            gRfuLinkStatus->my.id = 0;
        gRfuStatic->SCStartFlag = 0;
    }
    rfu_STC_REQ_callback(reqCommand, reqResult);
}
void rfu_STC_readChildList(void)
{
    u32 stwiParam;
    u8 numSlots = gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[1];
    u8 *data_p;
    u8 i;
    u8 bm_slot_id;
    u8 true_slots[4];
    if (numSlots != 0)
    {
        stwiParam = gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket32.data[0];
        STWI_set_Callback_M(rfu_CB_defaultCallback);
        STWI_send_LinkStatusREQ();
        if (STWI_poll_CommandEnd() == 0)
        {
            data_p = &gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[4];
            for (i = 0; i < 4; ++i)
                true_slots[i] = *data_p++;
        }
        gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket32.data[0] = stwiParam;
    }
    for (data_p = &gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[4];
         numSlots != 0;
         data_p += 4)
    {
        bm_slot_id = data_p[2];
        if (bm_slot_id < 4 && !((gRfuLinkStatus->connSlotFlag >> bm_slot_id) & 1) && !((gRfuLinkStatus->linkLossSlotFlag >> bm_slot_id) & 1))
        {
            if (true_slots[bm_slot_id] != 0)
                ++gRfuStatic->lsFixedCount[bm_slot_id];
            if (gRfuStatic->lsFixedCount[bm_slot_id] >= 4)
            {
                gRfuStatic->lsFixedCount[bm_slot_id] = 0;
                gRfuLinkStatus->strength[bm_slot_id] = 255;
                gRfuLinkStatus->connSlotFlag |= 1 << bm_slot_id;
                ++gRfuLinkStatus->connCount;
                gRfuLinkStatus->partner[bm_slot_id].id = *(u16 *)data_p;
                gRfuLinkStatus->partner[bm_slot_id].slot = bm_slot_id;
                gRfuLinkStatus->parentChild = 0x01;
                gRfuStatic->flags &= 0x7F;
                gRfuStatic->cidBak[bm_slot_id] = gRfuLinkStatus->partner[bm_slot_id].id;
            }
        }
        --numSlots;
    }
}
void rfu_REQ_startSearchParent(void)
{
    STWI_set_Callback_M(rfu_CB_startSearchParent);
    STWI_send_SP_StartREQ();
}
void rfu_CB_startSearchParent(u8 reqCommand, u16 reqResult)
{
    if (reqResult == 0)
        rfu_STC_clearLinkStatus(0x00);
    rfu_STC_REQ_callback(reqCommand, reqResult);
}
void rfu_REQ_pollSearchParent(void)
{
    STWI_set_Callback_M(rfu_CB_pollSearchParent);
    STWI_send_SP_PollingREQ();
}
void rfu_CB_pollSearchParent(u8 reqCommand, u16 reqResult)
{
    if (reqResult == 0)
        rfu_STC_readParentCandidateList();
    rfu_STC_REQ_callback(reqCommand, reqResult);
}
void rfu_REQ_endSearchParent(void)
{
    STWI_set_Callback_M(rfu_STC_REQ_callback);
    STWI_send_SP_EndREQ();
}
void rfu_STC_readParentCandidateList(void)
{
    u8 numSlots, i, check_sum, my_check_sum, j;
    u8 *uname_p, *packet_p;
    struct RfuTgtData *target;
    { vu16 tmp = (vu16)(0); CpuSet((void *)&tmp, gRfuLinkStatus->partner, 0x00000000 | 0x01000000 | ((sizeof(gRfuLinkStatus->partner))/(16/8) & 0x1FFFFF)); };
    packet_p = &gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[0];
    numSlots = packet_p[1];
    packet_p += 4;
    gRfuLinkStatus->findParentCount = 0;
    for (i = 0; i < 4 && numSlots != 0; ++i)
    {
        numSlots -= 7;
        uname_p = packet_p + 6;
        packet_p += 19;
        check_sum = ~*packet_p;
        ++packet_p;
        my_check_sum = 0;
        for (j = 0; j < 8; ++j)
        {
            my_check_sum += *packet_p++;
            my_check_sum += *uname_p++;
        }
        if (my_check_sum == check_sum)
        {
            packet_p -= 28;
            target = &gRfuLinkStatus->partner[gRfuLinkStatus->findParentCount];
            target->id = *(u16 *)packet_p;
            packet_p += 2;
            target->slot = *packet_p;
            packet_p += 2;
            target->serialNo = *(u16 *)packet_p & 0x7FFF;
            if (*(u16 *)packet_p & 0x8000)
                target->mbootFlag = 1;
            else
                target->mbootFlag = 0;
            packet_p += 2;
            for (j = 0; j < 13; ++j)
                target->gname[j] = *packet_p++;
            ++packet_p;
            for (j = 0; j < 7 + 1; ++j)
                target->uname[j] = *packet_p++;
            ++gRfuLinkStatus->findParentCount;
        }
    }
}
void rfu_REQ_startConnectParent(u16 pid)
{
    u16 result = 0;
    u8 i;
    for (i = 0; i < 4 && gRfuLinkStatus->partner[i].id != pid; ++i)
        ;
    if (i == 4)
        result = 0x0100;
    if (result == 0)
    {
        gRfuStatic->tryPid = pid;
        STWI_set_Callback_M(rfu_STC_REQ_callback);
        STWI_send_CP_StartREQ(pid);
    }
    else
    {
        rfu_STC_REQ_callback(0x001f, result);
    }
}
void rfu_REQ_pollConnectParent(void)
{
    STWI_set_Callback_M(rfu_CB_pollConnectParent);
    STWI_send_CP_PollingREQ();
}
void rfu_CB_pollConnectParent(u8 reqCommand, u16 reqResult)
{
    u16 id;
    u8 slot;
    u8 bm_slot_flag, i;
    struct RfuTgtData *target_p;
    struct RfuTgtData target_local;
    if (reqResult == 0)
    {
        id = gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket32.data[0];
        slot = gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[6];
        if (gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[7] == 0)
        {
            bm_slot_flag = 1 << slot;
            if (!(bm_slot_flag & gRfuLinkStatus->connSlotFlag))
            {
                gRfuLinkStatus->connSlotFlag |= bm_slot_flag;
                gRfuLinkStatus->linkLossSlotFlag &= ~bm_slot_flag;
                gRfuLinkStatus->my.id = id;
                ++gRfuLinkStatus->connCount;
                gRfuLinkStatus->parentChild = 0x00;
                gRfuStatic->flags |= 0x80;
                for (i = 0; i < 4; ++i)
                {
                    if (gRfuLinkStatus->partner[i].id == gRfuStatic->tryPid)
                    {
                        if (gRfuLinkStatus->findParentCount != 0)
                        {
                            target_p = &target_local;
                            CpuSet(&gRfuLinkStatus->partner[i], &target_local, 0x00000000 | ((sizeof(struct RfuTgtData))/(16/8) & 0x1FFFFF));
                            { vu16 tmp = (vu16)(0); CpuSet((void *)&tmp, gRfuLinkStatus->partner, 0x00000000 | 0x01000000 | ((sizeof(gRfuLinkStatus->partner))/(16/8) & 0x1FFFFF)); };
                            gRfuLinkStatus->findParentCount = 0;
                        }
                        else
                        {
                            target_p = &gRfuLinkStatus->partner[i];
                        }
                        break;
                    }
                }
                if (i < 4)
                {
                    CpuSet(target_p, &gRfuLinkStatus->partner[slot], 0x00000000 | ((sizeof(struct RfuTgtData))/(16/8) & 0x1FFFFF));
                    gRfuLinkStatus->partner[slot].slot = slot;
                }
            }
        }
    }
    rfu_STC_REQ_callback(reqCommand, reqResult);
}
u16 rfu_getConnectParentStatus(u8 *status, u8 *connectSlotNo)
{
    u8 *packet_p;
    *status = 0xFF;
    packet_p = gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data;
    if (packet_p[0] == 0xa0 || packet_p[0] == 0xa1)
    {
        packet_p += 6;
        *connectSlotNo = packet_p[0];
        *status = packet_p[1];
        return 0;
    }
    return (0x0000 | 0x0010);
}
void rfu_REQ_endConnectParent(void)
{
    STWI_set_Callback_M(rfu_CB_pollConnectParent);
    STWI_send_CP_EndREQ();
    if (gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[6] < 4)
        gRfuStatic->linkEmergencyFlag[gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[6]] = 0;
}
u16 rfu_syncVBlank(void)
{
    u8 masterSlave, i;
    s32 bmSlotFlag;
    rfu_NI_checkCommFailCounter();
    if (gRfuLinkStatus->parentChild == 0xff)
        return 0;
    if (gRfuStatic->nowWatchInterval != 0)
        --gRfuStatic->nowWatchInterval;
    masterSlave = rfu_getMasterSlave();
    if (!(gRfuStatic->flags & 2))
    {
        if (masterSlave == 0)
        {
            gRfuStatic->flags |= 4;
            gRfuStatic->watchdogTimer = 360;
        }
    }
    else if (masterSlave != 0)
    {
        gRfuStatic->flags &= 0xFB;
    }
    if (masterSlave != 0)
        gRfuStatic->flags &= 0xFD;
    else
        gRfuStatic->flags |= 2;
    if (!(gRfuStatic->flags & 4))
        return 0;
    if (gRfuStatic->watchdogTimer == 0)
    {
        gRfuStatic->flags &= 0xFB;
        bmSlotFlag = gRfuLinkStatus->connSlotFlag | gRfuLinkStatus->linkLossSlotFlag;
        for (i = 0; i < 4; ++i)
            if ((bmSlotFlag >> i) & 1)
                rfu_STC_removeLinkData(i, 1);
        gRfuLinkStatus->parentChild = 0xff;
        return 1;
    }
    --gRfuStatic->watchdogTimer;
    return 0;
}
u16 rfu_REQBN_watchLink(u16 reqCommandId, u8 *bmLinkLossSlot, u8 *linkLossReason, u8 *parentBmLinkRecoverySlot)
{
    u8 reasonMaybe = 0;
    u8 reqResult = 0;
    u8 i;
    s32 stwiCommand, stwiParam;
    u8 *packet_p;
    u8 to_req_disconnect, newLinkLossFlag, num_packets, connSlotFlag, to_disconnect;
    *bmLinkLossSlot = 0;
    *linkLossReason = 0x00;
    *parentBmLinkRecoverySlot = 0;
    if (gRfuLinkStatus->parentChild == 0xff || gSTWIStatus->msMode == 0)
        return 0;
    if (gRfuStatic->flags & 4)
        gRfuStatic->watchdogTimer = 360;
    if (gRfuStatic->nowWatchInterval == 0)
    {
        gRfuStatic->nowWatchInterval = gRfuLinkStatus->watchInterval;
        reasonMaybe = 1;
    }
    if ((u8)reqCommandId == 0x0029)
    {
        u8 *packet_p_2 = gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data;
        *bmLinkLossSlot = packet_p_2[4];
        *linkLossReason = packet_p_2[5];
        if (*linkLossReason == 0x01)
            *bmLinkLossSlot = gRfuLinkStatus->connSlotFlag;
        reasonMaybe = 2;
    }
    else
    {
        if (reqCommandId == 0x0136)
        {
            newLinkLossFlag = gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[5];
            newLinkLossFlag ^= gRfuLinkStatus->connSlotFlag;
            *bmLinkLossSlot = newLinkLossFlag & gRfuLinkStatus->connSlotFlag;
            *linkLossReason = 0x01;
            for (i = 0; i < 4; ++i)
            {
                if ((*bmLinkLossSlot >> i) & 1)
                {
                    gRfuLinkStatus->strength[i] = 0;
                    rfu_STC_removeLinkData(i, 0);
                }
            }
        }
        if (reasonMaybe == 0)
            return 0;
    }
    stwiCommand = gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket32.command;
    stwiParam = gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket32.data[0];
    STWI_set_Callback_M(rfu_CB_defaultCallback);
    STWI_send_LinkStatusREQ();
    reqResult = STWI_poll_CommandEnd();
    if (reqResult == 0)
    {
        packet_p = &gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[4];
        for (i = 0; i < 4; ++i)
            gRfuLinkStatus->strength[i] = *packet_p++;
        to_req_disconnect = 0;
        i = 0;
    }
    else
    {
        rfu_STC_REQ_callback(0x0011, reqResult);
        return reqResult;
    }
    for (; i < 4; ++i)
    {
        newLinkLossFlag = 1 << i;
        if (reqResult == 0)
        {
            if (reasonMaybe == 1 && (gRfuLinkStatus->connSlotFlag & newLinkLossFlag))
            {
                if (gRfuLinkStatus->strength[i] == 0)
                {
                    if (gRfuLinkStatus->parentChild == 0x01)
                    {
                        ++gRfuStatic->linkEmergencyFlag[i];
                        if (gRfuStatic->linkEmergencyFlag[i] > 3)
                        {
                            *bmLinkLossSlot |= newLinkLossFlag;
                            *linkLossReason = 0x01;
                        }
                    }
                    else
                    {
                        STWI_send_SystemStatusREQ();
                        if (STWI_poll_CommandEnd() == 0)
                        {
                            if (gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[7] == 0)
                            {
                                *bmLinkLossSlot |= newLinkLossFlag;
                                *linkLossReason = 0x01;
                            }
                            else
                            {
                                if (++gRfuStatic->linkEmergencyFlag[i] > gRfuStatic->linkEmergencyLimit)
                                {
                                    gRfuStatic->linkEmergencyFlag[i] = 0;
                                    STWI_send_DisconnectREQ(gRfuLinkStatus->connSlotFlag);
                                    STWI_poll_CommandEnd();
                                    *bmLinkLossSlot |= newLinkLossFlag;
                                    *linkLossReason = 0x01;
                                }
                            }
                        }
                    }
                }
                else
                {
                    gRfuStatic->linkEmergencyFlag[i] = 0;
                }
            }
            if (gRfuLinkStatus->parentChild == 0x01 && gRfuLinkStatus->strength[i] != 0)
            {
                if (newLinkLossFlag & gRfuLinkStatus->linkLossSlotFlag)
                {
                    if (gRfuLinkStatus->strength[i] > 10)
                    {
                        *parentBmLinkRecoverySlot |= newLinkLossFlag;
                        gRfuLinkStatus->connSlotFlag |= newLinkLossFlag;
                        gRfuLinkStatus->linkLossSlotFlag &= ~newLinkLossFlag;
                        ++gRfuLinkStatus->connCount;
                        gRfuStatic->linkEmergencyFlag[i] = 0;
                    }
                    else
                    {
                        gRfuLinkStatus->strength[i] = 0;
                    }
                }
                else
                {
                    if (!((gRfuLinkStatus->connSlotFlag | gRfuLinkStatus->linkLossSlotFlag) & newLinkLossFlag))
                    {
                        STWI_send_SlotStatusREQ();
                        STWI_poll_CommandEnd();
                        packet_p = gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data;
                        num_packets = packet_p[1] - 1;
                        for (packet_p += 8; num_packets != 0; packet_p += 4, --num_packets)
                        {
                            u16 cid = *(u16 *)packet_p;
                            if (packet_p[2] == i && cid == gRfuStatic->cidBak[i])
                            {
                                to_req_disconnect |= 1 << i;
                                break;
                            }
                        }
                    }
                }
            }
        }
        connSlotFlag = gRfuLinkStatus->connSlotFlag;
        to_disconnect = *bmLinkLossSlot;
        to_disconnect &= connSlotFlag;
        if (newLinkLossFlag & to_disconnect)
            rfu_STC_removeLinkData(i, 0);
    }
    if (to_req_disconnect != 0)
    {
        STWI_send_DisconnectREQ(to_req_disconnect);
        STWI_poll_CommandEnd();
    }
    *(u32 *)gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data = stwiCommand;
    gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket32.data[0] = stwiParam;
    return 0;
}
void rfu_STC_removeLinkData(u8 bmConnectedPartnerId, u8 bmDisconnect)
{
    u8 bmLinkLossFlag = 1 << bmConnectedPartnerId;
    s32 bmLinkRetainedFlag;
    if ((gRfuLinkStatus->connSlotFlag & bmLinkLossFlag) && gRfuLinkStatus->connCount != 0)
        --gRfuLinkStatus->connCount;
    gRfuLinkStatus->connSlotFlag &= bmLinkRetainedFlag = ~bmLinkLossFlag;
    gRfuLinkStatus->linkLossSlotFlag |= bmLinkLossFlag;
    if (gRfuLinkStatus->parentChild == 0x00 && gRfuLinkStatus->connSlotFlag == 0)
        gRfuLinkStatus->parentChild = 0xff;
    if (bmDisconnect)
    {
        { vu16 tmp = (vu16)(0); CpuSet((void *)&tmp, &gRfuLinkStatus->partner[bmConnectedPartnerId], 0x00000000 | 0x01000000 | ((sizeof(struct RfuTgtData))/(16/8) & 0x1FFFFF)); };
        gRfuLinkStatus->linkLossSlotFlag &= bmLinkRetainedFlag;
        gRfuLinkStatus->getNameFlag &= bmLinkRetainedFlag;
        gRfuLinkStatus->strength[bmConnectedPartnerId] = 0;
    }
}
void rfu_REQ_disconnect(u8 bmDisconnectSlot)
{
    u16 result;
    if ((gRfuLinkStatus->connSlotFlag | gRfuLinkStatus->linkLossSlotFlag) & bmDisconnectSlot)
    {
        gRfuStatic->recoveryBmSlot = bmDisconnectSlot;
        if (gRfuLinkStatus->parentChild == 0xff && gRfuStatic->flags & 0x80)
        {
            if (gRfuLinkStatus->linkLossSlotFlag & bmDisconnectSlot)
                rfu_CB_disconnect(48, 0);
        }
        else if (gRfuStatic->SCStartFlag
              && (STWI_set_Callback_M(rfu_CB_defaultCallback),
                  STWI_send_SC_EndREQ(),
                  (result = STWI_poll_CommandEnd()) != 0))
        {
            rfu_STC_REQ_callback(0x001b, result);
        }
        else
        {
            STWI_set_Callback_M(rfu_CB_disconnect);
            STWI_send_DisconnectREQ(bmDisconnectSlot);
        }
    }
}
void rfu_CB_disconnect(u8 reqCommand, u16 reqResult)
{
    u8 i, bm_slot_flag;
    if (reqResult == 3 && gRfuLinkStatus->parentChild == 0x00)
    {
        STWI_set_Callback_M(rfu_CB_defaultCallback);
        STWI_send_SystemStatusREQ();
        if (STWI_poll_CommandEnd() == 0 && gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[7] == 0)
            reqResult = 0;
    }
    gRfuStatic->recoveryBmSlot &= gRfuLinkStatus->connSlotFlag | gRfuLinkStatus->linkLossSlotFlag;
    gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[8] = gRfuStatic->recoveryBmSlot;
    if (reqResult == 0)
    {
        for (i = 0; i < 4; ++i)
        {
            bm_slot_flag = 1 << i;
            if (bm_slot_flag & gRfuStatic->recoveryBmSlot)
                rfu_STC_removeLinkData(i, 1);
        }
    }
    if ((gRfuLinkStatus->connSlotFlag | gRfuLinkStatus->linkLossSlotFlag) == 0)
        gRfuLinkStatus->parentChild = 0xff;
    rfu_STC_REQ_callback(reqCommand, reqResult);
    if (gRfuStatic->SCStartFlag)
    {
        STWI_set_Callback_M(rfu_CB_defaultCallback);
        STWI_send_SC_StartREQ();
        reqResult = STWI_poll_CommandEnd();
        if (reqResult != 0)
            rfu_STC_REQ_callback(0x0019, reqResult);
    }
}
void rfu_REQ_CHILD_startConnectRecovery(u8 bmRecoverySlot)
{
    u8 i;
    gRfuStatic->recoveryBmSlot = bmRecoverySlot;
    for (i = 0; i < 4 && !((bmRecoverySlot >> i) & 1); ++i)
        ;
    STWI_set_Callback_M(rfu_STC_REQ_callback);
    STWI_send_CPR_StartREQ(gRfuLinkStatus->partner[i].id, gRfuLinkStatus->my.id, bmRecoverySlot);
}
void rfu_REQ_CHILD_pollConnectRecovery(void)
{
    STWI_set_Callback_M(rfu_CB_CHILD_pollConnectRecovery);
    STWI_send_CPR_PollingREQ();
}
void rfu_CB_CHILD_pollConnectRecovery(u8 reqCommand, u16 reqResult)
{
    u8 bm_slot_flag, i;
    struct RfuLinkStatus *rfuLinkStatus;
    if (reqResult == 0 && gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[4] == 0 && gRfuStatic->recoveryBmSlot)
    {
        gRfuLinkStatus->parentChild = 0x00;
        for (i = 0; i < 4; ++i)
        {
            bm_slot_flag = 1 << i;
            rfuLinkStatus = gRfuLinkStatus;
            if (gRfuStatic->recoveryBmSlot & bm_slot_flag & rfuLinkStatus->linkLossSlotFlag)
            {
                gRfuLinkStatus->connSlotFlag |= bm_slot_flag;
                gRfuLinkStatus->linkLossSlotFlag &= ~bm_slot_flag;
                ++gRfuLinkStatus->connCount;
                gRfuStatic->linkEmergencyFlag[i] = 0;
            }
        }
        gRfuStatic->recoveryBmSlot = 0;
    }
    rfu_STC_REQ_callback(reqCommand, reqResult);
}
u16 rfu_CHILD_getConnectRecoveryStatus(u8 *status)
{
    *status = 0xFF;
    if (gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[0] == 0xB3 || gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[0] == 0xB4)
    {
        *status = gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[4];
        return 0;
    }
    return (0x0000 | 0x0010);
}
void rfu_REQ_CHILD_endConnectRecovery(void)
{
    STWI_set_Callback_M(rfu_CB_CHILD_pollConnectRecovery);
    STWI_send_CPR_EndREQ();
}
void rfu_STC_fastCopy(const u8 **src_p, u8 **dst_p, s32 size)
{
    const u8 *src = *src_p;
    u8 *dst = *dst_p;
    s32 i;
    for (i = size - 1; i != -1; --i)
        *dst++ = *src++;
    *src_p = src;
    *dst_p = dst;
}
void rfu_REQ_changeMasterSlave(void)
{
    if (STWI_read_status(1) == 1)
    {
        STWI_set_Callback_M(rfu_STC_REQ_callback);
        STWI_send_MS_ChangeREQ();
    }
    else
    {
        rfu_STC_REQ_callback(0x0027, 0);
    }
}
bool8 rfu_getMasterSlave(void)
{
    bool8 masterSlave = STWI_read_status(1);
    if (masterSlave == 1)
    {
        if (gSTWIStatus->sending)
        {
            if (gSTWIStatus->reqActiveCommand == 0x0027
             || gSTWIStatus->reqActiveCommand == 0x0025
             || gSTWIStatus->reqActiveCommand == 0x0037)
                masterSlave = 0;
        }
    }
    return masterSlave;
}
void rfu_clearAllSlot(void)
{
    u16 i;
    u16 IMEBackup = (*(vu16 *)(0x4000000 + 0x208));
    (*(vu16 *)(0x4000000 + 0x208)) = 0;
    for (i = 0; i < 4; ++i)
    {
        { vu16 tmp = (vu16)(0); CpuSet((void *)&tmp, gRfuSlotStatusNI[i], 0x00000000 | 0x01000000 | ((2 * sizeof(struct NIComm))/(16/8) & 0x1FFFFF)); };
        { vu16 tmp = (vu16)(0); CpuSet((void *)&tmp, gRfuSlotStatusUNI[i], 0x00000000 | 0x01000000 | ((sizeof(struct UNISend) + sizeof(struct UNIRecv))/(16/8) & 0x1FFFFF)); };
        gRfuLinkStatus->remainLLFrameSizeChild[i] = 16;
    }
    gRfuLinkStatus->remainLLFrameSizeParent = 87;
    gRfuLinkStatus->sendSlotNIFlag = 0;
    gRfuLinkStatus->recvSlotNIFlag = 0;
    gRfuLinkStatus->sendSlotUNIFlag = 0;
    gRfuStatic->recvRenewalFlag = 0;
    (*(vu16 *)(0x4000000 + 0x208)) = IMEBackup;
}
void rfu_STC_releaseFrame(u8 bm_slot_id, u8 send_recv, struct NIComm *NI_comm)
{
    if (!(gRfuStatic->flags & 0x80))
    {
        if (send_recv == 0)
            gRfuLinkStatus->remainLLFrameSizeParent += NI_comm->payloadSize;
        gRfuLinkStatus->remainLLFrameSizeParent += 3;
    }
    else
    {
        if (send_recv == 0)
            gRfuLinkStatus->remainLLFrameSizeChild[bm_slot_id] += NI_comm->payloadSize;
        gRfuLinkStatus->remainLLFrameSizeChild[bm_slot_id] += 2;
    }
}
u16 rfu_clearSlot(u8 connTypeFlag, u8 slotStatusIndex)
{
    u16 imeBak, send_recv, i;
    struct NIComm *NI_comm;
    if (slotStatusIndex >= 4)
        return (0x0400 | 0x0000);
    if (!(connTypeFlag & (0x01 | 0x02 | 0x04 | 0x08)))
        return 0x0600;
    imeBak = (*(vu16 *)(0x4000000 + 0x208));
    (*(vu16 *)(0x4000000 + 0x208)) = 0;
    if (connTypeFlag & (0x04 | 0x08))
    {
        for (send_recv = 0; send_recv < 2; ++send_recv)
        {
            NI_comm = ((void *)0);
            if (send_recv == 0)
            {
                if (connTypeFlag & 0x04)
                {
                    NI_comm = &gRfuSlotStatusNI[slotStatusIndex]->send;
                    gRfuLinkStatus->sendSlotNIFlag &= ~NI_comm->bmSlotOrg;
                }
            }
            else
            {
                if (connTypeFlag & 0x08)
                {
                    NI_comm = &gRfuSlotStatusNI[slotStatusIndex]->recv;
                    gRfuLinkStatus->recvSlotNIFlag &= ~(1 << slotStatusIndex);
                }
            }
            if (NI_comm != ((void *)0))
            {
                if (NI_comm->state & 0x8000)
                {
                    rfu_STC_releaseFrame(slotStatusIndex, send_recv, NI_comm);
                    for (i = 0; i < 4; ++i)
                        if ((NI_comm->bmSlotOrg >> i) & 1)
                            NI_comm->failCounter = 0;
                }
                { vu16 tmp = (vu16)(0); CpuSet((void *)&tmp, NI_comm, 0x00000000 | 0x01000000 | ((sizeof(struct NIComm))/(16/8) & 0x1FFFFF)); };
            }
        }
    }
    if (connTypeFlag & 0x01)
    {
        struct RfuSlotStatusUNI *slotStatusUNI = gRfuSlotStatusUNI[slotStatusIndex];
        if (slotStatusUNI->send.state & 0x8000)
        {
            if (!(gRfuStatic->flags & 0x80))
                gRfuLinkStatus->remainLLFrameSizeParent += 3 + (u8)slotStatusUNI->send.payloadSize;
            else
                gRfuLinkStatus->remainLLFrameSizeChild[slotStatusIndex] += 2 + (u8)slotStatusUNI->send.payloadSize;
            gRfuLinkStatus->sendSlotUNIFlag &= ~slotStatusUNI->send.bmSlot;
        }
        { vu16 tmp = (vu16)(0); CpuSet((void *)&tmp, &slotStatusUNI->send, 0x00000000 | 0x01000000 | ((sizeof(struct UNISend))/(16/8) & 0x1FFFFF)); };
    }
    if (connTypeFlag & 0x02)
    {
        { vu16 tmp = (vu16)(0); CpuSet((void *)&tmp, &gRfuSlotStatusUNI[slotStatusIndex]->recv, 0x00000000 | 0x01000000 | ((sizeof(struct UNIRecv))/(16/8) & 0x1FFFFF)); };
    }
    (*(vu16 *)(0x4000000 + 0x208)) = imeBak;
    return 0;
}
u16 rfu_setRecvBuffer(u8 connType, u8 slotNo, void *buffer, u32 buffSize)
{
    if (slotNo >= 4)
        return (0x0400 | 0x0000);
    if (connType & 0x20)
    {
        gRfuSlotStatusNI[slotNo]->recvBuffer = buffer;
        gRfuSlotStatusNI[slotNo]->recvBufferSize = buffSize;
    }
    else if (!(connType & 0x10))
    {
        return 0x0600;
    }
    else
    {
        gRfuSlotStatusUNI[slotNo]->recvBuffer = buffer;
        gRfuSlotStatusUNI[slotNo]->recvBufferSize = buffSize;
    }
    return 0;
}
u16 rfu_NI_setSendData(u8 bmSendSlot, u8 subFrameSize, const void *src, u32 size)
{
    return rfu_STC_setSendData_org(32, bmSendSlot, subFrameSize, src, size);
}
u16 rfu_UNI_setSendData(u8 bmSendSlot, const void *src, u8 size)
{
    u8 subFrameSize;
    if (gRfuLinkStatus->parentChild == 0x01)
        subFrameSize = size + 3;
    else
        subFrameSize = size + 2;
    return rfu_STC_setSendData_org(16, bmSendSlot, subFrameSize, src, 0);
}
u16 rfu_NI_CHILD_setSendGameName(u8 slotNo, u8 subFrameSize)
{
    return rfu_STC_setSendData_org(64, 1 << slotNo, subFrameSize, &gRfuLinkStatus->my.serialNo, 26);
}
u16 rfu_STC_setSendData_org(u8 ni_or_uni, u8 bmSendSlot, u8 subFrameSize, const void *src, u32 dataSize)
{
    u8 bm_slot_id, sendSlotFlag;
    u8 frameSize;
    u8 *llFrameSize_p;
    u8 sending;
    u8 i;
    u16 imeBak;
    struct RfuSlotStatusUNI *slotStatus_UNI;
    struct RfuSlotStatusNI *slotStatus_NI;
    if (gRfuLinkStatus->parentChild == 0xff)
        return (0x0300 | 0x0001);
    if (!(bmSendSlot & 0xF))
        return (0x0400 | 0x0000);
    if (((gRfuLinkStatus->connSlotFlag | gRfuLinkStatus->linkLossSlotFlag) & bmSendSlot) != bmSendSlot)
        return (0x0400 | 0x0001);
    if (ni_or_uni & 0x10)
        sendSlotFlag = gRfuLinkStatus->sendSlotUNIFlag;
    else
        sendSlotFlag = gRfuLinkStatus->sendSlotNIFlag;
    if (sendSlotFlag & bmSendSlot)
        return (0x0400 | 0x0002);
    for (bm_slot_id = 0; bm_slot_id < 4 && !((bmSendSlot >> bm_slot_id) & 1); ++bm_slot_id)
        ;
    if (gRfuLinkStatus->parentChild == 0x01)
        llFrameSize_p = &gRfuLinkStatus->remainLLFrameSizeParent;
    else if (gRfuLinkStatus->parentChild == 0x00)
        llFrameSize_p = &gRfuLinkStatus->remainLLFrameSizeChild[bm_slot_id];
    frameSize = llsf_struct[gRfuLinkStatus->parentChild].frameSize;
    if (subFrameSize > *llFrameSize_p || subFrameSize <= frameSize)
        return 0x0500;
    imeBak = (*(vu16 *)(0x4000000 + 0x208));
    (*(vu16 *)(0x4000000 + 0x208)) = 0;
    sending = ni_or_uni & 0x20;
    if (sending || ni_or_uni == 0x40)
    {
        slotStatus_NI = gRfuSlotStatusNI[bm_slot_id];
        slotStatus_UNI = ((void *)0);
        slotStatus_NI->send.errorCode = 0;
        slotStatus_NI->send.now_p[0] = &slotStatus_NI->send.dataType;
        slotStatus_NI->send.remainSize = 7;
        slotStatus_NI->send.bmSlotOrg = bmSendSlot;
        slotStatus_NI->send.bmSlot = bmSendSlot;
        slotStatus_NI->send.payloadSize = subFrameSize - frameSize;
        if (sending != 0)
            slotStatus_NI->send.dataType = 0;
        else
            slotStatus_NI->send.dataType = 1;
        slotStatus_NI->send.dataSize = dataSize;
        slotStatus_NI->send.src = src;
        slotStatus_NI->send.ack = 0;
        slotStatus_NI->send.phase = 0;
        for (i = 0; i < 4; ++i)
        {
            slotStatus_NI->send.recvAckFlag[i] = 0;
            slotStatus_NI->send.n[i] = 1;
        }
        for (bm_slot_id = 0; bm_slot_id < 4; ++bm_slot_id)
        {
            do
            {
                if ((bmSendSlot >> bm_slot_id) & 1)
                    gRfuSlotStatusNI[bm_slot_id]->send.failCounter = 0;
            } while (0);
        }
        gRfuLinkStatus->sendSlotNIFlag |= bmSendSlot;
            *llFrameSize_p -= subFrameSize;
        slotStatus_NI->send.state = (0x8000 | 0x0020 | 0x0001);
    }
    else if (ni_or_uni & 0x10)
    {
        slotStatus_UNI = gRfuSlotStatusUNI[bm_slot_id];
        slotStatus_UNI->send.bmSlot = bmSendSlot;
        slotStatus_UNI->send.src = src;
        slotStatus_UNI->send.payloadSize = subFrameSize - frameSize;
            *llFrameSize_p -= subFrameSize;
        slotStatus_UNI->send.state = (0x8000 | 0x0020 | 0x0004);
        gRfuLinkStatus->sendSlotUNIFlag |= bmSendSlot;
    }
    (*(vu16 *)(0x4000000 + 0x208)) = imeBak;
    return 0;
}
u16 rfu_changeSendTarget(u8 connType, u8 slotStatusIndex, u8 bmNewTgtSlot)
{
    struct RfuSlotStatusNI *slotStatusNI;
    u16 imeBak;
    u8 i;
    if (slotStatusIndex >= 4)
        return (0x0400 | 0x0000);
    if (connType == 0x20)
    {
        slotStatusNI = gRfuSlotStatusNI[slotStatusIndex];
        if ((slotStatusNI->send.state & 0x8000)
         && (slotStatusNI->send.state & 0x0020))
        {
            connType = bmNewTgtSlot ^ slotStatusNI->send.bmSlot;
            if (!(connType & bmNewTgtSlot))
            {
                if (connType)
                {
                    imeBak = (*(vu16 *)(0x4000000 + 0x208));
                    (*(vu16 *)(0x4000000 + 0x208)) = 0;
                    for (i = 0; i < 4; ++i)
                    {
                        if ((connType >> i) & 1)
                            gRfuSlotStatusNI[i]->send.failCounter = 0;
                    }
                    gRfuLinkStatus->sendSlotNIFlag &= ~connType;
                    slotStatusNI->send.bmSlot = bmNewTgtSlot;
                    if (slotStatusNI->send.bmSlot == 0)
                    {
                        rfu_STC_releaseFrame(slotStatusIndex, 0, &slotStatusNI->send);
                        slotStatusNI->send.state = ( 0x0020 | 0x007);
                    }
                    (*(vu16 *)(0x4000000 + 0x208)) = imeBak;
                }
            }
            else
            {
                return (0x0400 | 0x0004);
            }
        }
        else
        {
            return (0x0400 | 0x0003);
        }
    }
    else
    {
        if (connType == 16)
        {
            s32 bmSlot;
            if (gRfuSlotStatusUNI[slotStatusIndex]->send.state != (0x8000 | 0x0020 | 0x0004))
                return (0x0400 | 0x0003);
            for (bmSlot = 0, i = 0; i < 4; ++i)
                if (i != slotStatusIndex)
                    bmSlot |= gRfuSlotStatusUNI[i]->send.bmSlot;
            if (bmNewTgtSlot & bmSlot)
                return (0x0400 | 0x0004);
            imeBak = (*(vu16 *)(0x4000000 + 0x208));
            (*(vu16 *)(0x4000000 + 0x208)) = 0;
            gRfuLinkStatus->sendSlotUNIFlag &= ~gRfuSlotStatusUNI[slotStatusIndex]->send.bmSlot;
            gRfuLinkStatus->sendSlotUNIFlag |= bmNewTgtSlot;
            gRfuSlotStatusUNI[slotStatusIndex]->send.bmSlot = bmNewTgtSlot;
            (*(vu16 *)(0x4000000 + 0x208)) = imeBak;
        }
        else
        {
            return 0x0600;
        }
    }
    return 0;
}
u16 rfu_NI_stopReceivingData(u8 slotStatusIndex)
{
    u16 imeBak;
    struct NIComm *NI_comm;
    if (slotStatusIndex >= 4)
        return (0x0400 | 0x0000);
    NI_comm = &gRfuSlotStatusNI[slotStatusIndex]->recv;
    imeBak = (*(vu16 *)(0x4000000 + 0x208));
    (*(vu16 *)(0x4000000 + 0x208)) = 0;
    if (NI_comm->state & 0x8000)
    {
        if (NI_comm->state == (0x8000 | 0x0040 | 0x0003))
            NI_comm->state = (0x0040 | 0x008);
        else
            NI_comm->state = ( 0x0040 | 0x007);
        gRfuLinkStatus->recvSlotNIFlag &= ~(1 << slotStatusIndex);
        rfu_STC_releaseFrame(slotStatusIndex, 1, NI_comm);
    }
    (*(vu16 *)(0x4000000 + 0x208)) = imeBak;
    return 0;
}
u16 rfu_UNI_changeAndReadySendData(u8 slotStatusIndex, const void *src, u8 size)
{
    struct UNISend *UNI_send;
    u8 *frame_p;
    u16 imeBak;
    u8 frameEnd;
    if (slotStatusIndex >= 4)
        return (0x0400 | 0x0000);
    UNI_send = &gRfuSlotStatusUNI[slotStatusIndex]->send;
    if (UNI_send->state != (0x8000 | 0x0020 | 0x0004))
        return (0x0400 | 0x0003);
    if (gRfuLinkStatus->parentChild == 0x01)
    {
        frame_p = &gRfuLinkStatus->remainLLFrameSizeParent;
        frameEnd = gRfuLinkStatus->remainLLFrameSizeParent + (u8)UNI_send->payloadSize;
    }
    else
    {
        frame_p = &gRfuLinkStatus->remainLLFrameSizeChild[slotStatusIndex];
        frameEnd = gRfuLinkStatus->remainLLFrameSizeChild[slotStatusIndex] + (u8)UNI_send->payloadSize;
    }
    if (frameEnd < size)
        return 0x0500;
    imeBak = (*(vu16 *)(0x4000000 + 0x208));
    (*(vu16 *)(0x4000000 + 0x208)) = 0;
    UNI_send->src = src;
    *frame_p = frameEnd - size;
    UNI_send->payloadSize = size;
    UNI_send->dataReadyFlag = 1;
    (*(vu16 *)(0x4000000 + 0x208)) = imeBak;
    return 0;
}
void rfu_UNI_readySendData(u8 slotStatusIndex)
{
    if (slotStatusIndex < 4)
    {
        if (gRfuSlotStatusUNI[slotStatusIndex]->send.state == (0x8000 | 0x0020 | 0x0004))
            gRfuSlotStatusUNI[slotStatusIndex]->send.dataReadyFlag = 1;
    }
}
void rfu_UNI_clearRecvNewDataFlag(u8 slotStatusIndex)
{
    if (slotStatusIndex < 4)
        gRfuSlotStatusUNI[slotStatusIndex]->recv.newDataFlag = 0;
}
void rfu_REQ_sendData(bool8 clockChangeFlag)
{
    if (gRfuLinkStatus->parentChild != 0xff)
    {
        if (gRfuLinkStatus->parentChild == 0x01
         && !(gRfuLinkStatus->sendSlotNIFlag | gRfuLinkStatus->recvSlotNIFlag | gRfuLinkStatus->sendSlotUNIFlag))
        {
            if (gRfuStatic->commExistFlag)
            {
                gRfuStatic->emberCount = 16;
                gRfuStatic->nullFrameCount = 0;
            }
            if (gRfuStatic->emberCount)
                --gRfuStatic->emberCount;
            else
                ++gRfuStatic->nullFrameCount;
            if (gRfuStatic->emberCount
             || !(gRfuStatic->nullFrameCount & 0xF))
            {
                gRfuFixed->LLFBuffer[0] = 1;
                gRfuFixed->LLFBuffer[4] = 0xFF;
                STWI_set_Callback_M(rfu_CB_sendData3);
                if (!clockChangeFlag)
                    STWI_send_DataTxREQ(gRfuFixed->LLFBuffer, 1);
                else
                    STWI_send_DataTxAndChangeREQ(gRfuFixed->LLFBuffer, 1);
                return;
            }
        }
        else
        {
            if (!gRfuLinkStatus->LLFReadyFlag)
                rfu_constructSendLLFrame();
            if (gRfuLinkStatus->LLFReadyFlag)
            {
                STWI_set_Callback_M(rfu_CB_sendData);
                if (clockChangeFlag)
                {
                    STWI_send_DataTxAndChangeREQ(gRfuFixed->LLFBuffer, gRfuStatic->totalPacketSize + 4);
                    return;
                }
                STWI_send_DataTxREQ(gRfuFixed->LLFBuffer, gRfuStatic->totalPacketSize + 4);
            }
        }
        if (clockChangeFlag)
        {
            if (gRfuLinkStatus->parentChild == 0x01)
            {
                if (gSTWIStatus->callbackS != ((void *)0))
                    gSTWIStatus->callbackS(39);
            }
            else
            {
                STWI_set_Callback_M(rfu_CB_sendData2);
                STWI_send_MS_ChangeREQ();
            }
        }
    }
}
void rfu_CB_sendData(__attribute__((unused)) u8 reqCommand, u16 reqResult)
{
    u8 i;
    struct NIComm *NI_comm;
    if (reqResult == 0)
    {
        for (i = 0; i < 4; ++i)
        {
            if (gRfuSlotStatusUNI[i]->send.dataReadyFlag)
                gRfuSlotStatusUNI[i]->send.dataReadyFlag = 0;
            NI_comm = &gRfuSlotStatusNI[i]->send;
            if (NI_comm->state == (0x8000 | 0x0020 | 0x0000))
            {
                rfu_STC_releaseFrame(i, 0, NI_comm);
                gRfuLinkStatus->sendSlotNIFlag &= ~NI_comm->bmSlot;
                if (NI_comm->dataType == 1)
                    gRfuLinkStatus->getNameFlag |= 1 << i;
                NI_comm->state = ( 0x0020 | 0x006);
            }
        }
    }
    gRfuLinkStatus->LLFReadyFlag = 0;
    rfu_STC_REQ_callback(0x0024, reqResult);
}
void rfu_CB_sendData2(__attribute__((unused)) u8 reqCommand, u16 reqResult)
{
    rfu_STC_REQ_callback(0x0024, reqResult);
}
void rfu_CB_sendData3(u8 reqCommand, u16 reqResult)
{
    if (reqResult != 0)
        rfu_STC_REQ_callback(0x0024, reqResult);
    else if (reqCommand == 0x00ff)
        rfu_STC_REQ_callback(0x00ff, 0);
}
void rfu_constructSendLLFrame(void)
{
    u32 pakcketSize, currSize;
    u8 i;
    u8 *llf_p;
    if (gRfuLinkStatus->parentChild != 0xff
     && gRfuLinkStatus->sendSlotNIFlag | gRfuLinkStatus->recvSlotNIFlag | gRfuLinkStatus->sendSlotUNIFlag)
    {
        gRfuLinkStatus->LLFReadyFlag = 0;
        pakcketSize = 0;
        llf_p = (u8 *)&gRfuFixed->LLFBuffer[1];
        for (i = 0; i < 4; ++i)
        {
            currSize = 0;
            if (gRfuSlotStatusNI[i]->send.state & 0x8000)
                currSize = rfu_STC_NI_constructLLSF(i, &llf_p, &gRfuSlotStatusNI[i]->send);
            if (gRfuSlotStatusNI[i]->recv.state & 0x8000)
                currSize += rfu_STC_NI_constructLLSF(i, &llf_p, &gRfuSlotStatusNI[i]->recv);
            if (gRfuSlotStatusUNI[i]->send.state == (0x8000 | 0x0020 | 0x0004))
                currSize += rfu_STC_UNI_constructLLSF(i, &llf_p);
            if (currSize != 0)
            {
                if (gRfuLinkStatus->parentChild == 0x01)
                    pakcketSize += currSize;
                else
                    pakcketSize |= currSize << (5 * i + 8);
            }
        }
        if (pakcketSize != 0)
        {
            while ((u32)llf_p & 3)
                *llf_p++ = 0;
            gRfuFixed->LLFBuffer[0] = pakcketSize;
            if (gRfuLinkStatus->parentChild == 0x00)
            {
                u8 *maxSize = llf_p - ((unsigned long)&((struct RfuFixed *)0)->LLFBuffer[1]);
                pakcketSize = maxSize - *(u8 *volatile *)&gRfuFixed;
            }
        }
        gRfuStatic->totalPacketSize = pakcketSize;
    }
}
u16 rfu_STC_NI_constructLLSF(u8 bm_slot_id, u8 **dest_pp, struct NIComm *NI_comm)
{
    u16 size;
    u32 frame;
    u8 i;
    u8 *frame8_p;
    const struct LLSFStruct *llsf = &llsf_struct[gRfuLinkStatus->parentChild];
    if (NI_comm->state == (0x8000 | 0x0020 | 0x0002))
    {
        while (NI_comm->now_p[NI_comm->phase] >= (const u8 *)NI_comm->src + NI_comm->dataSize)
        {
            ++NI_comm->phase;
            if (NI_comm->phase == 4)
                NI_comm->phase = 0;
        }
    }
    if (NI_comm->state & 0x0040)
    {
        size = 0;
    }
    else if (NI_comm->state == (0x8000 | 0x0020 | 0x0002))
    {
        if (NI_comm->now_p[NI_comm->phase] + NI_comm->payloadSize > (const u8 *)NI_comm->src + NI_comm->dataSize)
            size = (const u8 *)NI_comm->src + NI_comm->dataSize - NI_comm->now_p[NI_comm->phase];
        else
            size = NI_comm->payloadSize;
    }
    else
    {
        if (NI_comm->remainSize >= NI_comm->payloadSize)
            size = NI_comm->payloadSize;
        else
            size = NI_comm->remainSize;
    }
    frame = (NI_comm->state & 0xF) << llsf->slotStateShift
         | NI_comm->ack << llsf->ackShift
         | NI_comm->phase << llsf->phaseShift
         | NI_comm->n[NI_comm->phase] << llsf->nShift
         | size;
    if (gRfuLinkStatus->parentChild == 0x01)
        frame |= NI_comm->bmSlot << 18;
    frame8_p = (u8 *)&frame;
    for (i = 0; i < llsf->frameSize; ++i)
        *(*dest_pp)++ = *frame8_p++;
    if (size != 0)
    {
        const u8 *src = NI_comm->now_p[NI_comm->phase];
        gRfuFixed->fastCopyPtr(&src, dest_pp, size);
    }
    if (NI_comm->state == (0x8000 | 0x0020 | 0x0002))
    {
        ++NI_comm->phase;
        if (NI_comm->phase == 4)
            NI_comm->phase = 0;
    }
    if (gRfuLinkStatus->parentChild == 0x01)
        gRfuLinkStatus->LLFReadyFlag = 1;
    else
        gRfuLinkStatus->LLFReadyFlag |= 1 << bm_slot_id;
    return size + llsf->frameSize;
}
u16 rfu_STC_UNI_constructLLSF(u8 bm_slot_id, u8 **dest_p)
{
    const struct LLSFStruct *llsf;
    const u8 *src_p;
    u32 frame;
    u8 *frame8_p;
    u8 i;
    struct UNISend *UNI_send = &gRfuSlotStatusUNI[bm_slot_id]->send;
    if (!UNI_send->dataReadyFlag || !UNI_send->bmSlot)
        return 0;
    llsf = &llsf_struct[gRfuLinkStatus->parentChild];
    frame = (UNI_send->state & 0xF) << llsf->slotStateShift
         | UNI_send->payloadSize;
    if (gRfuLinkStatus->parentChild == 0x01)
        frame |= UNI_send->bmSlot << 18;
    frame8_p = (u8 *)&frame;
    for (i = 0; i < llsf->frameSize; ++i)
        *(*dest_p)++ = *frame8_p++;
    src_p = UNI_send->src;
    gRfuFixed->fastCopyPtr(&src_p, dest_p, UNI_send->payloadSize);
    if (gRfuLinkStatus->parentChild == 0x01)
        gRfuLinkStatus->LLFReadyFlag = 16;
    else
        gRfuLinkStatus->LLFReadyFlag |= 16 << bm_slot_id;
    return llsf->frameSize + UNI_send->payloadSize;
}
void rfu_REQ_recvData(void)
{
    if (gRfuLinkStatus->parentChild != 0xff)
    {
        gRfuStatic->commExistFlag = gRfuLinkStatus->sendSlotNIFlag | gRfuLinkStatus->recvSlotNIFlag | gRfuLinkStatus->sendSlotUNIFlag;
        gRfuStatic->recvErrorFlag = 0;
        STWI_set_Callback_M(rfu_CB_recvData);
        STWI_send_DataRxREQ();
    }
}
void rfu_CB_recvData(u8 reqCommand, u16 reqResult)
{
    u8 i;
    struct RfuSlotStatusNI *slotStatusNI;
    struct NIComm *NI_comm;
    if (reqResult == 0 && gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[1])
    {
        gRfuStatic->NIEndRecvFlag = 0;
        if (gRfuLinkStatus->parentChild == 0x01)
            rfu_STC_PARENT_analyzeRecvPacket();
        else
            rfu_STC_CHILD_analyzeRecvPacket();
        for (i = 0; i < 4; ++i)
        {
            slotStatusNI = gRfuSlotStatusNI[i];
            if (slotStatusNI->recv.state == (0x8000 | 0x0040 | 0x0003) && !((gRfuStatic->NIEndRecvFlag >> i) & 1))
            {
                NI_comm = &slotStatusNI->recv;
                if (NI_comm->dataType == 1)
                    gRfuLinkStatus->getNameFlag |= 1 << i;
                rfu_STC_releaseFrame(i, 1, NI_comm);
                gRfuLinkStatus->recvSlotNIFlag &= ~NI_comm->bmSlot;
                slotStatusNI->recv.state = ( 0x0040 | 0x006);
            }
        }
        if (gRfuStatic->recvErrorFlag)
            reqResult = gRfuStatic->recvErrorFlag | 0x0700;
    }
    rfu_STC_REQ_callback(reqCommand, reqResult);
}
void rfu_STC_PARENT_analyzeRecvPacket(void)
{
    u32 frames32;
    u8 bm_slot_id;
    u8 frame_counts[4];
    u8 *packet_p;
    frames32 = gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket32.data[0] >> 8;
    for (bm_slot_id = 0; bm_slot_id < 4; ++bm_slot_id)
    {
        frame_counts[bm_slot_id] = frames32 & 0x1F;
        frames32 >>= 5;
        if (frame_counts[bm_slot_id] == 0)
            gRfuStatic->NIEndRecvFlag |= 1 << bm_slot_id;
    }
    packet_p = &gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[8];
    for (bm_slot_id = 0; bm_slot_id < 4; ++bm_slot_id)
    {
        if (frame_counts[bm_slot_id])
        {
            u8 *frames_p = &frame_counts[bm_slot_id];
            do
            {
                u8 analyzed_frames = rfu_STC_analyzeLLSF(bm_slot_id, packet_p, *frames_p);
                packet_p += analyzed_frames;
                *frames_p -= analyzed_frames;
            } while (!(*frames_p & 0x80) && (*frames_p));
        }
    }
}
void rfu_STC_CHILD_analyzeRecvPacket(void)
{
    u16 frames_remaining;
    u8 *packet_p;
    u16 analyzed_frames;
    frames_remaining = *(u16 *)&gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[4] & 0x7F;
    packet_p = &gRfuFixed->STWIBuffer->rxPacketAlloc.rfuPacket8.data[8];
    if (frames_remaining == 0)
        gRfuStatic->NIEndRecvFlag = 15;
    do
    {
        if (frames_remaining == 0)
            break;
        analyzed_frames = rfu_STC_analyzeLLSF(0, packet_p, frames_remaining);
        packet_p += analyzed_frames;
        frames_remaining -= analyzed_frames;
    } while (!(frames_remaining & 0x8000));
}
u16 rfu_STC_analyzeLLSF(u8 slot_id, const u8 *src, u16 last_frame)
{
    struct RfuLocalStruct llsf_NI;
    const struct LLSFStruct *llsf_p;
    u32 frames;
    u8 i;
    u16 retVal;
    llsf_p = &llsf_struct[~gRfuLinkStatus->parentChild & (0xff & 0x01)];
    if (last_frame < llsf_p->frameSize)
        return last_frame;
    frames = 0;
    for (i = 0; i < llsf_p->frameSize; ++i)
        frames |= *src++ << 8 * i;
    llsf_NI.recvFirst = (frames >> llsf_p->recvFirstShift) & llsf_p->recvFirstMask;
    llsf_NI.connSlotFlag = (frames >> llsf_p->connSlotFlagShift) & llsf_p->connSlotFlagMask;
    llsf_NI.slotState = (frames >> llsf_p->slotStateShift) & llsf_p->slotStateMask;
    llsf_NI.ack = (frames >> llsf_p->ackShift) & llsf_p->ackMask;
    llsf_NI.phase = (frames >> llsf_p->phaseShift) & llsf_p->phaseMask;
    llsf_NI.n = (frames >> llsf_p->nShift) & llsf_p->nMask;
    llsf_NI.frame = (frames & llsf_p->framesMask) & frames;
    retVal = llsf_NI.frame + llsf_p->frameSize;
    if (llsf_NI.recvFirst == 0)
    {
        if (gRfuLinkStatus->parentChild == 0x01)
        {
            if ((gRfuLinkStatus->connSlotFlag >> slot_id) & 1)
            {
                if (llsf_NI.slotState == 0x0004)
                {
                    rfu_STC_UNI_receive(slot_id, &llsf_NI, src);
                }
                else if (llsf_NI.ack == 0)
                {
                    rfu_STC_NI_receive_Receiver(slot_id, &llsf_NI, src);
                }
                else
                {
                    for (i = 0; i < 4; ++i)
                        if (((gRfuSlotStatusNI[i]->send.bmSlot >> slot_id) & 1)
                         && ((gRfuLinkStatus->sendSlotNIFlag >> slot_id) & 1))
                            break;
                    if (i < 4)
                        rfu_STC_NI_receive_Sender(i, slot_id, &llsf_NI, src);
                }
            }
        }
        else
        {
            s32 conSlots = gRfuLinkStatus->connSlotFlag & llsf_NI.connSlotFlag;
            if (conSlots)
            {
                for (i = 0; i < 4; ++i)
                {
                    if ((conSlots >> i) & 1)
                    {
                        if (llsf_NI.slotState == 0x0004)
                            rfu_STC_UNI_receive(i, &llsf_NI, src);
                        else if (llsf_NI.ack == 0)
                            rfu_STC_NI_receive_Receiver(i, &llsf_NI, src);
                        else if ((gRfuLinkStatus->sendSlotNIFlag >> i) & 1)
                            rfu_STC_NI_receive_Sender(i, i, &llsf_NI, src);
                    }
                }
            }
        }
    }
    return retVal;
}
void rfu_STC_UNI_receive(u8 bm_slot_id, const struct RfuLocalStruct *llsf_NI, const u8 *src)
{
    u8 *dest;
    u32 size;
    struct RfuSlotStatusUNI *slotStatusUNI = gRfuSlotStatusUNI[bm_slot_id];
    struct UNIRecv *UNI_recv = &slotStatusUNI->recv;
    UNI_recv->errorCode = 0;
    if (gRfuSlotStatusUNI[bm_slot_id]->recvBufferSize < llsf_NI->frame)
    {
        slotStatusUNI->recv.state = ( 0x0040 | 0x009);
        UNI_recv->errorCode = (0x0700 | 0x0001);
    }
    else
    {
        if (UNI_recv->dataBlockFlag)
        {
            if (UNI_recv->newDataFlag)
            {
                UNI_recv->errorCode = (0x0700 | 0x0001 | 0x0008);
                goto force_tail_merge;
            }
        }
        else
        {
            if (UNI_recv->newDataFlag)
                UNI_recv->errorCode = (0x0700 | 0x0008);
        }
        UNI_recv->state = (0x8000 | 0x0040 | 0x0002);
        size = UNI_recv->dataSize = llsf_NI->frame;
        dest = gRfuSlotStatusUNI[bm_slot_id]->recvBuffer;
        gRfuFixed->fastCopyPtr(&src, &dest, size);
        UNI_recv->newDataFlag = 1;
        UNI_recv->state = 0;
    }
force_tail_merge:
    if (UNI_recv->errorCode)
        gRfuStatic->recvErrorFlag |= 16 << bm_slot_id;
}
void rfu_STC_NI_receive_Sender(u8 NI_slot, u8 bm_flag, const struct RfuLocalStruct *llsf_NI, const u8 *data_p)
{
    struct NIComm *NI_comm = &gRfuSlotStatusNI[NI_slot]->send;
    u16 state = NI_comm->state;
    u8 n = NI_comm->n[llsf_NI->phase];
    u8 i;
    u16 imeBak;
    if ((llsf_NI->slotState == 0x0002 && state == (0x8000 | 0x0020 | 0x0002))
     || (llsf_NI->slotState == 0x0001 && state == (0x8000 | 0x0020 | 0x0001))
     || (llsf_NI->slotState == 0x0003 && state == (0x8000 | 0x0020 | 0x0003)))
    {
        if (NI_comm->n[llsf_NI->phase] == llsf_NI->n)
            NI_comm->recvAckFlag[llsf_NI->phase] |= 1 << bm_flag;
    }
    if ((NI_comm->recvAckFlag[llsf_NI->phase] & NI_comm->bmSlot) == NI_comm->bmSlot)
    {
        NI_comm->n[llsf_NI->phase] = (NI_comm->n[llsf_NI->phase] + 1) & 3;
        NI_comm->recvAckFlag[llsf_NI->phase] = 0;
        if ((u16)(NI_comm->state + ~(0x8000 | 0x0020 | 0x0000)) <= 1)
        {
            if (NI_comm->state == (0x8000 | 0x0020 | 0x0001))
                NI_comm->now_p[llsf_NI->phase] += NI_comm->payloadSize;
            else
                NI_comm->now_p[llsf_NI->phase] += NI_comm->payloadSize << 2;
            NI_comm->remainSize -= NI_comm->payloadSize;
            switch (NI_comm->remainSize)
            {
            default:
            case 0:
                NI_comm->phase = 0;
                if (NI_comm->state == (0x8000 | 0x0020 | 0x0001))
                {
                    for (i = 0; i < 4; ++i)
                    {
                        NI_comm->n[i] = 1;
                        NI_comm->now_p[i] = NI_comm->src + NI_comm->payloadSize * i;
                    }
                    NI_comm->remainSize = NI_comm->dataSize;
                    NI_comm->state = (0x8000 | 0x0020 | 0x0002);
                }
                else
                {
                    NI_comm->n[0] = 0;
                    NI_comm->remainSize = 0;
                    NI_comm->state = (0x8000 | 0x0020 | 0x0003);
                }
                break;
            case 1 ... 2147483647:
                break;
            }
        }
        else if (NI_comm->state == (0x8000 | 0x0020 | 0x0003))
        {
            NI_comm->state = (0x8000 | 0x0020 | 0x0000);
        }
    }
    if (NI_comm->state != state
     || NI_comm->n[llsf_NI->phase] != n
     || (NI_comm->recvAckFlag[llsf_NI->phase] >> bm_flag) & 1)
    {
        imeBak = (*(vu16 *)(0x4000000 + 0x208));
        (*(vu16 *)(0x4000000 + 0x208)) = 0;
        gRfuStatic->recvRenewalFlag |= 16 << bm_flag;
        gRfuSlotStatusNI[bm_flag]->send.failCounter = 0;
        (*(vu16 *)(0x4000000 + 0x208)) = imeBak;
    }
}
void rfu_STC_NI_receive_Receiver(u8 bm_slot_id, const struct RfuLocalStruct *llsf_NI, const u8 *data_p)
{
    u16 imeBak;
    u32 state_check = 0;
    struct RfuSlotStatusNI *slotStatus_NI = gRfuSlotStatusNI[bm_slot_id];
    struct NIComm *recvSlot = &slotStatus_NI->recv;
    u16 state = slotStatus_NI->recv.state;
    u8 n = slotStatus_NI->recv.n[llsf_NI->phase];
    if (llsf_NI->slotState == 0x0003)
    {
        gRfuStatic->NIEndRecvFlag |= 1 << bm_slot_id;
        if (slotStatus_NI->recv.state == (0x8000 | 0x0040 | 0x0002))
        {
            slotStatus_NI->recv.phase = 0;
            slotStatus_NI->recv.n[0] = 0;
            slotStatus_NI->recv.state = (0x8000 | 0x0040 | 0x0003);
        }
    }
    else if (llsf_NI->slotState == 0x0002)
    {
        if (state == (0x8000 | 0x0040 | 0x0001) && !recvSlot->remainSize)
            rfu_STC_NI_initSlot_asRecvDataEntity(bm_slot_id, recvSlot);
        if (recvSlot->state == (0x8000 | 0x0040 | 0x0002))
            state_check = 1;
    }
    else if (llsf_NI->slotState == 0x0001)
    {
        if (state == (0x8000 | 0x0040 | 0x0001))
        {
            state_check = 1;
        }
        else
        {
            rfu_STC_NI_initSlot_asRecvControllData(bm_slot_id, recvSlot);
            if (slotStatus_NI->recv.state != (0x8000 | 0x0040 | 0x0001))
                return;
            state_check = 1;
        }
    }
    if (state_check != 0)
    {
        if (llsf_NI->n == ((recvSlot->n[llsf_NI->phase] + 1) & 3))
        {
            gRfuFixed->fastCopyPtr(&data_p, (u8 **)&recvSlot->now_p[llsf_NI->phase], llsf_NI->frame);
            if (recvSlot->state == (0x8000 | 0x0040 | 0x0002))
                recvSlot->now_p[llsf_NI->phase] += 3 * recvSlot->payloadSize;
            recvSlot->remainSize -= llsf_NI->frame;
            recvSlot->n[llsf_NI->phase] = llsf_NI->n;
        }
    }
    if (recvSlot->errorCode == 0)
    {
        recvSlot->phase = llsf_NI->phase;
        if (recvSlot->state != state || recvSlot->n[llsf_NI->phase] != n || recvSlot->n[llsf_NI->phase] == llsf_NI->n)
        {
            imeBak = (*(vu16 *)(0x4000000 + 0x208));
            (*(vu16 *)(0x4000000 + 0x208)) = 0;
            gRfuStatic->recvRenewalFlag |= 1 << bm_slot_id;
            recvSlot->failCounter = 0;
            (*(vu16 *)(0x4000000 + 0x208)) = imeBak;
        }
    }
}
void rfu_STC_NI_initSlot_asRecvControllData(u8 bm_slot_id, struct NIComm *NI_comm)
{
    u8 *llFrameSize_p;
    u32 llFrameSize;
    u8 bm_slot_flag;
    if (gRfuLinkStatus->parentChild == 0x01)
    {
        llFrameSize = 3;
        llFrameSize_p = &gRfuLinkStatus->remainLLFrameSizeParent;
    }
    else
    {
        llFrameSize = 2;
        llFrameSize_p = &gRfuLinkStatus->remainLLFrameSizeChild[bm_slot_id];
    }
    bm_slot_flag = 1 << bm_slot_id;
    if (NI_comm->state == 0)
    {
        if (*llFrameSize_p < llFrameSize)
        {
            NI_comm->state = ( 0x0040 | 0x009);
            NI_comm->errorCode = (0x0700 | 0x0002);
            gRfuStatic->recvErrorFlag |= bm_slot_flag;
        }
        else
        {
            NI_comm->errorCode = 0;
            *llFrameSize_p -= llFrameSize;
            NI_comm->now_p[0] = &NI_comm->dataType;
            NI_comm->remainSize = 7;
            NI_comm->ack = 1;
            NI_comm->payloadSize = 0;
            NI_comm->bmSlot = bm_slot_flag;
            NI_comm->state = (0x8000 | 0x0040 | 0x0001);
            gRfuLinkStatus->recvSlotNIFlag |= bm_slot_flag;
        }
    }
}
void rfu_STC_NI_initSlot_asRecvDataEntity(u8 bm_slot_id, struct NIComm *NI_comm)
{
    u8 bm_slot_flag, win_id;
    if (NI_comm->dataType == 1)
    {
        NI_comm->now_p[0] = (void *)&gRfuLinkStatus->partner[bm_slot_id].serialNo;
    }
    else
    {
        if (NI_comm->dataSize > gRfuSlotStatusNI[bm_slot_id]->recvBufferSize)
        {
            bm_slot_flag = 1 << bm_slot_id;
            gRfuStatic->recvErrorFlag |= bm_slot_flag;
            gRfuLinkStatus->recvSlotNIFlag &= ~bm_slot_flag;
            NI_comm->errorCode = (0x0700 | 0x0001);
            NI_comm->state = ( 0x0040 | 0x007);
            rfu_STC_releaseFrame(bm_slot_id, 1, NI_comm);
            return;
        }
        NI_comm->now_p[0] = gRfuSlotStatusNI[bm_slot_id]->recvBuffer;
    }
    for (win_id = 0; win_id < 4; ++win_id)
    {
        NI_comm->n[win_id] = 0;
        NI_comm->now_p[win_id] = &NI_comm->now_p[0][NI_comm->payloadSize * win_id];
    }
    NI_comm->remainSize = NI_comm->dataSize;
    NI_comm->state = (0x8000 | 0x0040 | 0x0002);
}
void rfu_NI_checkCommFailCounter(void)
{
    u16 imeBak;
    u32 recvRenewalFlag;
    u8 bm_slot_flag, bm_slot_id;
    if (gRfuLinkStatus->sendSlotNIFlag | gRfuLinkStatus->recvSlotNIFlag)
    {
        imeBak = (*(vu16 *)(0x4000000 + 0x208));
        (*(vu16 *)(0x4000000 + 0x208)) = 0;
        recvRenewalFlag = gRfuStatic->recvRenewalFlag >> 4;
        for (bm_slot_id = 0; bm_slot_id < 4; ++bm_slot_id)
        {
            bm_slot_flag = 1 << bm_slot_id;
            if (gRfuLinkStatus->sendSlotNIFlag & bm_slot_flag
             && !(gRfuStatic->recvRenewalFlag & bm_slot_flag))
                ++gRfuSlotStatusNI[bm_slot_id]->send.failCounter;
            if (gRfuLinkStatus->recvSlotNIFlag & bm_slot_flag
             && !(recvRenewalFlag & bm_slot_flag))
                ++gRfuSlotStatusNI[bm_slot_id]->recv.failCounter;
        }
        gRfuStatic->recvRenewalFlag = 0;
        (*(vu16 *)(0x4000000 + 0x208)) = imeBak;
    }
}
void rfu_REQ_noise(void)
{
    STWI_set_Callback_M(rfu_STC_REQ_callback);
    STWI_send_TestModeREQ(1, 0);
}
