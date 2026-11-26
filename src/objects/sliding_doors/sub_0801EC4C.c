#include "objects/sliding_doors/scene_object_controller.h"
#include "scene/motion/scene_actor_terrain.h"
void TerrainModifiers_RegisterCell(void *, u32, u32, u32, u32, u32);

// confidence=low: places a newly appended scene object on its terrain cell and starts its idle state.
void SlidingDoors_InitPlacement(struct SceneObjPool *mgr, struct SceneObjSlot *obj)
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
        cellField = &obj->cellHeight;
        obj->cellHeight = v;
        TerrainModifiers_RegisterCell(obj->terrain, obj->cellA, 0, obj->cellHeight, 0xff, 0);
    }
    {
        struct SceneObjSlot *sprite = obj;
        u8 *cf2;
        u32 type = obj->type;
        cf2 = cellField;
        switch (type) {
            case 0:
            case 3:
                Sprite_Init(sprite, &mgr->animationData[0][obj->animationIndex], 0);
                break;
            case 4:
                Sprite_Init(sprite, &mgr->animationData[3][obj->animationIndex], 0);
                break;
            case 5:
                Sprite_Init(sprite, &mgr->animationData[4][obj->animationIndex], 0);
                AnimData_SelectPaletteBank(&mgr->animationData[4][obj->animationIndex], 0x3e);
                break;
            case 6:
            case 7:
                Sprite_Init(sprite, &mgr->animationData[5][obj->animationIndex], 0);
                break;
            case 15:
                Sprite_Init(sprite, &mgr->animationData[6][obj->animationIndex], 0);
                break;
        }
        sprite->sprite.position = obj->r0.signedPosition;
        if (!(*dir & 1)) {
            sprite->sprite.position.x -= 0x20;
            sprite->sprite.position.z += 0x20;
        } else {
            sprite->sprite.position.x += 0x20;
            sprite->sprite.position.z -= 0x20;
            sprite->sprite.flags |= 4;
        }
        obj->q0.signedPosition = sprite->sprite.position;
        t = obj->variant;
        if (t == 0) {
            obj->callback = SlidingDoors_UpdateInactiveIdle;
            SceneObjSlot_ResetPose(obj, 0);
        } else if (t == 1) {
            obj->callback = SlidingDoors_UpdateActiveIdle;
            SceneObjSlot_ResetPose(obj, 2);
        } else {
            obj->callback = SlidingDoors_UpdateInactiveIdle;
            SceneObjSlot_ResetPose(obj, 0);
        }
        if (obj->variant == 0) {
            obj->terrain->cell.flags &= 0xfffd;
            obj->terrain->cell.flags |= 3;
            obj->terrain->cell.shapeAndHeight = (obj->terrain->cell.shapeAndHeight & 0xf0) | (*cf2 + 1);
        } else if (obj->variant == 1) {
            obj->terrain->cell.flags &= 0xfffc;
            obj->terrain->cell.flags |= 2;
            obj->terrain->cell.shapeAndHeight = (obj->terrain->cell.shapeAndHeight & 0xf0) | *cf2;
        }
    }
}
