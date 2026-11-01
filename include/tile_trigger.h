#ifndef GUARD_TILE_TRIGGER_H
#define GUARD_TILE_TRIGGER_H

#include "player_cue_sequence.h"
#include "scene_vec.h"

struct TileTrigger {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x74 - 0x2C];
    struct DisplaySequenceState animation;
    struct SceneVec position;
    u32 key;
    u8 unknown90[0x28];
    u16 sequence, delay, unknownBC;
    s16 state;
    u16 triggerFlags;
    u8 forced, frame;
    s32 timer;
};

#endif
