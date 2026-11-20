#include "scene_object_controller.h"
#include "scene_actor_terrain.h"
void TerrainModifiers_RegisterCell(void *, u32, u32, u32, u32, u32);
void SceneObject_UpdateIdleA(void);
void SceneObject_UpdateIdleB(void);

// confidence=low: places a newly appended scene object on its terrain cell and starts its idle state.
void SceneObject_InitSlotPlacement(struct SceneObjPool *mgr, struct SceneObjSlot *obj)
{
    s32 cell = cellIndexAt(&obj->pos.position);
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
            obj->pos.position.z += 0x80;
            break;
        case 1:
            obj->cellA = cell - (u16)((struct TerrainGrid *)gTerrainMap->cells)->width;
            obj->cellB = cell;
            obj->pos.position.x += 0x80;
            break;
        case 2:
            obj->cellA = cell;
            obj->cellB = cell - 1;
            obj->pos.position.z += 0x80;
            break;
        case 3:
            obj->cellA = cell;
            obj->cellB = cell - (u16)((struct TerrainGrid *)gTerrainMap->cells)->width;
            obj->pos.position.x += 0x80;
            break;
    }
    {
        u16 c = obj->cellA;
        u8 v = *SceneActorTerrain_GetCell(c) & 15;
        cellField = &obj->b3b;
        obj->b3b = v;
        TerrainModifiers_RegisterCell(obj->block, obj->cellA, 0, obj->b3b, 0xff, 0);
    }
    {
        struct SceneObjSlot *sprite = obj;
        u8 *cf2;
        s32 mode = mgr->separatePalettes;
        cf2 = cellField;
        if (mode == 0) {
            switch (obj->type) {
                case 2:
                    AnimData_SelectPaletteBank(&mgr->animationData[2][obj->value], 0xe6);
                    Sprite_Init(sprite, &mgr->animationData[2][obj->value], 0);
                    break;
                case 16:
                    Sprite_Init(sprite, &mgr->animationData[3][obj->value], 0);
                    break;
            }
        } else {
            u8 *p;
            struct SceneAnimation **bank;
            u32 k = obj->b45;
            p = &obj->b45;
            switch (k) {
                case 0:
                    AnimData_SelectPaletteBank(&mgr->animationData[2][obj->value], *p + 0xe6);
                    Sprite_Init(sprite, &mgr->animationData[2][obj->value], 0);
                    break;
                case 1:
                    bank = &mgr->paletteAnimationData[0];
                    goto common;
                case 2:
                    bank = &mgr->paletteAnimationData[1];
                    goto common;
                case 3:
                    bank = &mgr->paletteAnimationData[2];
                    goto common;
                case 4:
                    bank = &mgr->paletteAnimationData[3];
                    goto common;
                case 5:
                    bank = &mgr->paletteAnimationData[4];
                    goto common;
                case 6:
                    bank = &mgr->paletteAnimationData[5];
                common:
                    AnimData_SelectPaletteBank(&(*bank)[obj->value], *p + 0xe6);
                    Sprite_Init(sprite, &(*bank)[obj->value], 0);
                    break;
                case 7:
                    bank = &mgr->paletteAnimationData[6];
                    AnimData_SelectPaletteBank(&(*bank)[obj->value], *p + 0xe6);
                    Sprite_Init(sprite, &(*bank)[obj->value], 0);
                    break;
            }
        }
        sprite->spritePos.words = obj->r0.words;
        if (!(*dir & 1)) {
            sprite->spritePos.position.x -= 0x20;
            sprite->spritePos.position.z += 0x20;
        } else {
            sprite->spritePos.position.x += 0x20;
            sprite->spritePos.position.z -= 0x20;
            *(u32 *)sprite |= 4;
        }
        obj->q0.words = sprite->spritePos.words;
        t = obj->variant;
        if (t == 0) {
            obj->callback = SceneObject_UpdateIdleA;
            SceneObjSlot_ResetPose(obj, 0);
        } else if (t == 1) {
            obj->callback = SceneObject_UpdateIdleB;
            SceneObjSlot_ResetPose(obj, 2);
        } else {
            obj->callback = SceneObject_UpdateIdleA;
            SceneObjSlot_ResetPose(obj, 0);
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
