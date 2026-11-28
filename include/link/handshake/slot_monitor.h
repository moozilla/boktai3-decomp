#ifndef GUARD_INCLUDE_LINK_HANDSHAKE_SLOT_MONITOR_H
#define GUARD_INCLUDE_LINK_HANDSHAKE_SLOT_MONITOR_H

#include "global.h"

struct SlotMonitor {
    u8 pad0[0x18];
    s32 timeout;
    u32 param;
    u32 f20;    /* confidence=low */
    s32 result; /* confidence=low */
    u32 f28;    /* confidence=low */
    u32 f2c;    /* confidence=low */
    u32 f30;
    s32 elapsed; /* confidence=low; counts toward timeout */
    u32 f38;     /* confidence=low */
    u32 f3c;     /* confidence=low */
    u32 f40;     /* confidence=low */
    s32 f44;     /* confidence=low */
    s32 f48;     /* confidence=low */
    s32 field4c; /* confidence=low */
    s32 retryTimer;
    s32 ticks;
    s32 ticks58; /* confidence=low */
    u16 idleTimer;
    u16 activeTimer;
    u16 flags;    /* confidence=low */
    u16 slots[4]; /* confidence=low */
    u16 limit;    /* confidence=low */
    u16 config;
    u8 unk6e; /* confidence=low */
    u8 mode;  /* confidence=low */
    u32 f70;  /* confidence=low */
    u32 f74;
    u32 f78;
    void (*callback)(void);
    u32 f80;
};

u8 LinkHandshake_AdvanceTimeout(struct SlotMonitor *obj);

typedef char SlotMonitorSizeCheck[sizeof(struct SlotMonitor) == 0x84 ? 1 : -1];

#endif
