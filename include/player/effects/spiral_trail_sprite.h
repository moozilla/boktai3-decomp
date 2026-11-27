#ifndef GUARD_INCLUDE_PLAYER_EFFECTS_SPIRAL_TRAIL_SPRITE_H
#define GUARD_INCLUDE_PLAYER_EFFECTS_SPIRAL_TRAIL_SPRITE_H

#include "engine/video/sprite.h"
#include "scene/scene_animation.h"

struct SpiralTrailSprite {
    struct DisplaySequenceSprite drawable;
    struct SceneAnimation graphics;
    const u16 *sourcePalette;
    u16 colors[16];
    u8 active, alpha;
};

typedef char SpiralTrailSpriteSizeCheck[sizeof(struct SpiralTrailSprite) == 0x70 ? 1 : -1];

#endif
