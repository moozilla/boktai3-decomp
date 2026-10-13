#include "global.h"
#include "scene_object.h"
/* Start falling for objects resting above another object in the same cell. */
typedef void (*UpdateFn)(struct SceneObject *);
static inline UpdateFn fallCallback(void)
{
    return Scene_UpdateFallingObject;
}
void Scene_DropObjectsAbove(struct SceneObject *object)
{
    struct SceneObjectManager *manager = gUnk_02000024;
    struct SceneObject *p = manager->objects;
    s32 i;
    for (i = 0; i < manager->count; i++, p++) {
        if (!p->active)
            continue;
        if (p == object)
            continue;
        {
            u8 *height = &p->cellRef.cell.height;
            u8 *reference = &object->cellRef.cell.height;
            if (p->cellRef.index != object->cellRef.index)
                continue;
            if (p->kind != 0)
                continue;
            if ((u32)(*height & 15) <= (u32)(*reference & 15))
                continue;
            p->update = fallCallback();
            p->timer = 0;
            p->entered = 1;
            p->kind = 2;
            p->dx = 0;
            p->dz = 0;
            p->direction = 0;
            p->targetY = p->body.position.y;
            if (p->targetY > 255)
                p->targetY = p->body.position.y - 256;
            else
                p->targetY = 0;
        }
    }
}
