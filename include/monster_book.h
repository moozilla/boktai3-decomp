/* Monster book (bestiary) screen layout, inferred from MonsterBook_Create's
 * teardown order and the list/detail redraw helpers. Field roles are tentative. */
#ifndef GUARD_MONSTER_BOOK_H
#define GUARD_MONSTER_BOOK_H
#include "scene_input.h"

struct MonsterBook {
    struct Task task;
    u32 unknown18[2];
    u32 frame;
    u8 mode;
    u8 category;
    u8 entry;
    u8 unk27;
    s32 windows[8];
    u8 selectedRow;
    u8 unknown49[3];
    u8 animData[0x20];
    struct SceneInputWidget s6C;
    struct SceneInputWidget sCC;
    struct SceneInputWidget s12C;
    struct SceneInputWidget s18C;
    struct SceneInputWidget s1EC;
    struct SceneInputWidget s24C;
    struct SceneInputWidget s2AC[7];
    struct SceneInputWidget s54C[7][2];
    struct SceneInputWidget sA8C[2];
    struct SceneInputWidget sB4C;
    struct SceneInputWidget sBAC[2];
    struct SceneInputWidget completionBadge, groupBadge;
    u16 palette[16];
    s16 highlightFrame;
    u16 transitionFlags;
    const u8 *script;
    u8 unkD54[0xDA7 - 0xD54];
    u8 pendingExit;
    u8 unkDA8[0xF18 - 0xDA8];
    void *closeScript;
    void *display;
};

#endif
