#include "global.h"

#include "scene_object_slot.h"
#include "scene_actor_terrain.h"
void TerrainModifiers_RegisterCell(void *, u32, u32, u32, u32, u32);
void SceneObject_UpdateRaised(void);
void SceneObject_UpdateLowered(void);
void *SceneParticleBurst_EnsureSingleton(u32, u32);
struct ObjBlockH {
    u16 h0;
};

#ifdef NONMATCHING
// fuzzy: 99.8%
// confidence=low: places a raisable scene object (blocks/gates with 10 facings) on its terrain cell.
void sub_0801F058(u8 *mgr, struct SceneObjSlot *obj)
{
    s32 cell = cellIndexAt(&obj->pos);
    u8 *dir;
    struct SceneObjSlot *sprite;
    s32 d;
    d = obj->dir;
    dir = &obj->dir;
    switch (d) {
        case 0:
            obj->cellA = cell - 1;
            obj->cellB = cell;
            obj->pos.z += 0x80;
            *(u32 *)((u8 *)obj + 0x84) = 1;
            break;
        case 1:
            obj->cellA = cell - *(u16 *)(gUnk_030052F4->cells + 4);
            obj->cellB = cell;
            obj->pos.x += 0x80;
            *(u32 *)((u8 *)obj + 0x84) = 1;
            break;
        case 2:
            obj->cellA = cell;
            obj->cellB = cell - 1;
            obj->pos.z += 0x80;
            *(u32 *)((u8 *)obj + 0x84) = 2;
            break;
        case 3:
            obj->cellA = cell;
            obj->cellB = cell - *(u16 *)(gUnk_030052F4->cells + 4);
            obj->pos.x += 0x80;
            *(u32 *)((u8 *)obj + 0x84) = 2;
            break;
        case 4:
            obj->cellA = cell - 1;
            obj->cellB = cell;
            obj->pos.z += 0x80;
            *(u32 *)((u8 *)obj + 0x84) = 0;
            break;
        case 5:
            obj->cellA = cell - *(u16 *)(gUnk_030052F4->cells + 4);
            obj->cellB = cell;
            obj->pos.x += 0x80;
            *(u32 *)((u8 *)obj + 0x84) = 0;
            break;
        case 6:
            obj->cellA = cell;
            obj->cellB = cell - 1;
            obj->pos.z += 0x80;
            *(u32 *)((u8 *)obj + 0x84) = 1;
            break;
        case 7:
            obj->cellA = cell;
            obj->cellB = cell - *(u16 *)(gUnk_030052F4->cells + 4);
            obj->pos.x += 0x80;
            *(u32 *)((u8 *)obj + 0x84) = 1;
            break;
        case 8:
            obj->cellA = cell;
            obj->cellB = cell - 1;
            obj->pos.z += 0x80;
            *(u32 *)((u8 *)obj + 0x84) = 0;
            break;
        case 9:
            obj->cellA = cell;
            obj->cellB = cell - *(u16 *)(gUnk_030052F4->cells + 4);
            obj->pos.x += 0x80;
            *(u32 *)((u8 *)obj + 0x84) = 0;
            break;
    }
    obj->b3b = *SceneActorTerrain_GetCell(obj->cellA) & 15;
    TerrainModifiers_RegisterCell(obj->block, obj->cellA, 0, obj->b3b, 0xff, 0);
    obj->b39 = *((u8 *)obj + 0x84);
    obj->b3a = 0;
    sprite = obj;
    Sprite_Init(sprite, mgr + (obj->value * 28 + 0xc8), 0);
    *(struct Pair *)&obj->a = *(struct Pair *)&obj->r0;
    if (!(*dir & 1)) {
        *(u16 *)&obj->b += 0x80;
    } else {
        *(u16 *)&obj->a += 0x80;
        *(u32 *)obj |= 4;
    }
    *(struct Pair *)&obj->q0 = *(struct Pair *)&sprite->a;
    if (obj->variant == 0)
        obj->callback = SceneObject_UpdateLowered;
    else if (obj->variant == 1)
        obj->callback = SceneObject_UpdateRaised;
    else
        obj->callback = SceneObject_UpdateLowered;
    if (obj->variant == 0) {
        *(u16 *)((u8 *)obj + 0x82) = 0;
        obj->variant = 1;
        obj->callback = SceneObject_UpdateLowered;
        obj->b39 = *((u8 *)obj + 0x84);
        obj->b3a = 0;
        obj->block->w6 &= 0xfffd;
        obj->block->w6 |= 3;
        ((struct ObjBlockH *)obj->block)->h0 &= 0xfffe;
        obj->block->b4 = (obj->block->b4 & 0xf0) | (obj->b3b + 1);
        *(u16 *)((u8 *)obj + 0x80) = 0;
        {
            u16 t = *(u16 *)((u8 *)obj + 0x7a);
            *(u16 *)((u8 *)obj + 0x72) = t;
        }
    } else if (obj->variant == 1) {
        u16 *p = (u16 *)((u8 *)obj + 0x82);
        u8 z8 = 0;
        *p = 0;
        obj->variant = z8;
        obj->callback = SceneObject_UpdateRaised;
        obj->b39 = z8;
        obj->b3a = z8;
        obj->block->w6 &= 0xfffc;
        obj->block->w6 |= 2;
        ((struct ObjBlockH *)obj->block)->h0 |= 1;
        obj->block->b4 = (obj->block->b4 & 0xf0) | obj->b3b;
        {
            u16 t = *(u16 *)((u8 *)obj + 0x88);
            *(u16 *)((u8 *)obj + 0x80) = t;
        }
        {
            u16 t = *(u16 *)((u8 *)obj + 0x7a) + *(u16 *)((u8 *)obj + 0x88);
            *(u16 *)((u8 *)obj + 0x72) = t;
        }
    }
    *(u32 *)((u8 *)obj + 0x60) = 0;
    *(u32 *)((u8 *)obj + 0x64) = 0;
    SceneParticleBurst_EnsureSingleton(0, 0);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0801F058);
#endif
