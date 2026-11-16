#ifndef GUARD_VORN_H
#define GUARD_VORN_H

#include "chase_actor.h"

/* Vorn's class fields past the chase actor, as far as its code reads them (provisional). */
struct Vorn {
    struct ChaseActor chase;
    u8 unknown624[0x58];
    s16 field67C;
    u8 unknown67E[4];
    s16 field682;
    u8 unknown684[4];
    void *field688;
    u8 field68C; /* 1: Vorn starts hidden, in state 3, with its shadow off */
    u8 timer68D;
    u8 unknown68E[0xA];
    u16 field698;
    u16 frame69A;
    u8 unknown69C[2];
    u16 field69E;
    u8 unknown6A0[0xC];
    u16 field6AC;
    u8 unknown6AE[0xB];
    u8 field6B9;
    u8 unknown6BA[0x16];
    u16 field6D0;
    u8 unknown6D2[2];
    u16 flags6D4;
    u16 field6D6;
    struct ContactPosition vector6D8;
    struct ContactPosition vector6E0;
    u16 field6E8;
    u8 unknown6EA[0xA];
    struct ChaseRouteProbe routeProbe;
    struct ContactPosition destination;
};

#endif // GUARD_VORN_H
