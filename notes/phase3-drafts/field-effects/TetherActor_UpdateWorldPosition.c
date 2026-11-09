void TetherActor_UpdateWorldPosition(struct TetherActor *actor)
{
    struct ContactPosition *position;
    u32 oldCell, oldSlope, oldHeight, oldAttributes;
    if (!actor->statusFlags && actor->state != 5) {
        s32 cell;
        u8 *properties;
        u32 surface, height;
        position = &actor->position;
        cell = cellIndex_0804AF80(position->x >> 8, position->z >> 8);
        properties = cellProperties(cell);
        surface = *properties >> 4;
        height = (*properties & 15) << 8;
        switch (surface) {
            case 1:
                height -= (u8)position->z;
                break;
            case 2:
                height -= (u8)position->x;
                break;
        }
        actor->position.y = height;
    }
    position = &actor->position;
    ((struct ContactPosition *)actor->display.animation.position)->x = actor->shakeX + position->x;
    ((struct ContactPosition *)actor->display.animation.position)->y =
        actor->displayOffsetY + position->y;
    ((struct ContactPosition *)actor->display.animation.position)->z = actor->shakeZ + position->z;
    Collider_SetPosition(&actor->chargeCollider, position, 0);
    oldCell = actor->movementProbeStorage.cell;
    oldSlope = actor->movementProbeStorage.slope;
    oldHeight = actor->movementProbeStorage.height;
    oldAttributes = actor->movementProbeStorage.attributes;
    if (actor->kind == 9) {
        if (!actor->movementProbe) {
            CellObject_Init(&actor->movementProbeStorage, position);
            actor->movementProbe = &actor->movementProbeStorage;
        }
        Obj_StepPosition(&actor->bodyConfig);
    } else {
        u8 *properties;
        actor->movementProbeStorage.cell = cellIndex_0804AF80(position->x >> 8, position->z >> 8);
        properties = TerrainModifiers_FindCellOverride(actor->movementProbeStorage.cell, 1);
        if (properties)
            properties += 4;
        else
            properties = gTerrainMap->cells + actor->movementProbeStorage.cell * 4 + 12;
        actor->movementProbeStorage.slope = *properties >> 4;
        actor->movementProbeStorage.height = *properties & 15;
        actor->movementProbeStorage.attributes = *(u16 *)(properties + 2);
    }
    actor->movementProbeStorage.previousCell = oldCell;
    actor->movementProbeStorage.previousSlope = oldSlope;
    actor->movementProbeStorage.previousHeight = oldHeight;
    actor->movementProbeStorage.previousAttributes = oldAttributes;
    if (actor->kind == 9)
        TetherActor_HandleFlaggedCell(actor);
    TetherActor_UpdateSampledPosition(actor);
    actor->targetPosition = actor->position;
    actor->targetPosition.pad = actor->heading;
}

