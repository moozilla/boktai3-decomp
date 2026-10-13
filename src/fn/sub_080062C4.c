#include "terrain_modifier.h"

s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u32 sub_0821ABA8(u32, u32);

u32 sub_080062C4(void)
{
    struct TerrainScriptVec bf;
    u32 a = sub_0821ABA8(0x6e, 0);
    u32 b = sub_0821ABA8(0x74, 0);
    u32 c = sub_0821ABA8(0x66, 0);
    u32 d = sub_0821ABA8(0x65, 0);
    if (Script_SeekToKeyword(0x70)) {
        bf.x = Script_GetValue();
        bf.y = Script_GetValue();
        bf.z = Script_GetValue();
    } else {
        bf.x = 0;
        bf.y = 0;
        bf.z = 0;
    }
    TerrainModifiers_Create(a, (struct TerrainVec *)&bf, c, b, d);
    return 0;
}
