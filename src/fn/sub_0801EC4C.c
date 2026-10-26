#include "scene_object_slot.h"
#include "scene_actor_terrain.h"
void TerrainModifiers_RegisterCell(void *, u32, u32, u32, u32, u32);
void SceneObject_UpdateInactiveIdle(void);
void SceneObject_UpdateActiveIdle(void);

// confidence=low: places a newly appended scene object on its terrain cell and starts its idle state.
void SceneObject_InitPlacement(u8 *mgr, struct SceneObjSlot *obj)
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
        switch (({
            u32 ty = obj->type;
            cf2 = cellField;
            ty;
        })) {
            case 0:
            case 3:
                Sprite_Init(sprite, mgr + (obj->value * 28 + 0x3c), 0);
                break;
            case 4:
                Sprite_Init(sprite, mgr + (obj->value * 28 + 0x1e0), 0);
                break;
            case 5:
                Sprite_Init(sprite, mgr + (obj->value * 28 + 0x26c), 0);
                AnimData_SelectPaletteBank(mgr + (obj->value * 28 + 0x26c), 0x3e);
                break;
            case 6:
            case 7:
                Sprite_Init(sprite, mgr + (obj->value * 28 + 0x2f8), 0);
                break;
            case 15:
                Sprite_Init(sprite, mgr + (obj->value * 28 + 0x384), 0);
                break;
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
            obj->callback = SceneObject_UpdateInactiveIdle;
            obj->b39 = 0;
            obj->b3a = 0;
        } else if (t == 1) {
            u32 two;
            obj->callback = SceneObject_UpdateActiveIdle;
            two = 2;
            obj->b39 = two;
            obj->b3a = 0;
        } else {
            obj->callback = SceneObject_UpdateInactiveIdle;
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
