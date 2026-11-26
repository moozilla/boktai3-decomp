#ifndef GUARD_VORN_H
#define GUARD_VORN_H

#include "monsters/chase_actor.h"

/* Vorn's class fields past the chase actor, as far as its code reads them (provisional). */
struct Vorn {
    struct ChaseActor chase;
    u16 field624;
    u8 field626;
    u8 field627;
    struct ContactCollider attackCollider;
    s16 response;
    s16 field67E;
    s16 field680;
    s16 field682;
    s32 field684;
    void *field688;
    u8 field68C; /* 1: Vorn starts hidden, in state 3, with its shadow off */
    u8 timer68D;
    u8 unknown68E[2];
    u16 randomReactionTimer, randomReactionPeriod;
    u16 delayedAnimationFlags;
    u8 unknown696, delayedAnimationTimer;
    u16 field698;
    u16 frame69A;
    u16 pursuitWaitTimer;
    u16 field69E;
    u8 unknown6A0[8];
    u16 nearTimer;
    u16 farTimer;
    u16 pursuitCooldown;
    u8 unknown6AE[2];
    struct ContactPosition uphillPosition;
    s8 uphillRouteId;
    u8 axis;
    u16 sideStepDuration;
    u16 uphillProbeDelay, uphillProbePeriod;
    u8 foreignPosition;
    u8 field6C1;
    u8 unknown6C2[2];
    u32 retreatRequests;
    u32 minimumDistance;
    u32 maximumDistance;
    u16 field6D0;
    u8 unknown6D2[2];
    u16 flags6D4;
    u16 field6D6;
    struct ContactPosition vector6D8;
    struct ContactPosition vector6E0;
    u16 field6E8;
    u8 unknown6EA[2];
    u16 field6EC;
    u8 unknown6EE[6];
    struct ChaseRouteProbe routeProbe;
    struct ContactPosition destination;
};

typedef char VornSize[(sizeof(struct Vorn) == 0x720) ? 1 : -1];

#endif // GUARD_VORN_H
