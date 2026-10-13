#include "global.h"
struct Object {
    u8 type, flags;
    u16 state, kind;
    u8 pad6[11];
    u8 moving;
    u8 pad12[4];
    s16 resource;
    u8 pad18[6];
    u8 active;
    u8 pad1F[389];
    void (*update)(void);
};
struct Manager {
    u8 pad[0x18];
    u16 count;
    u8 pad1A[86];
    struct Object *objects;
};
extern struct Manager *gUnk_02000024;
void Scene_UpdateObjectDisappearance(void);
s32 Scene_ActivateTypeOneObjects(void)
{
    struct Manager *manager = gUnk_02000024;
    struct Object *p;
    s32 i;
    if (!manager)
        return -1;
    p = manager->objects;
    for (i = 0; i < manager->count; p++, i++) {
        if (!p->active)
            continue;
        if (p->type != 1)
            continue;
        p->resource = 0;
        if (!p->kind) {
            p->update = Scene_UpdateObjectDisappearance;
            p->state = 0;
            p->moving = 1;
            p->kind = 3;
        }
    }
    return 0;
}
