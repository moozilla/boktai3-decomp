void TetherActor_UpdateWorldPosition(struct TetherActor *actor)
{
    struct ContactPosition *position;
    u32 oldCell;
    u32 oldSlope;
    u32 oldHeight;
    u32 oldAttributes;
    u32 offset;
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
    {
        struct DisplayPosition **slot = &actor->display.animation.position;
        struct ContactPosition *display = (struct ContactPosition *)*slot;
        u16 value;
        u32 coordinate;
        position = &actor->position;
        offset = 0x37E;
        value = (u16)actor->shakeX;
        coordinate = (u16)position->x;
        display->x = value + coordinate;
        {
            struct ContactPosition *worldDisplay = (struct ContactPosition *)*slot;
            {
                u16 *axis = (u16 *)&actor->position.y;
                offset += 2;
                value = (u16)actor->displayOffsetY;
                coordinate = *axis;
                worldDisplay->y = value + coordinate;
            }
                worldDisplay = (struct ContactPosition *)*slot;
            {
                u16 *axis = (u16 *)&actor->position.z;
                value = (u16)actor->shakeZ;
                {
                    u32 z = *axis;
                    worldDisplay->z = value + z;
                }
            }
        }
        offset = 0x130;
        Collider_SetPosition(&actor->chargeCollider, position, 0);
    }
    oldCell = actor->movementProbeStorage.cell;
    oldSlope = actor->movementProbeStorage.slope;
    oldHeight = actor->movementProbeStorage.height;
    offset -= 2;
    oldAttributes = actor->movementProbeStorage.attributes;
    if (actor->kind == 9) {
        if (!actor->movementProbe) {
            CellObject_Init(&actor->movementProbeStorage, position);
            actor->movementProbe = &actor->movementProbeStorage;
        }
        Obj_StepPosition(&actor->bodyConfig);
    } else {
        u8 *properties;
        u32 currentCell;
        u32 index = cellIndex_0804AF80(position->x >> 8, position->z >> 8);
        {
            u16 *field = &actor->movementProbeStorage.cell;
            *field = index;
            currentCell = *field;
        }
        {
            u8 *raw = TerrainModifiers_FindCellOverride(currentCell, 1);
            if (raw)
                properties = raw + 4;
            else {
                struct EngineMap *map = gTerrainMap;
                u32 offset = currentCell * 4 + 12;
                properties = map->cells + offset;
            }
        }
        actor->movementProbeStorage.slope = *properties >> 4;
        {
            u32 byte = *properties;
            u32 height = 15;
            u32 heightOffset;
            height &= byte;
            heightOffset = 0x129;
            actor->movementProbeStorage.height = height;

        }
        actor->movementProbeStorage.attributes = *(u16 *)(properties + 2);
    }
    actor->movementProbeStorage.previousCell = oldCell;
    {
        u32 slopeOffset = 0x12A;
        u8 *destination = (u8 *)actor + slopeOffset;
        u32 sample = oldSlope;
        *destination = sample;
        {
            u32 heightOffset = 0x128;
            u32 height;
            destination = &actor->movementProbeStorage.previousHeight;
            height = oldHeight;
            *destination = height;
        }
        slopeOffset += 2;
        destination = (u8 *)actor + slopeOffset;
        sample = oldAttributes;
        *(u16 *)destination = sample;
    }
    if (actor->kind == 9)
        TetherActor_HandleFlaggedCell(actor);
    TetherActor_UpdateSampledPosition(actor);
    actor->targetPosition = actor->position;
    actor->targetPosition.pad = actor->heading;
}

