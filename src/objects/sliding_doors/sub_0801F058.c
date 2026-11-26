#include "objects/sliding_doors/scene_object_controller.h"
#include "scene/motion/scene_actor_terrain.h"
void TerrainModifiers_RegisterCell(void *, u32, u32, u32, u32, u32);
void SlidingDoor_UpdateRaised(struct SceneObjSlot *);
void SlidingDoor_UpdateLowered(struct SceneObjSlot *);
void *SceneParticleBurst_EnsureSingleton(u32, u32);
// confidence=low: places a raisable scene object (blocks/gates with 10 facings) on its terrain cell.
void SlidingDoor_InitLiftPlacement(struct SceneObjPool *mgr, struct SceneObjSlot *obj)
{
    s32 cell = cellIndexAt(&obj->pos.position);
    u8 *dir;
    struct SceneObjSlot *sprite;
    s32 d;
    d = obj->dir;
    dir = &obj->dir;
    switch (d) {
        case 0:
            obj->cellA = cell - 1;
            obj->cellB = cell;
            obj->pos.position.z += 0x80;
            obj->loweredPose = 1;
            break;
        case 1:
            obj->cellA = cell - (u16)((struct TerrainGrid *)gTerrainMap->cells)->width;
            obj->cellB = cell;
            obj->pos.position.x += 0x80;
            obj->loweredPose = 1;
            break;
        case 2:
            obj->cellA = cell;
            obj->cellB = cell - 1;
            obj->pos.position.z += 0x80;
            obj->loweredPose = 2;
            break;
        case 3:
            obj->cellA = cell;
            obj->cellB = cell - (u16)((struct TerrainGrid *)gTerrainMap->cells)->width;
            obj->pos.position.x += 0x80;
            obj->loweredPose = 2;
            break;
        case 4:
            obj->cellA = cell - 1;
            obj->cellB = cell;
            obj->pos.position.z += 0x80;
            obj->loweredPose = 0;
            break;
        case 5:
            obj->cellA = cell - (u16)((struct TerrainGrid *)gTerrainMap->cells)->width;
            obj->cellB = cell;
            obj->pos.position.x += 0x80;
            obj->loweredPose = 0;
            break;
        case 6:
            obj->cellA = cell;
            obj->cellB = cell - 1;
            obj->pos.position.z += 0x80;
            obj->loweredPose = 1;
            break;
        case 7:
            obj->cellA = cell;
            obj->cellB = cell - (u16)((struct TerrainGrid *)gTerrainMap->cells)->width;
            obj->pos.position.x += 0x80;
            obj->loweredPose = 1;
            break;
        case 8:
            obj->cellA = cell;
            obj->cellB = cell - 1;
            obj->pos.position.z += 0x80;
            obj->loweredPose = 0;
            break;
        case 9:
            obj->cellA = cell;
            obj->cellB = cell - (u16)((struct TerrainGrid *)gTerrainMap->cells)->width;
            obj->pos.position.x += 0x80;
            obj->loweredPose = 0;
            break;
    }
    obj->cellHeight = *SceneActorTerrain_GetCell(obj->cellA) & 15;
    TerrainModifiers_RegisterCell(obj->terrain, obj->cellA, 0, obj->cellHeight, 0xff, 0);
    obj->pose = *(u8 *)&obj->loweredPose;
    obj->poseFrame = 0;
    sprite = obj;
    Sprite_Init(sprite, &mgr->animationData[1][obj->animationIndex], 0);
    obj->sprite.position = obj->r0.signedPosition;
    if (!(*dir & 1)) {
        obj->sprite.position.z += 0x80;
    } else {
        obj->sprite.position.x += 0x80;
        obj->sprite.flags |= 4;
    }
    obj->q0.signedPosition = sprite->sprite.position;
    if (obj->variant == 0)
        obj->callback = SlidingDoor_UpdateLowered;
    else if (obj->variant == 1)
        obj->callback = SlidingDoor_UpdateRaised;
    else
        obj->callback = SlidingDoor_UpdateLowered;
    if (obj->variant == 0) {
        obj->timer = 0;
        obj->variant = 1;
        obj->callback = SlidingDoor_UpdateLowered;
        obj->pose = *(u8 *)&obj->loweredPose;
        obj->poseFrame = 0;
        obj->terrain->cell.flags &= 0xfffd;
        obj->terrain->cell.flags |= 3;
        obj->terrain->flags &= 0xfffe;
        obj->terrain->cell.shapeAndHeight =
            (obj->terrain->cell.shapeAndHeight & 0xf0) | (obj->cellHeight + 1);
        obj->lift = 0;
        {
            u16 t = obj->r0.position.y;
            obj->q0.position.y = t;
        }
    } else if (obj->variant == 1) {
        obj->timer = 0;
        obj->variant = 0;
        obj->callback = SlidingDoor_UpdateRaised;
        obj->pose = 0;
        obj->poseFrame = 0;
        obj->terrain->cell.flags &= 0xfffc;
        obj->terrain->cell.flags |= 2;
        obj->terrain->flags |= 1;
        obj->terrain->cell.shapeAndHeight =
            (obj->terrain->cell.shapeAndHeight & 0xf0) | obj->cellHeight;
        {
            u16 t = obj->maxLift;
            obj->lift = t;
        }
        {
            u16 t = obj->r0.position.y + obj->maxLift;
            obj->q0.position.y = t;
        }
    }
    obj->raisedScript = 0;
    obj->loweredScript = 0;
    SceneParticleBurst_EnsureSingleton(0, 0);
}
