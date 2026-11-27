#ifndef GUARD_INCLUDE_LINK_LINK_SCRIPT_PANEL_H
#define GUARD_INCLUDE_LINK_LINK_SCRIPT_PANEL_H

#include "global.h"

struct LinkScriptPanel {
    u8 pad0[0x23];
    u8 selection;
    u8 pad24[0xc];
    u32 counter;
    u8 pad34[0xa];
    u16 scale; /* confidence=low */
    u8 pad40[0x11];
    u8 windowReady;
    u8 pad52[0x12];
    u8 buttons[4]; /* confidence=low */
    u8 pad68[0x10];
    u8 hudReady;
    u8 pad79[0x3f];
    s32 window;
    u8 padBC[0x14];
    u32 script;
    u8 padD4[0x4];
    const u8 *scriptD8; /* confidence=low */
};

typedef char LinkScriptPanelSizeCheck[sizeof(struct LinkScriptPanel) == 0xdc ? 1 : -1];

#endif
