#ifndef GUARD_RFU_MANAGER_H
#define GUARD_RFU_MANAGER_H

#include "global.h"

/* SDK manager prefix reconciled with the game's callback/configuration tail. */
struct RfuVBlankTimer {
    u8 active, unk01;
    u16 count_max;
    u16 count[4];
};

union RfuEventParameters {
    struct { u8 bits, unk01; u16 value; } bytes;
    struct { u16 code, value; } words;
    u16 param[2];
};

struct RfuConfig;
struct RfuLinkManager {
    u8 acceptSlot_flag, acceptCount;
    vu8 childClockSlave_flag, parentAck_flag;
    u8 state, next_state, parent_child, pcswitch_flag;
    u8 RFU_powerOn_flag, linkRecovery_enable, linkRecovery_start_flag, fastSearchParent_flag;
    u8 connectSlot_flag_old, reserveDisconnectSlot_flag, active, msc_exe_flag;
    u8 child_slot, state_bak[2], unk13;
    union RfuEventParameters event;
    u16 NI_failCounter_limit, connect_period, pcswitch_period_bak, work;
    const u16 *acceptable_serialNo_list;
    struct RfuVBlankTimer nameAcceptTimer, linkRecoveryTimer;
    struct RfuConfig *config;
    u32 unk40, unk44;
};

extern struct RfuLinkManager gRfuLinkManager;

#endif
