#ifndef GUARD_MENU_PROMPT_H
#define GUARD_MENU_PROMPT_H
#include "global.h"

struct MenuPrompt {
    union {
        struct {
            u8 unknown0[0xC4], animationData[0xA4C - 0xC4];
            u16 timer;
            u8 unknownA4E[0x3BE0 - 0xA4E];
            u8 animation[4];
        } ramp;
        struct {
            u8 unknown0[0xC4], animationData[0xA4C - 0xC4];
            u16 timer;
            u8 unknownA4E[0x3BE0 - 0xA4E];
            u8 animation[0x48EA - 0x3BE0];
            u16 result;
            u8 unknown48EC[4];
            u32 script;
        } delayed;
        struct {
            u8 unknown0[0x48F4];
            void *child;
        } child;
        struct {
            u8 unknown0[0xC4], animationData[0x3C40 - 0xC4];
            u8 firstSprite[0x60], secondSprite[0x80];
            s16 selectionX, selectionY;
            u8 unknown3D24[0x48E4 - 0x3D24];
            u8 selection;
        } selection;
    } view;
};
/* Pause-menu prompt state (layout follows SceneHudState offsets; tentative). */
struct MenuPromptAccount {
    u8 unknown0[0x418];
    u8 kind, unknown419[15];
    u16 amount;
};
struct MenuPromptSprite {
    u8 animation[8];
    u32 flags;
    u8 unknownC[0x14];
    u16 x, y;
    u8 unknown24[0x3C];
};
struct MenuPromptPanel {
    u8 unknown0[0x18];
    u32 resource;
    u8 unknown1C[8];
    void *layerMap;
    u32 unknown28, mapResource;
    u8 unknown30[0x28];
    void *text;
    u8 unknown5C[0x68];
    u8 keyframes[0x97C];
    struct MenuPromptAccount *account;
    u8 unknownA44[0x1DC];
    struct MenuPromptSprite base;
    u8 unknownC80[0x2F60];
    struct MenuPromptSprite mainSprite, secondary[2], detailSprite;
    u8 unknown3D60[0x120];
    struct MenuPromptSprite rows[4], icon, rowIcons[4];
    u8 unknown41E0[0x706];
    u8 iconIndex, unknown48E7[2], state;
    u16 timer, selection, previousSelection;
};
#endif
