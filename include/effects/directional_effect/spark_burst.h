#ifndef GUARD_DIRECTIONAL_SPARK_BURST_H
#define GUARD_DIRECTIONAL_SPARK_BURST_H

#include "global.h"
#include "engine/video/sprite_drawable_api.h"
#include "player/player.h"

struct DirectionalSpark {
    struct SpriteDrawable drawable;
    struct ContactPosition velocity;
    u8 active, age, baseFrame;
    u8 unk_33;
};

/* The player's six pooled sparks, stored after the directional effect (Player + 0x868). */
struct DirectionalSparkBurst {
    const struct SpriteEffectTileResource *resource;
    u8 active, cursor;
    u8 unk_06[2];
    struct DirectionalSpark sparks[6];
};

/* A view of struct Player (whose header leaves this range unnamed). */
struct PlayerSparkView {
    u8 unk_000[0x868];
    struct DirectionalSparkBurst burst;
};

#endif
