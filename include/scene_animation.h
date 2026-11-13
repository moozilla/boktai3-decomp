#ifndef GUARD_SCENE_ANIMATION_H
#define GUARD_SCENE_ANIMATION_H
#include "global.h"

struct SceneAnimation {
    u8 pad0[6];
    u16 paletteIndex; /* selected bank of sixteen RGB15 colors */
    u8 pad8[4];
    void *palette; /* palette override used by type 6 objects */
    u8 tail[12];
};

#endif
