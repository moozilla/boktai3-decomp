#include "scene_object_slot.h"
#include "scene_actor_terrain.h"
void TerrainModifiers_RegisterCell(void *, u32, u32, u32, u32, u32);
void SceneObject_UpdateIdleA(void);
void SceneObject_UpdateIdleB(void);

// confidence=low: places a newly appended scene object on its terrain cell and starts its idle state.
void SceneObject_InitSlotPlacement(u8 *mgr, struct SceneObjSlot *obj)
{
    s32 cell = cellIndexAt(&obj->pos);
    u8 *dir;
    u8 *cellField;
    u32 t;
    s32 d;
    d = obj->dir;
    dir = &obj->dir;
    switch (d) {
        case 0:
            obj->cellA = cell - 1;
            obj->cellB = cell;
            obj->pos.z += 0x80;
            break;
        case 1:
            obj->cellA = cell - *(u16 *)(gTerrainMap->cells + 4);
            obj->cellB = cell;
            obj->pos.x += 0x80;
            break;
        case 2:
            obj->cellA = cell;
            obj->cellB = cell - 1;
            obj->pos.z += 0x80;
            break;
        case 3:
            obj->cellA = cell;
            obj->cellB = cell - *(u16 *)(gTerrainMap->cells + 4);
            obj->pos.x += 0x80;
            break;
    }
    {
        u16 c = obj->cellA;
        u8 v = *SceneActorTerrain_GetCell(c) & 15;
        cellField = &obj->b3b;
        {
            u32 zero = 0;
            *cellField = v;
            TerrainModifiers_RegisterCell(obj->block, obj->cellA, 0, *cellField, 0xff, zero);
        }
    }
    {
        struct SceneObjSlot *sprite = obj;
        u8 *cf2;
        if (((({
                s32 m = *(s32 *)(mgr + 0x34);
                cf2 = cellField;
                m;
            }))) == 0) {
            switch (obj->type) {
                case 2:
                    AnimData_SelectPaletteBank(mgr + (obj->value * 28 + 0x154), 0xe6);
                    Sprite_Init(sprite, mgr + (obj->value * 28 + 0x154), 0);
                    break;
                case 16:
                    Sprite_Init(sprite, mgr + (obj->value * 28 + 0x1e0), 0);
                    break;
            }
        } else {
            u8 *p;
            u8 **tbl;
            u32 k = obj->b45;
            p = &obj->b45;
            switch (k) {
                case 0:
                    AnimData_SelectPaletteBank(mgr + (obj->value * 28 + 0x154), *p + 0xe6);
                    Sprite_Init(sprite, mgr + (obj->value * 28 + 0x154), 0);
                    break;
                case 1:
                    tbl = (u8 **)(mgr + 0x410);
                    goto common;
                case 2:
                    tbl = (u8 **)(mgr + 0x414);
                    goto common;
                case 3:
                    tbl = (u8 **)(mgr + 0x418);
                    goto common;
                case 4:
                    tbl = (u8 **)(mgr + 0x41c);
                    goto common;
                case 5:
                    tbl = (u8 **)(mgr + 0x420);
                    goto common;
                case 6:
                    tbl = (u8 **)(mgr + 0x424);
                common:
                    AnimData_SelectPaletteBank(obj->value * 28 + *tbl, *p + 0xe6);
                    Sprite_Init(sprite, obj->value * 28 + *tbl, 0);
                    break;
                case 7:
                    tbl = (u8 **)(mgr + 0x428);
                    AnimData_SelectPaletteBank(obj->value * 28 + *tbl, *p + 0xe6);
                    Sprite_Init(sprite, obj->value * 28 + *tbl, 0);
                    break;
            }
        }
        *(struct Pair *)&sprite->a = *(struct Pair *)&obj->r0;
        if (!(*dir & 1)) {
            *(u16 *)&sprite->a -= 0x20;
            *(u16 *)&sprite->b += 0x20;
        } else {
            *(u16 *)&sprite->a += 0x20;
            *(u16 *)&sprite->b -= 0x20;
            *(u32 *)sprite |= 4;
        }
        *(struct Pair *)&obj->q0 = *(struct Pair *)&sprite->a;
        t = obj->variant;
        if (t == 0) {
            obj->callback = SceneObject_UpdateIdleA;
            obj->b39 = 0;
            obj->b3a = 0;
        } else if (t == 1) {
            u32 two;
            obj->callback = SceneObject_UpdateIdleB;
            two = 2;
            obj->b39 = two;
            obj->b3a = 0;
        } else {
            obj->callback = SceneObject_UpdateIdleA;
            obj->b39 = 0;
            obj->b3a = 0;
        }
        if (obj->variant == 0) {
            obj->block->w6 &= 0xfffd;
            obj->block->w6 |= 3;
            obj->block->b4 = (obj->block->b4 & 0xf0) | (*cf2 + 1);
        } else if (obj->variant == 1) {
            obj->block->w6 &= 0xfffc;
            obj->block->w6 |= 2;
            obj->block->b4 = (obj->block->b4 & 0xf0) | *cf2;
        }
    }
}
