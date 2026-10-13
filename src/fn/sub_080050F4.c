#include "global.h"
#include "scene_object.h"
void sub_0821FF24(struct SceneCollider *, struct SceneVec *, s32);
void sub_0821FE40(struct SceneCollider *);
void Scene_UpdateObjectStep(struct SceneObject *p)
{
    struct SceneBody *body = &p->body;
    struct SceneCell *terrain = &p->cellRef.cell;
    if (p->entered) {
        p->entered = 0;
        terrain->flags |= 1;
    }
    body->vx = p->dx;
    body->vz = p->dz;
    body->position.y = (terrain->height & 15) << 8;
    if (p->kind == 2)
        body->position.y -= 256;
    p->center.y = body->position.y;
    sub_0821FF24(&p->stepCollider, &p->body.position, 0);
    sub_0821FE40(&p->stepCollider);
    if (p->type != 1 && !(p->timer & 3))
        Scene_SpawnDirectionalDust(p);
    if (++p->timer >= p->duration) {
        s32 height = terrain->height & 15;
        if (height < 15)
            height++;
        terrain->height = (terrain->height & 0xF0) | height;
        terrain->tile = 255;
        if (p->type == 1) {
            if (p->kind == 2) {
                {
                    register void (*callback)(void) asm("r1") = Scene_UpdateFallingObject;
                    p->update = callback;
                }
                p->timer = 0;
                p->entered = 1;
                p->kind = 2;
                p->dx = 0;
                p->dz = 0;
                p->direction = 0;
            } else if (!Scene_TryStepDown(p, p->cellRef.index, p->direction)) {
                Scene_PlayNeighborMovementEffect(p);
                {
                    register void (*callback)(void) asm("r1") = sub_080050AC;
                    p->update = callback;
                }
                p->timer = 0;
                p->entered = 1;
                p->kind = 0;
                p->dx = 0;
                p->dz = 0;
                p->direction = 0;
                terrain->flags &= ~1;
            }
        } else {
            if (p->kind == 2) {
                {
                    register void (*callback)(void) asm("r0") = Scene_UpdateFallingObject;
                    p->update = callback;
                }
                p->timer = 0;
                p->entered = 1;
                p->kind = 2;
            } else {
                Scene_PlayNeighborMovementEffect(p);
                {
                    register void (*callback)(void) asm("r1") = sub_080050AC;
                    p->update = callback;
                }
                p->timer = 0;
                p->entered = 1;
                p->kind = 0;
                terrain->flags &= ~1;
            }
            p->dx = 0;
            p->dz = 0;
            p->direction = 0;
        }
    }
}
