#ifndef GUARD_INCLUDE_LINK_PLAYER_BRIEF_SPRITE_EFFECT_H
#define GUARD_INCLUDE_LINK_PLAYER_BRIEF_SPRITE_EFFECT_H

#include "global.h"
#include "engine/video/sprite_oam.h"

struct PlayerBriefSpriteEffect {
    u8 pad0[0x744];
    void *graphics; /* confidence=low */
    struct SpriteDrawable sprite; /* confidence=low */
    u8 active;
    u8 tick; /* confidence=low */
    u8 firstFrame;
    u8 pad773[0x1];
};

typedef char PlayerBriefSpriteEffectSizeCheck[sizeof(struct PlayerBriefSpriteEffect) == 0x774 ? 1 : -1];

#endif
