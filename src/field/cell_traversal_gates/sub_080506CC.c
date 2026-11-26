#include "field/cell_traversal_gates/cell_gate.h"
void CellGate_UpdatePassageBarrier(struct CellGate *);
void CellGate_UpdateForcedDirection(struct CellGate *);
void CellGate_UpdateConditionalBarrier(struct CellGate *);
s32 CellGate_InitializeFromScript(struct CellGate *gate, u32 id)
{
    struct ContactPackedVector position;

    s32 x, z;
    u32 value;
    u32 cell;
    gate->id = id;
    gate->registered = 0;
    if (Script_SeekToKeyword('t')) {
        value = Script_GetValue();
    } else {
        value = 9;
    }
    gate->flags = value;
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    {
        struct ContactPosition *coord = (void *)&position;
        {
            s16 *xp = (void *)&position;
            x = *xp >> 8;
        }
        z = coord->z >> 8;
    }
    if ((((x < 0) || (z < 0)) || (((u32)x) >= gTerrainWidth)) || (((u32)z) >= gTerrainHeight)) {
        cell = 0;
    } else {
        cell = gTerrainMap->rows[z] + x;
    }
    gate->cell = cell;
    gate->height = ((struct ContactPosition *)&position)->y >> 8;
    value = Script_SeekToKeyword('R');
    if (value) {
        value = Script_GetValue();
    }
    gate->scriptId = value;
    {
        u32 flags = gate->flags;
        if (flags & 1)
            gate->update = CellGate_UpdatePassageBarrier;
        else {
            u32 mask = 2;
            mask &= flags;
            if (mask)
                gate->update = CellGate_UpdateForcedDirection;
            else
                gate->update = CellGate_UpdateConditionalBarrier;
        }
    }
    return 0;
}
