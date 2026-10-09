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
#include "rfu_compat.h"

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
    // If we're copying our interrupt into RAM, DMA it to block1 and use
    // block2 for our STWIStatus, otherwise block1 holds the STWIStatus.
    // interrupt usually is a pointer to gIntrTable[1]
    if (copyInterruptToRam == TRUE)
    {
        *interrupt = (IntrFunc)interruptStruct->block1;
        DmaCopy16(3, &IntrSIO32, interruptStruct->block1, sizeof(interruptStruct->block1));
        gSTWIStatus = &interruptStruct->block2;
    }
    else
    {
        *interrupt = IntrSIO32;
        gSTWIStatus = (struct STWIStatus *)interruptStruct->block1;
    }
    gSTWIStatus->rxPacket = &interruptStruct->rxPacketAlloc;
    gSTWIStatus->txPacket = &interruptStruct->txPacketAlloc;
    gSTWIStatus->msMode = AGB_CLK_MASTER;
    gSTWIStatus->state = 0; // master send req
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
    REG_RCNT = 0x100; // TODO: mystery bit?
    REG_SIOCNT = SIO_INTR_ENABLE | SIO_32BIT_MODE | SIO_115200_BPS;
    STWI_init_Callback_M();
    STWI_init_Callback_S();
    IntrEnable(INTR_FLAG_SERIAL);
}

void STWI_init_timer(IntrFunc *interrupt, s32 timerSelect)
{
    *interrupt = STWI_intr_timer;
    gSTWIStatus->timerSelect = timerSelect;
    IntrEnable(INTR_FLAG_TIMER0 << gSTWIStatus->timerSelect);
}

void AgbRFU_SoftReset(void)
{
    vu16 *timerL;
    vu16 *timerH;

    REG_RCNT = 0x8000;
    REG_RCNT = 0x80A0; // all these bits are undocumented
    timerL = &REG_TMCNT_L(gSTWIStatus->timerSelect);
    timerH = &REG_TMCNT_H(gSTWIStatus->timerSelect);
    *timerH = 0;
    *timerL = 0;
    *timerH = TIMER_ENABLE | TIMER_1024CLK;
    while (*timerL <= 0x11)
        REG_RCNT = 0x80A2;
    *timerH = 3;
    REG_RCNT = 0x80A0;
    REG_SIOCNT = SIO_INTR_ENABLE | SIO_32BIT_MODE | SIO_115200_BPS;
    gSTWIStatus->state = 0; // master send req
    gSTWIStatus->reqLength = 0;
    gSTWIStatus->reqNext = 0;
    gSTWIStatus->reqActiveCommand = 0;
    gSTWIStatus->ackLength = 0;
    gSTWIStatus->ackNext = 0;
    gSTWIStatus->ackActiveCommand = 0;
    gSTWIStatus->timerState = 0;
    gSTWIStatus->timerActive = 0;
    gSTWIStatus->error = 0;
    gSTWIStatus->msMode = AGB_CLK_MASTER;
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
    STWI_set_Callback_M(NULL);
}

void STWI_init_Callback_S(void)
{
    STWI_set_Callback_S(NULL);
}

// The callback can take 2 or 3 arguments.
void STWI_set_Callback_M(void *callbackM)
{
    gSTWIStatus->callbackM = callbackM;
}

void STWI_set_Callback_S(void (*callbackS)(u16))
{
    gSTWIStatus->callbackS = callbackS;
}

