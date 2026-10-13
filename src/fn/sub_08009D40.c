#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Actor {
    u8 pad[48];
    struct Vec position;
};
struct Controller {
    u16 id;
    u8 orientation, flags, disabled, state, entered, level;
    s32 region;
    u16 startScript, endScript;
    u32 distance;
    s16 arguments[4];
    struct Vec position;
    u32 drawableFlags;
    u8 rest[68];
};
struct Manager {
    u8 pad[24];
    u32 count : 8;
    s32 selected : 8;
    s32 pending : 8;
    u32 unused : 8;
    u32 enabled;
    struct Controller *slots;
    struct Actor *actor;
    struct Vec position;
};
extern struct Manager *gUnk_02000038;
void sub_08009728(struct Manager *, s32);
void sub_08009500(struct Controller *, u32);
s32 DirectionalController_ScriptRelease(void)
{
    struct Manager *manager = gUnk_02000038;
    if (!manager || manager->selected < 0 || manager->selected >= manager->count)
        return -1;
    sub_08009728(manager, manager->selected);
    sub_08009500(&manager->slots[manager->selected], 0);
    return 0;
}
