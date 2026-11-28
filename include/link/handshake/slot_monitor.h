#ifndef GUARD_INCLUDE_LINK_HANDSHAKE_SLOT_MONITOR_H
#define GUARD_INCLUDE_LINK_HANDSHAKE_SLOT_MONITOR_H

#include "global.h"

struct SlotMonitor {
    u8 pad0[0x18];
    s32 timeout;
    u32 param;
    u32 f20;       /* confidence=low */
    s32 result;    /* confidence=low */
    u32 succeeded; /* confidence=low */
    u32 finished;  /* confidence=low */
    u32 f30;
    s32 elapsed;        /* confidence=low; counts toward timeout */
    s32 readyFrames;    /* confidence=low */
    s32 errorFrames;    /* confidence=low */
    s32 roleRetryTimer; /* confidence=low; frames without a serial role */
    s32 sendResult;     /* confidence=low */
    s32 receiveResult;  /* confidence=low */
    s32 field4c;        /* confidence=low */
    s32 retryTimer;
    s32 ticks;
    s32 ticks58; /* confidence=low */
    u16 idleTimer;
    u16 activeTimer;
    u16 flags;    /* confidence=low */
    u16 slots[4]; /* confidence=low */
    u16 limit;    /* confidence=low */
    u16 config;
    u8 restartPending; /* confidence=low */
    u8 mode;           /* confidence=low */
    u32 checkTimeouts; /* confidence=low */
    void (*onSuccess)(void *context, s32 role, s32 result, u32 mode);
    void (*onFailure)(void *context, s32 role, s32 result, u32 mode);
    void (*callback)(struct SlotMonitor *obj);
    void *context;
};

void LinkConnection_SetRetryOptions(struct SlotMonitor *obj, u32 options);
u8 SlotMonitor_CheckTimeout(struct SlotMonitor *obj);
u8 LinkHandshake_AdvanceTimeout(struct SlotMonitor *obj);
void LinkHandshake_UpdateRestartState(struct SlotMonitor *obj);
void SlotMonitor_PollResult(struct SlotMonitor *obj);
void sub_08108C94(struct SlotMonitor *obj);

typedef char SlotMonitorSizeCheck[sizeof(struct SlotMonitor) == 0x84 ? 1 : -1];

#endif
