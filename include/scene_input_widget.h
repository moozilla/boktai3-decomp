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
    u8 unknown0[2];
    u16 stamp; /* Copied from 03005238 on initialization; meaning tentative. */
    u8 unknown4[4];
    u32 flags;
    u16 fieldC, attributeE, scale;
    u8 unknown12[8];
    u8 attribute1A, attribute1B;
    u8 unknown1C[4];
    struct ContactPosition position;
    u8 unknown28[0x12];
    u16 paletteBank;
    u8 unknown3C[0xC];
    u16 *palette;
    u8 unknown4C[0x14];
};

#endif
