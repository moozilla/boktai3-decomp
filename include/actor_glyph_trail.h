#ifndef GUARD_ACTOR_GLYPH_TRAIL_H
#define GUARD_ACTOR_GLYPH_TRAIL_H

#include "global.h"

struct GlyphPosition {
    u16 x, y, z, pad;
} __attribute__((aligned(4)));

struct GlyphOwner {
    u8 pad0[0x30];
    struct GlyphPosition position;
    u8 pad38[0x36B];
    u8 side;
    u8 direction;
};

struct GlyphTrailParticle {
    u32 flags;
    u8 drawable[0x14];
    struct GlyphPosition position;
    u8 spriteTail[8];
    u8 active;
    s8 angle;
    u16 radius;
    u16 timer;
    u16 pad2E;
};

struct ActorGlyphTrail {
    u8 task[0x18];
    u8 *owner;
    u32 flags;
    u8 spriteBody[0xC];
    u16 spriteFrame;
    u8 spriteTail[0xA];
    struct GlyphPosition position;
    u8 animationPrefix[8];
    u8 animation[0x1C];
    u16 velocityX;
    u16 velocityY;
    u16 velocityZ;
    u16 pad6A;
    u16 speed;
    u8 direction;
    u8 state;
    u16 timer;
    u16 durationOrVariant;
    u8 pad74[3];
    u8 particleIndex;
    void *particleResource;
    struct GlyphTrailParticle particles[4];
    u32 phase;
};

#endif
