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
void sub_08214514(void *);
void sub_08219D38(void *);
#define DirectionalController_DestroyManager sub_08009A68
s32 DirectionalController_DestroyManager(struct Manager *manager)
{
    struct Controller *p = manager->slots;
    s32 i;
    for (i = 0; i < manager->count; i++, p++)
        sub_08214514(&p->drawableFlags);
    if (manager->slots)
        sub_08219D38(manager->slots);
    gUnk_02000038 = 0;
    return 0;
}
