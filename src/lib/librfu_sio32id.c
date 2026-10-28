/* Adapted from pret/pokeemerald 731ad5bfd6e6f265508d0efcca0ba42f9dcf5881, LIBRFU_VERSION 1024.
 * Reconstructed SDK source retains upstream licensing status; see THIRD_PARTY.md and docs/RFU.md. */
// CFLAGS: -O2 -mthumb-interwork
#define STWI_send_ConfigStatusREQ sub_08244250
#define STWI_send_DataReadyAndChangeREQ sub_08244534
#define STWI_send_DisconnectedAndChangeREQ sub_08244584
#define STWI_send_VersionStatusREQ sub_082441E4
#define STWI_set_MS_mode sub_082440C4
#define rfu_MBOOT_CHILD_inheritanceLinkStatus sub_08245744
#define rfu_REQ_noise sub_0824826C
#define IntrSIO32 sub_08244A38
#define llsf_struct gUnk_08602CD0
#define str_checkMbootLL gUnk_08602CFC
#define Sio32ConnectionData gUnk_08602D08
#include "rfu_compat.h"

void Sio32IDIntr(void);
void Sio32IDInit(void);
s32 Sio32IDMain(void);


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
