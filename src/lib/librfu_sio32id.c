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

void Sio32IDIntr(void);
void Sio32IDInit(void);
s32 Sio32IDMain(void);

struct RfuSIO32Id
{
    u8 MS_mode;
    u8 state;
    u16 count;
    u16 send_id;
    u16 recv_id;
    u16 unk8; // unused
    u16 lastId;
};

extern struct RfuSIO32Id gRfuSIO32Id ;

extern const u16 Sio32ConnectionData[4]; // NINTENDO
extern const char Sio32IDLib_Var[];

s32 AgbRFU_checkID(u8 maxTries)
{
    u16 ieBak;
    vu16 *regTMCNTL;
    s32 id;

    // Interrupts must be enabled
    if (REG_IME == 0)
        return -1;
    ieBak = REG_IE;
    gSTWIStatus->state = 10;
    STWI_set_Callback_ID(Sio32IDIntr);
    Sio32IDInit();
    regTMCNTL = &REG_TMCNT_L(gSTWIStatus->timerSelect);
    maxTries *= 8;
    while (--maxTries != 0xFF)
    {
        id = Sio32IDMain();
        if (id != 0)
            break;
        regTMCNTL[1] = 0;
        regTMCNTL[0] = 0;
        regTMCNTL[1] = TIMER_1024CLK | TIMER_ENABLE;
        while (regTMCNTL[0] < 32)
            ;
        regTMCNTL[1] = 0;
        regTMCNTL[0] = 0;
    }
    REG_IME = 0;
    REG_IE = ieBak;
    REG_IME = 1;
    gSTWIStatus->state = 0;
    STWI_set_Callback_ID(NULL);
    return id;
}

void Sio32IDInit(void)
{
    REG_IME = 0;
    REG_IE &= ~((8 << gSTWIStatus->timerSelect) | INTR_FLAG_SERIAL);
    REG_IME = 1;
    REG_RCNT = 0;
    REG_SIOCNT = SIO_32BIT_MODE;
    REG_SIOCNT |= SIO_INTR_ENABLE | SIO_ENABLE;
    CpuFill32(0, &gRfuSIO32Id, sizeof(struct RfuSIO32Id));
    REG_IF = INTR_FLAG_SERIAL;
}

s32 Sio32IDMain(void)
{
    switch (gRfuSIO32Id.state)
    {
    case 0:
        gRfuSIO32Id.MS_mode = AGB_CLK_MASTER;
        REG_SIOCNT |= SIO_38400_BPS;
        REG_IME = 0;
        REG_IE |= INTR_FLAG_SERIAL;
        REG_IME = 1;
        gRfuSIO32Id.state = 1;
        *(vu8 *)&REG_SIOCNT |= SIO_ENABLE;
        break;
    case 1:
        if (gRfuSIO32Id.lastId == 0)
        {
            if (gRfuSIO32Id.MS_mode == AGB_CLK_MASTER)
            {
                if (gRfuSIO32Id.count == 0)
                {
                    REG_IME = 0;
                    REG_SIOCNT |= SIO_ENABLE;
                    REG_IME = 1;
                }
            }
            else if (gRfuSIO32Id.send_id != RFU_ID && !gRfuSIO32Id.count)
            {
                REG_IME = 0;
                REG_IE &= ~INTR_FLAG_SERIAL;
                REG_IME = 1;
                REG_SIOCNT = 0;
                REG_SIOCNT = SIO_32BIT_MODE;
                REG_IF = INTR_FLAG_SERIAL;
                REG_SIOCNT |= SIO_INTR_ENABLE | SIO_ENABLE;
                REG_IME = 0;
                REG_IE |= INTR_FLAG_SERIAL;
                REG_IME = 1;
            }
            break;
        }
        else
        {
            gRfuSIO32Id.state = 2;
            // fallthrough
        }
    default:
        return gRfuSIO32Id.lastId;
    }
    return 0;
}

void Sio32IDIntr(void)
{
    u32 regSIODATA32;
    u16 delay;
    u32 rfuSIO32IdUnk0_times_16;

    regSIODATA32 = REG_SIODATA32;
    if (gRfuSIO32Id.MS_mode != AGB_CLK_MASTER)
        REG_SIOCNT |= SIO_ENABLE;
    rfuSIO32IdUnk0_times_16 = (regSIODATA32 << (16 * gRfuSIO32Id.MS_mode)) >> 16;
    regSIODATA32 = (regSIODATA32 << 16 * (1 - gRfuSIO32Id.MS_mode)) >> 16;
    if (gRfuSIO32Id.lastId == 0)
    {
        u16 backup = rfuSIO32IdUnk0_times_16;
        if (backup == gRfuSIO32Id.recv_id)
        {
            if (gRfuSIO32Id.count < 4)
            {
                backup = (u16)~gRfuSIO32Id.send_id;
                if (gRfuSIO32Id.recv_id == backup)
                {
                    if (regSIODATA32 == (u16)~gRfuSIO32Id.recv_id)
                        ++gRfuSIO32Id.count;
                }
            }
            else
            {
                gRfuSIO32Id.lastId = regSIODATA32;
            }
        }
        else
        {
            gRfuSIO32Id.count = 0;
        }
    }
    if (gRfuSIO32Id.count < 4)
        gRfuSIO32Id.send_id = *(gRfuSIO32Id.count + Sio32ConnectionData);
    else
        gRfuSIO32Id.send_id = RFU_ID;
    gRfuSIO32Id.recv_id = ~regSIODATA32;
    REG_SIODATA32 = (gRfuSIO32Id.send_id << 16 * (1 - gRfuSIO32Id.MS_mode))
                  + (gRfuSIO32Id.recv_id << 16 * gRfuSIO32Id.MS_mode);
    if (gRfuSIO32Id.MS_mode == AGB_CLK_MASTER && (gRfuSIO32Id.count != 0 || regSIODATA32 == 0x494e))
    {
        for (delay = 0; delay < 600; ++delay)
            ;
        if (gRfuSIO32Id.lastId == 0)
            REG_SIOCNT |= SIO_ENABLE;
    }
}
