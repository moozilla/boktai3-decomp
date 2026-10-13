#include "global.h"
/* Tentative: start falling for objects above another object in the same cell. */
struct Object {
    u8 type, flags;
    u16 state, kind;
    u8 pad6[6];
    u16 dx, dz;
    u8 direction, moving;
    u16 targetY;
    u8 pad14[10];
    u8 active;
    u8 pad1F[27];
    u16 y;
    u8 pad3C[130];
    u16 cell;
    u8 height;
    u8 padC1[227];
    void (*update)(void);
};
struct Manager {
    u8 pad[0x18];
    u16 count;
    u8 pad1A[86];
    struct Object *objects;
};
extern struct Manager *gUnk_02000024;
typedef void (*UpdateFn)(void);
void Scene_UpdateFallingObject(void);
static inline UpdateFn fallCallback(void)
{
    return Scene_UpdateFallingObject;
}
void Scene_DropObjectsAbove(struct Object *object)
{
    struct Manager *manager = gUnk_02000024;
    struct Object *p = manager->objects;
    s32 i;
    for (i = 0; i < manager->count; i++, p++) {
        if (!p->active)
            continue;
        if (p == object)
            continue;
        {
            u8 *height = &p->height;
            u8 *reference = &object->height;
            if (p->cell != object->cell)
                continue;
            if (p->kind != 0)
                continue;
            if ((u32)(*height & 15) <= (u32)(*reference & 15))
                continue;
            p->update = fallCallback();
            p->state = 0;
            p->moving = 1;
            p->kind = 2;
            p->dx = 0;
            p->dz = 0;
            p->direction = 0;
            p->targetY = p->y;
            if (p->targetY > 255)
                p->targetY = p->y - 256;
            else
                p->targetY = 0;
        }
    }
}
