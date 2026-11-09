s32 Scene_TryStepDown(struct SceneObject *actor, s32 index, s32 direction)
{
    struct SceneCell *reference = &actor->cellRef.cell;
    s32 savedIndex, neighbor;
    s32 delta = 0, dx = 0, dz = 0, speed, step;
    struct SceneCell *cell;
    u8 *dynamic;
    if (({
            u8 active = 0;
            if (gPlayerPassageState && gPlayerInteractionMode == 2)
                active = 1;
            active;
        }) ||
        actor->type == 1) {
        step = 16;
        speed = 16;
    } else {
        step = 4;
        speed = 64;
    }
    switch (direction) {
        case 0:
            dz = -step;
            delta = -((s16 *)gTerrainMap->cells)[2];
            break;
        case 1:
            dx = step;
            delta = 1;
            break;
        case 2:
            dz = step;
            delta = ((s16 *)gTerrainMap->cells)[2];
            break;
        case 3:
            dx = -step;
            delta = -1;
            break;
    }
    neighbor = index + delta;
    dynamic = TerrainModifiers_FindCellOverride(neighbor, 1);
    savedIndex = neighbor;
    if (dynamic)
        cell = (struct SceneCell *)(dynamic + 4);
    else
        cell = (struct SceneCell *)(gTerrainMap->cells + 12 + savedIndex * 4);
    if (cell) {
        u32 heightByte = cell->height;
        u32 mask = 0xF0;
        s32 type = heightByte >> 4;
        switch (type) {
            case 0: {
                u32 lowMask = 15;
                u32 referenceHeight;
                s32 next;
                s32 current;
                referenceHeight = ({
                    struct SceneCell *p = reference;
                    p->height;
                });
                next = heightByte & lowMask;
                current = referenceHeight & lowMask;
                if ((u32)next >= (u32)current)
                    break;
                if (cell->flags & 8)
                    break;
                if (current - next > 1) {
                    actor->targetY = ({
                        u32 y = next << 8;
                        y;
                    });
                    actor->direction = ({
                        u32 d = direction;
                        d;
                    });
                    {
                        register void (*callback)(struct SceneObject *) = Scene_UpdateObjectStep;
                        actor->update = callback;
                    }
                    actor->timer = 0;
                    actor->entered = 1;
                    actor->dx = dx;
                    actor->dz = dz;
                    actor->kind = 2;
                    actor->cellRef.index = savedIndex;
                    actor->duration = speed;
                    return 3;
                } else {
                    u32 referenceHeight;
                    actor->targetY = ({
                        u32 y = next << 8;
                        y;
                    });
                    referenceHeight = ({
                        struct SceneCell *p = reference;
                        p->height;
                    });
                    {
                        register s32 remaining = referenceHeight & lowMask;
                        if (remaining > 0)
                            remaining--;
                        mask &= referenceHeight;
                        mask |= remaining;
                        reference->height = mask;
                    }
                    {
                        u32 tile = cell->tile;
                        reference->tile = tile;
                    }
                    actor->direction = ({
                        u32 d = direction;
                        d;
                    });
                    {
                        register void (*callback)(struct SceneObject *) = Scene_UpdateObjectStep;
                        actor->update = callback;
                    }
                    actor->timer = 0;
                    actor->entered = 1;
                    actor->dx = dx;
                    actor->dz = dz;
                    actor->kind = 1;
                    actor->cellRef.index = savedIndex;
                    actor->duration = speed;
                    return 1;
                }
            }
            default:
                if (type == 1 || type == 2)
                    return 0;
                break;
        }
    }
    return 0;
}
