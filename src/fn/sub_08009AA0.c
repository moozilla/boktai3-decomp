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
extern struct Actor *gUnk_02000580;
s32 sub_0821ABA8(u32, s32);
void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
s32 sub_08009524(struct Manager *, struct Controller *, s32);
s32 DirectionalController_InitManager(struct Manager *manager)
{
    struct Controller *p;
    s32 i;
    gUnk_02000038 = manager;
    manager->count = sub_0821ABA8('n', 1);
    manager->selected = -1;
    manager->pending = -1;
    manager->enabled = 0;
    manager->slots = sub_08219C40(manager->count * sizeof(struct Controller));
    if (!manager->slots) {
    fail:
        return -1;
    }
    sub_08219DD8(manager->slots, manager->count * sizeof(struct Controller));
    p = manager->slots;
    for (i = 0; i < manager->count; i++, p++)
        if (sub_08009524(manager, p, i) < 0)
            goto fail;
    manager->actor = gUnk_02000580;
    if (manager->actor)
        manager->position = manager->actor->position;
    else {
        manager->position.x = 0;
        manager->position.y = 0;
        manager->position.z = 0;
    }
    return 0;
}
