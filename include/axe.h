#ifndef GUARD_AXE_H
#define GUARD_AXE_H

#include "chase_actor.h"
#include "sprite.h"
#include "display_sequence.h"
#include "contact_swarm.h"

/* One of the four afterimage sprites the axe leaves along its path (struct Axe + 0x708). */
struct AxeTrailSlot {
    struct DisplaySequenceSprite sprite;
    struct ContactSwarmGraphics graphics;
    struct DisplaySequenceState sequence;
    const void *resource;
    u16 lifetime, unknown5E;
};

/* Axe's class fields past the chase actor (Axe_Create allocates 0xF24 bytes; provisional). */
struct Axe {
    struct ChaseActor chase;
    u8 unknown624[0xE4];
    struct AxeTrailSlot trail[4];
    struct ContactPosition position; /* Legacy name for the start of the black effect palette. */
    u8 unknown890[0x78];
    u16 originalEffectPalette[16];
    struct ContactCollider auxiliaryAttack[3];
    struct ContactPosition trailPosition, trailVelocity;
    u8 trailAngle, trailHeading, trailSpinRate, unknownA37;
    struct DisplaySequenceSprite auxiliarySprite;
    struct SpriteRenderResource auxiliaryGraphics;
    struct DisplaySequenceState auxiliarySequence;
    struct ContactCollider attack[3];
    struct ContactCollider body[3];
    struct ContactPosition origin;
    u8 unknownC90[8];
    struct ContactPosition center;
    s16 radius;
    u8 unknownCA2[2];
    u8 speed;
    s8 direction;
    u8 heading;
    u8 shadowActive;
    u8 trailStartAngle;
    u8 angle;
    u8 unknownCAA;
    u8 flagsCAB;
    u8 auxiliaryShadow[0x40];
    struct ContactCollider extraAttack[2]; /* the axe actor's own pairs (Axe_InitColliders) */
    struct ContactCollider extraBody[2];
    u8 unknownE3C[2];
    u8 delay;
    u8 turnStep;
    u8 relocationDelay;
    u8 unknownE41[3];
    u16 nextTrail;   /* the trail slot the next afterimage uses */
    u16 trailSpacing; /* frames between afterimages */
    s32 healthThreshold;
    u32 flagsE4C;
    u8 unknownE50[6];
    u16 distance;
    u8 unknownE58[0x50];
    u16 curveTime; /* 0..1024 along the movement curve, +128 per step */
    u8 hoverPhase; /* index into the sine table for the hover bob, +2 per frame */
    u8 unknownEAB;
    s16 curve;
    u8 acceleration;
    u8 unknownEAF;
    u8 timerEB0;
    u8 unknownEB1;
    u8 parameterEB2;
    u8 unknownEB3;
    u16 lowHealthThreshold; /* the second attack threshold, below healthThreshold */
    s16 routeId;
    u16 attackCooldowns[4]; /* counted down each frame (Axe_TickCooldowns) */
    u16 attackDelays[4];    /* what a cooldown restarts from */
    u8 recentRoutePoints[3];
    u8 recentRoutePointIndex;
    u8 unknownECC[4];
    u8 active;
    u8 lungeQueued;
    u8 sparkStep;
    u8 savedAngle;
    u16 reactionHoldTimer; /* while set, reactionFlags 0x20 is held (Axe_UpdateDamageSpin) */
    u8 damageSpinStarted;
    u8 variant;
    u8 unknownED8[0x2C];
    struct ContactPosition curveStart, curveEnd; /* the movement curve's endpoints */
    u32 clearF14[2];
    struct ContactPosition target;
};

#endif // GUARD_AXE_H
