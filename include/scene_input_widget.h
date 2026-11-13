#ifndef GUARD_SCENE_INPUT_WIDGET_H
#define GUARD_SCENE_INPUT_WIDGET_H

#include "contact_position.h"

struct SceneWidgetPlayback {
    u8 priority, mode;
};

struct SceneInputResource {
    u32 words[8];
};

struct SceneInputWidget {
    u16 glyphKind;
    u16 stamp; /* Copied from 03005238 on initialization; meaning tentative. */
    u8 unknown4[2]; /* first byte: linked to an animation list */
    u16 value6;
    u32 flags;
    u16 fieldC, attributeE, scale;
    u16 frame, frameTimer, frameDuration, value18;
    u8 attribute1A, attribute1B;
    u8 list, status;
    u16 unknown1E;
    struct ContactPosition position;
    u16 width, height, originX, originY, centerX, centerY;
    u8 scaleX, scaleY;
    u16 rotation; /* 0x36: angle copied by the thrown HUD icon. */
    u16 resourceIndex;
    u16 paletteBank;
    u32 value3C;
    s32 cachedFrame, cachedPalette;
    u16 *palette;
    u8 *tileData, *extraData;
    u32 renderData;
    struct SceneInputWidget *previous, *next;
};

#endif
