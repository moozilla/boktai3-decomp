#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define Scene_StopSelectedObject sub_08004888
struct Object {
    u8 type, flags;
    u16 state, kind;
    u8 pad6[16];
    s16 resource;
    u8 pad18, mode, pad1A[4], active, pad1F[13];
    u32 id;
    u8 pad30[372];
    void (*update)(void);
};
struct Manager {
    u8 pad[0x18];
    u16 count;
    u8 pad1A[86];
    struct Object *objects;
};
extern struct Manager *gUnk_02000024;
u32 sub_0821ABA8(s32, s32);
s32 Scene_StopSelectedObject(void)
{
    struct Manager *manager = gUnk_02000024;
    register u32 id asm("r6");
    struct Object *p;
    register s32 i asm("r5");
    register s32 count asm("r4");
    if (!manager || !(id = sub_0821ABA8(0x6E, 0)))
        return -1;
    p = manager->objects;
    i = 0;
    count = manager->count;
    for (; i < count; p++, i++) {
        if (!p->active)
            continue;
        if (p->type == 1)
            continue;
        if (p->kind)
            continue;
        if (p->id == id) {
            p->resource = 0;
            p->mode = 3;
            p->update = 0;
            return 1;
        }
    }
    return 0;
}
