#ifndef GUARD_SCENE_PANEL_H
#define GUARD_SCENE_PANEL_H
#include "global.h"

struct PanelAccount {
    u8 unknown0[0x418];
    u8 kind, unknown419[15];
    u16 amount;
};
struct PanelSprite {
    u8 animation[8];
    u32 flags;
    u8 unknownC[0x14];
    u16 x, y;
    u8 unknown24[0x3C];
};
struct ScenePanel {
    u8 unknown0[0x18];
    u32 resource;
    u8 unknown1C[8];
    void *layerMap;
    u32 unknown28, mapResource;
    u8 unknown30[0x28];
    void *text;
    u8 unknown5C[0x68];
    u8 keyframes[0x97C];
    struct PanelAccount *account;
    u8 unknownA44[0x1DC];
    struct PanelSprite base;
    u8 unknownC80[0x2F60];
    struct PanelSprite mainSprite, secondary[2], detailSprite;
    u8 unknown3D60[0x120];
    struct PanelSprite rows[4], icon, rowIcons[4];
    u8 unknown41E0[0x706];
    u8 iconIndex, unknown48E7[2], state;
    u16 timer, selection, previousSelection;
};
#endif
