#ifndef GUARD_AXE_H
#define GUARD_AXE_H

#include "chase_actor.h"
#include "sprite.h"
#include "display_sequence.h"

/* Axe's class fields past the chase actor (Axe_Create allocates 0xF24 bytes; provisional). */
struct Axe {
    struct ChaseActor chase;
    u8 unknown624[0x264];
    struct ContactPosition position;
    u8 unknown890[0x1A8];
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
    u8 unknownCA8;
    u8 angle;
    u8 unknownCAA;
    u8 flagsCAB;
    u8 auxiliaryShadow[0x40];
    u8 unknownCEC[0x152];
    u8 delay;
    u8 turnStep;
    u8 unknownE40[8];
    s32 healthThreshold;
    u32 flagsE4C;
    u8 unknownE50[6];
    u16 distance;
    u8 unknownE58[0x56];
    u8 acceleration;
    u8 unknownEAF;
    u8 timerEB0;
    u8 unknownEB1;
    u8 parameterEB2;
    u8 unknownEB3[7];
    u16 fieldEBA;
    u8 unknownEBC[6];
    u16 fieldEC2;
    u8 unknownEC4[0xC];
    u8 active;
    u8 unknownED1[2];
    u8 savedAngle;
    u8 unknownED4[3];
    u8 variant;
    u8 unknownED8[0x3C];
    u32 clearF14[2];
    struct ContactPosition target;
};

#endif // GUARD_AXE_H
