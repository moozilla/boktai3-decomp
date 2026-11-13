#ifndef GUARD_REGION_MENU_H
#define GUARD_REGION_MENU_H

#include "global.h"

struct RegionEntry {
    s16 group;
    u16 name;
    s32 longitude;
    s32 latitude;
};

struct RegionMenuAnim {
    u8 widget[8];
    u32 flags;
    u8 reserved0C[0x3C];
    void *palette;
    u8 reserved4C[0x14];
    u8 resource[0x28];
};

struct RegionMenu {
    u8 task[0x1C];
    s32 page;
    s32 count;
    u8 reserved24[2];
    s8 selected;
    u8 highlightPulse;
    s32 pageFirst[14];
    struct RegionMenuAnim anims[4];
    struct RegionEntry entries[64];
    void *savedPalette;
    union {
        u16 colors[16];
        struct {
            u8 reserved[24];
            u8 pulseState[8];
        } pulse;
    } highlightPalette;
    s32 state;
    s32 confirmed;
    s32 timer;
    s32 fadeSpeed;
    s32 skipped;
};

void Time_ShowRegionMenu(struct RegionMenu *menu);
void Time_OnRegionListSelect(struct RegionMenu *menu);
void Time_HandleRegionKeypad(struct RegionMenu *menu);
void Time_DestroyRegionMenu(struct RegionMenu *menu);
s32 Time_LoadRegionList(struct RegionMenu *menu);
void *Time_CreateRegionMenu(void);

#endif
