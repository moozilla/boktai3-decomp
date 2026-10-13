#include "global.h"
#include "scene_object.h"
// MATCHING_CONCESSION: register-pinning
void sub_08219DD8(void *, u32);
void sub_082151E4(struct SceneAnimation *, u32);
void sub_082144A4(struct SceneDraw *, struct SceneAnimation *, s32);
void sub_08225AB4(struct SceneBody *, u32, struct SceneVec *, s32, s32, struct SceneObject *);
void sub_08225B28(struct SceneBody *, struct SceneDraw *);
u32 sub_0822C034(struct SceneVec *);
u8 *sub_0821D64C(s32, s32);
u32 sub_0821D698(void *, u32, u32, u32, u32, u32);
void sub_0821FEB4(struct SceneCollider *, u16, u32, s32, s32, struct SceneExtent *, struct SceneExtent *);
void sub_0821FF24(struct SceneCollider *, struct SceneVec *, s32);
void sub_0821FF84(struct SceneCollider *, void *, struct SceneObject *);
void sub_0821FE40(struct SceneCollider *);
void sub_0821FF50(struct SceneCollider *, s32, s32);
s32 Mod(s32, s32);
void sub_08215284(struct SceneAnimation *, u16);
s32 sub_08003C00(struct SceneBody *);
static inline void initializeCell(void *cell, s32 index, s32 height)
{
    sub_0821D698(cell, index, 0, height, 255, ({
                     register s32 zero asm("r2") = 0;
                     zero;
                 }));
}
static inline void setStepFlags(struct SceneCollider *c)
{
    c->contactFlags = 0x800;
    c->contactFlags2 = 0;
}
static inline u16 flags16(struct SceneObject *p, u32 mask)
{
    return mask & p->flags;
}
s32 SceneObjects_CreateObject(u32 id, s32 type, u32 flags, struct SceneVec *position,
                              u32 animationResource, s32 drawMode, s32 health,
                              s32 initialAnimation, s32 animation, u32 script)
{
    struct SceneExtent size, offset;
    struct SceneObjectManager *manager = gUnk_02000024;
    struct SceneObject *p;
    register struct SceneDraw *draw asm("r5");
    register struct SceneVec *pos asm("r9");
    s32 x, z, index, height;
    u32 clear;
    u8 *dynamic;
    struct SceneCell *cell;
    if (!manager || !(p = sub_080057D0(manager)))
        return -1;
    sub_08219DD8(p, 0x1A8);
    p->active = 1;
    p->type = type;
    if (p->type == 6)
        manager->fading = 1;
    p->flags = flags;
    p->timer = 0;
    p->kind = 0;
    sub_082151E4(&p->animationData, animationResource);
    draw = &p->draw;
    asm("" : "+r"(draw));
    sub_082144A4(draw, &p->animationData, 0);
    draw->mode = drawMode;
    p->center.x = (position->x & ~255) + 128;
    p->center.y = position->y & ~255;
    p->center.z = (position->z & ~255) + 128;
    sub_08225AB4(&p->body, id, position, 0, 7, p);
    sub_08225B28(&p->body, draw);
    p->body.position = p->center;
    if (flags16(p, 16)) {
        p->owner = sub_0822C034(&p->body.position);
        pos = &p->body.position;
    } else {
        p->owner = 0;
        pos = &p->body.position;
    }
    p->dx = 0;
    p->dz = 0;
    p->direction = 0;
    if (flags16(p, 4)) {
        {
            register void (*callback)(struct SceneObject *) asm("r0") = Scene_UpdateObjectFlash;
            asm("" : "+r"(callback));
            p->update = callback;
        }
        p->timer = 0;
    } else {
        {
            register void (*callback)(struct SceneObject *) asm("r0") = sub_080050AC;
            asm("" : "+r"(callback));
            p->update = callback;
        }
        p->timer = 0;
    }
    p->entered = 1;
    x = p->body.position.x >> 8;
    z = ({
            register struct SceneVec *coords asm("r1") = pos;
            asm("" : "+r"(coords));
            coords->z;
        }) >>
        8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        index = 0;
    else
        index = gUnk_030052F4->rows[z] + x;
    dynamic = sub_0821D64C(index, 1);
    if (dynamic)
        cell = (struct SceneCell *)(dynamic + 4);
    else
        cell = (struct SceneCell *)(gUnk_030052F4->cells + 12 + index * 4);
    height = cell->height & 15;
    if (height < 15)
        height++;
    initializeCell(&p->cellRef, index, height);
    {
        register u32 y asm("r0") = height << 8;
        asm("" : "+r"(y));
        clear = ({
            register u32 zero asm("r3") = 0;
            asm("" : "+r"(zero));
            zero;
        });
        p->targetY = y;
    }
    size.x = 130;
    size.y = 128;
    size.z = 130;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    sub_0821FEB4(&p->collider, p->body.id, 0x4041, 0, 16, &size, &offset);
    sub_0821FF24(&p->collider, pos, 0);
    sub_0821FF84(&p->collider, Scene_ApplyTerrainInteraction, p);
    sub_0821FE40(&p->collider);
    ((u16 *)&size)[0] = 128;
    ((u16 *)&size)[1] = 128;
    ((u16 *)&size)[2] = 128;
    sub_0821FEB4(&p->stepCollider, p->body.id, 0x2801, 0, 1, &size, &offset);
    sub_0821FF24(&p->stepCollider, pos, 0);
    sub_0821FF84(&p->stepCollider, sub_080046EC, p);
    sub_0821FF50(&p->stepCollider, 1500, 50);
    setStepFlags(&p->stepCollider);
    p->health = health;
    p->damageTimer = clear;
    p->mode = clear;
    p->animationTimer = clear;
    p->phase = clear;
    p->cooldown = clear;
    p->unk23 = 4;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    p->randomPhase = Mod(({
                             u32 offset = gUnk_03005308 * 2;
                             register u16 *table asm("r2");
                             asm("" : "+r"(offset));
                             table = gUnk_0203B400;
                             asm("" : "+r"(table));
                             *(u16 *)(offset + (u32)table);
                         }),
                         16);
    {
        register s32 initial asm("r3") = initialAnimation;
        asm("" : "+r"(initial));
        if (initial >= 0)
            p->initialAnimation = initial;
        else
            p->initialAnimation = p->animationData.initial;
    }
    sub_08215284(&p->animationData, p->initialAnimation);
    p->animation = animation;
    p->link.id = p->body.id;
    p->link.position = pos;
    p->script = script;
    if (!(p->flags & 2) && sub_08003C00(&p->body) < 0)
        p->flags |= 2;
    if (p->type == 5) {
        p->pushLimited = 1;
        p->pushesLeft = 1;
    } else {
        p->pushLimited = 0;
        p->pushesLeft = 0;
    }
    return 0;
}
