void TetherActor_BlockPlayerAcrossTerrain(struct TetherActor *actor)
{
    u8 *properties = cellProperties(
        cellIndex_0804B6A4(gPlayers[0]->position.x >> 8, gPlayers[0]->position.z >> 8));
    u32 attributes = *(u16 *)(properties + 2);
    struct ContactPosition *position;
    s32 playerCell;
    s32 actorCell;
    s32 rowStride;
    attributes &= 1;
    position = &actor->position;
    if (attributes == 0) {
        struct TetherActor *current = actor;
        s32 playerHeight = surfaceHeight((const struct ContactPosition *)&gPlayers[0]->position);
        s32 height, difference;
        {
            struct ContactPosition *heightPosition = &current->position;
            s32 x = heightPosition->x >> 8, z = heightPosition->z >> 8;
            u8 *properties;
            u32 surface;
            position = heightPosition;
            properties = cellProperties(cellIndex_0804B6A4(x, z));
            surface = *properties >> 4;
            height = (*properties & 15) << 8;
            switch (surface) {
                case 1:
                    height -= (u8)heightPosition->z;
                    break;
                case 2:
                    height -= (u8)heightPosition->x;
                    break;
            }
        }
        difference = playerHeight - height;
        if (difference < 0)
            difference = -difference;
        if (difference <= 255)
            return;
    }
    {
        s32 x;
        s32 z;
        u32 raw;
        raw = (u16)gPlayers[0]->position.x;
        x = (s16)raw >> 8;
        raw = (u16)gPlayers[0]->position.z;
        z = (s16)raw >> 8;
        playerCell = cellIndex_0804B6A4(x, z);
        raw = (u16)position->x;
        x = (s16)raw >> 8;
        raw = (u16)position->z;
        z = (s16)raw >> 8;
        actorCell = cellIndex_0804B6A4(x, z);
    }
    if (actorCell == playerCell + 1) {
        gPlayers[0]->position.x = ((position->x >> 8) << 8) + 2;
        return;
    }
    if (actorCell == playerCell - 1) {
        gPlayers[0]->position.x = (((position->x >> 8) + 1) << 8) - 2;
        return;
    }
    rowStride = *(s16 *)(gTerrainMap->cells + 4);
    if (actorCell == playerCell + rowStride)
        gPlayers[0]->position.z = ((actor->position.z >> 8) << 8) + 2;
    else if (actorCell == playerCell - rowStride)
        gPlayers[0]->position.z = (((actor->position.z >> 8) + 1) << 8) - 2;
    else {
        gPlayers[0]->position.x = position->x;
        gPlayers[0]->position.z = actor->position.z;
    }
}

