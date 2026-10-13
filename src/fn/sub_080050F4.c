#include "global.h"
struct Vec {
    s16 x, y, z;
};
struct Body {
    u8 pad[12];
    s16 x, y, z, unused, vx, vy, vz;
};
struct Cell {
    u8 height, tile;
    u16 flags;
};
struct Object {
    u8 type, flags;
    u16 timer, kind;
    u8 pad6[6];
    u16 dx, dz;
    u8 direction, entered;
    u16 targetY, duration;
    u8 pad16[22];
    struct Body body;
    u8 pad48[100];
    struct Vec position;
    u8 padB4[10];
    u16 cell;
    struct Cell terrain;
    u8 padC4[224];
    void (*update)(void);
};
void sub_0821FF24(void *, void *, s32);
void sub_0821FE40(void *);
void Scene_SpawnDirectionalDust(struct Object *);
s32 Scene_TryStepDown(struct Object *, s32, s32);
void Scene_PlayNeighborMovementEffect(struct Object *);
void Scene_UpdateFallingObject(void);
void sub_080050AC(void);
void Scene_UpdateObjectStep(struct Object *p)
{
    struct Body *body = &p->body;
    struct Cell *terrain = &p->terrain;
    if (p->entered) {
        p->entered = 0;
        terrain->flags |= 1;
    }
    body->vx = p->dx;
    body->vz = p->dz;
    body->y = (terrain->height & 15) << 8;
    if (p->kind == 2)
        body->y -= 256;
    p->position.y = body->y;
    sub_0821FF24((u8 *)p + 0x120, (u8 *)p + 0x38, 0);
    sub_0821FE40((u8 *)p + 0x120);
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
            } else if (!Scene_TryStepDown(p, p->cell, p->direction)) {
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
