#ifndef GUARD_INCLUDE_UI_BOOKS_BESTIARY_DISPLAY_H
#define GUARD_INCLUDE_UI_BOOKS_BESTIARY_DISPLAY_H

#include "global.h"
#include "engine/video/sprite.h"

struct BestiaryDisplay;

struct BestiaryAnimation {
    u32 active;
    u8 unk04[0x88];
    u16 *position; /* x, y, z */
    u8 unk90[4];
};

struct BestiaryDisplayConfig {
    u16 id;
    u16 frames;
    u16 kind;
    u16 sequence;
    u32 direction;
    u16 palette;
    u8 directionIndex;
    u8 flags;
    void (*create)(struct BestiaryDisplay *);
    void (*update)(struct BestiaryDisplay *);
    s8 x, y;
    u8 swayAmplitude, swaySpeed;
    u8 unk1C[4];
    u8 overlay;
    u8 unk21[3];
};

struct BestiaryDisplay {
    u8 unk00[0x18];
    s32 swayPhase;
    s32 entry;
    s32 frame;
    u8 state;
    u8 hidden;
    u16 mode;
    struct BestiaryAnimation animations[6];
    u8 unk3A0[0x9A4 - 0x3A0];
    struct DisplaySequenceSprite sprite;
    u8 animData[0x1C];
    u16 origin[4];
    u8 unk9F4[8];
    u16 sequence;
    u8 dirX, dirY;
    u16 scale;
    u8 holdAnimation;
    u8 entryChanged;
    u8 unkA04[4];
    const struct BestiaryDisplayConfig *config;
};

typedef char BestiaryDisplaySizeCheck[sizeof(struct BestiaryDisplay) == 0xA0C ? 1 : -1];

extern const struct BestiaryDisplayConfig gUnk_086126E0[];

#endif