void STWI_set_Callback_ID(void (*func)(void)) // name in SDK, but is actually setting a function pointer
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
    if (!STWI_init(ID_RESET_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_LinkStatusREQ(void)
{
    if (!STWI_init(ID_LINK_STATUS_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_VersionStatusREQ(void)
{
    if (!STWI_init(ID_VERSION_STATUS_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_SystemStatusREQ(void)
{
    if (!STWI_init(ID_SYSTEM_STATUS_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_SlotStatusREQ(void)
{
    if (!STWI_init(ID_SLOT_STATUS_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_ConfigStatusREQ(void)
{
    if (!STWI_init(ID_CONFIG_STATUS_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_GameConfigREQ(const u8 *serial_gname, const u8 *uname)
{
    u8 *packetBytes;
    s32 i;

    if (!STWI_init(ID_GAME_CONFIG_REQ))
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
    if (!STWI_init(ID_SYSTEM_CONFIG_REQ))
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
    if (!STWI_init(ID_SC_START_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_SC_PollingREQ(void)
{
    if (!STWI_init(ID_SC_POLL_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_SC_EndREQ(void)
{
    if (!STWI_init(ID_SC_END_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_SP_StartREQ(void)
{
    if (!STWI_init(ID_SP_START_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_SP_PollingREQ(void)
{
    if (!STWI_init(ID_SP_POLL_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_SP_EndREQ(void)
{
    if (!STWI_init(ID_SP_END_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_CP_StartREQ(u16 unk1)
{
    if (!STWI_init(ID_CP_START_REQ))
    {
        gSTWIStatus->reqLength = 1;
        gSTWIStatus->txPacket->rfuPacket32.data[0] = unk1;
        STWI_start_Command();
    }
}

void STWI_send_CP_PollingREQ(void)
{
    if (!STWI_init(ID_CP_POLL_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_CP_EndREQ(void)
{
    if (!STWI_init(ID_CP_END_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_DataTxREQ(const void *in, u8 size)
{
    if (!STWI_init(ID_DATA_TX_REQ))
    {
        u8 reqLength = (size / sizeof(u32));
        if (size & (sizeof(u32) - 1))
            reqLength += 1;
        gSTWIStatus->reqLength = reqLength;
        CpuCopy32(in, gSTWIStatus->txPacket->rfuPacket32.data, gSTWIStatus->reqLength * sizeof(u32));
        STWI_start_Command();
    }
}

void STWI_send_DataTxAndChangeREQ(const void *in, u8 size)
{
    if (!STWI_init(ID_DATA_TX_AND_CHANGE_REQ))
    {
        u8 reqLength = (size / sizeof(u32));
        if (size & (sizeof(u32) - 1))
            reqLength += 1;
        gSTWIStatus->reqLength = reqLength;
        CpuCopy32(in, gSTWIStatus->txPacket->rfuPacket32.data, gSTWIStatus->reqLength * sizeof(u32));
        STWI_start_Command();
    }
}

void STWI_send_DataRxREQ(void)
{
    if (!STWI_init(ID_DATA_RX_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_MS_ChangeREQ(void)
{
    if (!STWI_init(ID_MS_CHANGE_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_DataReadyAndChangeREQ(u8 unk)
{
    if (!STWI_init(ID_DATA_READY_AND_CHANGE_REQ))
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
    if (!STWI_init(ID_DISCONNECTED_AND_CHANGE_REQ))
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
    if (!STWI_init(ID_RESUME_RETRANSMIT_AND_CHANGE_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_DisconnectREQ(u8 unk)
{
    if (!STWI_init(ID_DISCONNECT_REQ))
    {
        gSTWIStatus->reqLength = 1;
        gSTWIStatus->txPacket->rfuPacket32.data[0] = unk;
        STWI_start_Command();
    }
}

void STWI_send_TestModeREQ(u8 unk0, u8 unk1)
{
    if (!STWI_init(ID_TEST_MODE_REQ))
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

    if (!STWI_init(ID_CPR_START_REQ))
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
    if (!STWI_init(ID_CPR_POLL_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_CPR_EndREQ(void)
{
    if (!STWI_init(ID_CPR_END_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_send_StopModeREQ(void)
{
    if (!STWI_init(ID_STOP_MODE_REQ))
    {
        gSTWIStatus->reqLength = 0;
        STWI_start_Command();
    }
}

void STWI_intr_timer(void)
{
    switch (gSTWIStatus->timerState)
    {
    // TODO: Make an enum for these
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
        if (gSTWIStatus->callbackM != NULL)
            gSTWIStatus->callbackM(ID_CLOCK_SLAVE_MS_CHANGE_ERROR_BY_DMA_REQ, 0);
        break;
    }
}

void STWI_set_timer(u8 count)
{
    vu16 *timerL = &REG_TMCNT_L(gSTWIStatus->timerSelect);
    vu16 *timerH = &REG_TMCNT_H(gSTWIStatus->timerSelect);
    REG_IME = 0;
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
    *timerH = TIMER_ENABLE | TIMER_INTR_ENABLE | TIMER_1024CLK;
    REG_IF = INTR_FLAG_TIMER0 << gSTWIStatus->timerSelect;
    REG_IME = 1;
}

void STWI_stop_timer(void)
{
    gSTWIStatus->timerState = 0;
    REG_TMCNT_L(gSTWIStatus->timerSelect) = 0;
    REG_TMCNT_H(gSTWIStatus->timerSelect) = 0;
}

/*
 * Set up STWI to send REQ. Returns 1 if error (see below).
 */
u16 STWI_init(u8 request)
{
    if (!REG_IME)
    {
        // Can't start sending if IME is disabled.
        gSTWIStatus->error = ERR_REQ_CMD_IME_DISABLE;
        if (gSTWIStatus->callbackM != NULL)
            gSTWIStatus->callbackM(request, gSTWIStatus->error);
        return TRUE;
    }
    else if (gSTWIStatus->sending == 1)
    {
        // Already sending something. Cancel and error.
        gSTWIStatus->error = ERR_REQ_CMD_SENDING;
        gSTWIStatus->sending = 0;
        if (gSTWIStatus->callbackM != NULL)
            gSTWIStatus->callbackM(request, gSTWIStatus->error);
        return TRUE;
    }
    else if (gSTWIStatus->msMode == AGB_CLK_SLAVE)
    {
        // Can't send if clock slave
        gSTWIStatus->error = ERR_REQ_CMD_CLOCK_SLAVE;
        if (gSTWIStatus->callbackM != NULL)
            gSTWIStatus->callbackM(request, gSTWIStatus->error, gSTWIStatus);
        return TRUE;
    }
    else
    {
        // Good to go, start sending
        gSTWIStatus->sending = 1;
        gSTWIStatus->reqActiveCommand = request;
        gSTWIStatus->state = 0; // master send req
        gSTWIStatus->reqLength = 0;
        gSTWIStatus->reqNext = 0;
        gSTWIStatus->ackLength = 0;
        gSTWIStatus->ackNext = 0;
        gSTWIStatus->ackActiveCommand = 0;
        gSTWIStatus->timerState = 0;
        gSTWIStatus->timerActive = 0;
        gSTWIStatus->error = 0;
        gSTWIStatus->recoveryCount = 0;
        REG_RCNT = 0x100;
        REG_SIOCNT = SIO_INTR_ENABLE | SIO_32BIT_MODE | SIO_115200_BPS;
        return FALSE;
    }
}

s32 STWI_start_Command(void)
{
    u16 imeTemp;

    // equivalent to gSTWIStatus->txPacket->rfuPacket32.command,
    // but the cast here is required to avoid register issue
    *(u32 *)gSTWIStatus->txPacket->rfuPacket8.data = 0x99660000 | (gSTWIStatus->reqLength << 8) | gSTWIStatus->reqActiveCommand;
    REG_SIODATA32 = gSTWIStatus->txPacket->rfuPacket32.command;
    gSTWIStatus->state = 0; // master send req
    gSTWIStatus->reqNext = 1;
    imeTemp = REG_IME;
    REG_IME = 0;
    REG_IE |= (INTR_FLAG_TIMER0 << gSTWIStatus->timerSelect);
    REG_IE |= INTR_FLAG_SERIAL;
    REG_IME = imeTemp;
    REG_SIOCNT = SIO_INTR_ENABLE | SIO_32BIT_MODE | SIO_MULTI_BUSY | SIO_115200_BPS;
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
        if (gSTWIStatus->reqActiveCommand == ID_MS_CHANGE_REQ || gSTWIStatus->reqActiveCommand == ID_DATA_TX_AND_CHANGE_REQ || gSTWIStatus->reqActiveCommand == ID_UNK35_REQ || gSTWIStatus->reqActiveCommand == ID_RESUME_RETRANSMIT_AND_CHANGE_REQ)
        {
            gSTWIStatus->error = ERR_REQ_CMD_CLOCK_DRIFT;
            gSTWIStatus->sending = 0;
            if (gSTWIStatus->callbackM != NULL)
                gSTWIStatus->callbackM(gSTWIStatus->reqActiveCommand, gSTWIStatus->error);
        }
        else
        {
            gSTWIStatus->error = ERR_REQ_CMD_CLOCK_DRIFT;
            gSTWIStatus->sending = 0;
            if (gSTWIStatus->callbackM != NULL)
                gSTWIStatus->callbackM(gSTWIStatus->reqActiveCommand, gSTWIStatus->error);
            gSTWIStatus->state = 4; // error
        }
    }
    return 0;
}

s32 STWI_reset_ClockCounter(void)
{
    gSTWIStatus->state = 5; // slave receive req init
    gSTWIStatus->reqLength = 0;
    gSTWIStatus->reqNext = 0;
    REG_SIODATA32 = (1 << 31);
    REG_SIOCNT = 0;
    REG_SIOCNT = SIO_INTR_ENABLE | SIO_32BIT_MODE | SIO_115200_BPS;
    REG_SIOCNT = (SIO_INTR_ENABLE | SIO_32BIT_MODE | SIO_115200_BPS) + 0x7F;
    return 0;
}
