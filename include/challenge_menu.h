#ifndef GUARD_CHALLENGE_MENU_H
#define GUARD_CHALLENGE_MENU_H

#include "scene_input.h"

struct ChallengeMenu {
    u8 unknown0[0x18];
    struct SceneInputResource resources[3];
    struct SceneInputWidget widgets[6];
    u8 unknown2B8[0x20];
    u16 highlightPalette[4];
    u8 highlightPhase;
    u8 unknown2E1[0x2FC - 0x2E1];
    s32 group, selection;
    u8 unknown304[8];
    void *stateCallback;
};

s32 sub_081C58BC(struct ChallengeMenu *menu);

#endif
