#ifndef GUARD_MUMMY_H
#define GUARD_MUMMY_H

#include "engine/shared/chase_actor.h"

/* Mummy's class fields past the chase actor, as far as its code reads them (provisional). */
struct Mummy {
    struct ChaseActor chase;
    u8 unknown624[0xA8];
    u16 field6CC;
    u16 currentTick;
    u16 targetTick;
    u16 pursuitDelay;
    u16 flags6D4;
    u16 savedTick;
    u32 limit;
    u32 field6DC;
    u8 unknown6E0[8];
    u16 field6E8;
    u8 interactionFrames; /* While nonzero, retains the active stimulus interaction. */
    u8 unknown6EB[3];
    u16 counter6EE;
    u8 unknown6F0[2];
    u16 flags6F2;
    struct ChaseRouteProbe routeProbe;
    struct ContactPosition destination;
};

#endif // GUARD_MUMMY_H
