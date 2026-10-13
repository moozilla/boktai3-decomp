#include "global.h"
#include "scene_object.h"
// MATCHING_CONCESSION: register-pinning
u32 sub_0821ABA8(s32, s32);
s32 Scene_StopSelectedObject(void)
{
    struct SceneObjectManager *manager = gUnk_02000024;
    register u32 id asm("r6");
    struct SceneObject *p;
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
        if (p->body.id == id) {
            p->health = 0;
            p->mode = 3;
            p->update = 0;
            return 1;
        }
    }
    return 0;
}
