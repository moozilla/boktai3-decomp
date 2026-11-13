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
#endif
