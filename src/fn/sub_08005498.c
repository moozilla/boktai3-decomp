#include "global.h"
#include "scene_object.h"
// MATCHING_CONCESSION: register-pinning
struct Args {
    u32 count : 16;
    s32 *values;
};
extern void (*const gUnk_086049B4[])(struct SceneObject *);
extern u8 *gUnk_02000710;
void sub_0821AD08(u32, struct Args *);
void sub_08225B74(void *);
void sub_0821DE30(void *);
void sub_0821FF24(void *, void *, s32);
void sub_08215284(void *, u16);
void sub_08221110(void *, u32, u32, s32, s32);
static inline u8 ownerIsCurrent(u32 owner)
{
    if (*(u32 *)(gUnk_02000710 + 0x5A0) == owner)
        return 1;
    return 0;
}
static inline void disableCollision(u16 *flags)
{
    register u32 value asm("r1") = *flags;
    {
        u32 result = value | 4;
        *flags = result;
    }
}
s32 SceneObjects_Update(struct SceneObjectManager *manager)
{
    struct SceneObject *p = manager->objects;
    s32 i;
    s32 values[5];
    struct Args args;
    for (i = 0; i < manager->count; i++, p++) {
        if (!p->active)
            continue;
        if (p->mode) {
            if (p->mode == 3 && gUnk_086049B4[p->type])
                gUnk_086049B4[p->type](p);
            if (p->script) {
                values[0] = p->body.id;
                values[1] = p->type;
                values[2] = p->body.position.x;
                values[3] = p->body.position.y;
                values[4] = p->body.position.z;
                args.count = 5;
                args.values = values;
                sub_0821AD08(p->script, &args);
                p->script = 0;
            }
            Scene_DropObjectsAbove(p);
            sub_080047AC(p);
            continue;
        }
        sub_08225B74(&p->body);
        if (p->update)
            p->update(p);
        if (p->flags & 0x20) {
            if (!p->kind) {
                p->body.flags &= ~2;
                sub_0821DE30(&p->link);
            } else
                p->body.flags |= 2;
        } else
            sub_0821DE30(&p->link);
        if (p->cooldown)
            p->cooldown--;
        if (!p->owner || ownerIsCurrent(p->owner))
            p->draw.flags &= ~1;
        else
            p->draw.flags |= 1;
        if (p->damageTimer || p->health <= 0) {
            p->damageTimer--;
            goto disable;
        }
        if (!p->owner || ownerIsCurrent(p->owner)) {
            p->collider.state &= ~4;
            sub_0821FF24(&p->collider, &p->body.position, 0);
        } else {
        disable:
            disableCollision(&p->collider.state);
        }
        if (p->animationTimer && !--p->animationTimer)
            sub_08215284((u8 *)p + 0x74, p->initialAnimation);
    }
    if (manager->fading) {
        sub_08221110(manager->paletteA, manager->fromA, manager->toA, manager->fade, 5);
        sub_08221110(manager->paletteB, manager->fromB, manager->toB, manager->fade, 5);
        if (!manager->descending) {
            if (++manager->fade > 31) {
                manager->fade = 32;
                manager->descending = 1;
            }
        } else if (--manager->fade <= 0) {
            manager->fade = 0;
            manager->descending = 0;
            if (manager->stopFade)
                manager->stopFade = 0;
        }
    }
    manager->frame = 1 - manager->frame;
    return 0;
}
