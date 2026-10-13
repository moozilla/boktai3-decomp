#include "global.h"
#include "scene_object.h"
void Scene_UpdateFallingObject(struct SceneObject *p)
{
    struct SceneBody *body = &p->body;
    struct SceneCell *terrain = &p->cellRef.cell;
    if (p->entered) {
        s32 height;
        p->entered = 0;
        height = (p->targetY >> 8) + 1;
        if (height > 15)
            terrain->height = (terrain->height & 0xF0) | 15;
        else
            terrain->height = (terrain->height & 0xF0) | height;
        if (({
                u32 mask = 8;
                mask & p->flags;
            }))
            p->cellRef.cell.flags |= 0x21;
    }
    body->vy = p->targetY - body->position.y;
    if (body->vy < -16)
        body->vy = -16;
    else if (body->vy > 0)
        body->vy = 0;
    if (body->vy == 0) {
        if (({
                u32 mask = 8;
                mask & p->flags;
            })) {
            p->cellRef.cell.flags |= 0x21;
            p->health = 0;
        } else {
            p->cellRef.cell.flags &= ~0x21;
            Scene_PlayMovementEffect(p);
        }
        {
            register void (*callback)(void) asm("r1") = sub_080050AC;
            p->update = callback;
        }
        p->timer = 0;
        p->entered = 1;
        p->dx = 0;
        p->dz = 0;
        p->direction = 0;
        p->kind = 0;
    }
}
