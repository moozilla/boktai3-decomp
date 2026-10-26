#include "global.h"
#include "engine.h"
#include "script_reference.h"

struct Pos08227C2C {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 w : 16;
};
s32 Div(s32, s32);
s32 Mod(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);

s32 Script_TeleportToCell(void)
{
    struct Pos08227C2C pos;
    struct Pos08227C2C *p;
    struct ScriptUtilityReference ref;
    s32 cell = Script_GetValue();
    u8 *grid = gTerrainMap->cells;
    if (grid) {
        if (cell >= *(s16 *)(grid + 4) * *(s16 *)(grid + 6)) {
            pos.x = 0;
            pos.y = 0;
            pos.z = 0;
        } else {
            u8 *props;
            u32 type;
            u32 h;
            pos.x = Mod(cell, *(s16 *)(grid + 4)) * 256 + 128;
            pos.z = Div(cell, *(s16 *)(grid + 4)) * 256 + 128;
            p = &pos;
            props = TerrainModifiers_FindCellOverride(cell, 1);
            if (props)
                props += 4;
            else
                props = gTerrainMap->cells + (cell * 4 + 12);
            type = *props >> 4;
            h = (*props & 0xf) << 8;
            switch (type) {
                case 1:
                    h -= (u8)p->z;
                    break;
                case 2:
                    h -= (u8)p->x;
                    break;
            }
            pos.y = h;
        }
        Script_ReadReference(&ref);
        Script_WriteReferenceValue(&ref, 0, ((s16 *)&pos)[0]);
        Script_ReadReference(&ref);
        Script_WriteReferenceValue(&ref, 0, ((s16 *)&pos)[1]);
        Script_ReadReference(&ref);
        Script_WriteReferenceValue(&ref, 0, ((s16 *)&pos)[2]);
    }
    return 0;
}
