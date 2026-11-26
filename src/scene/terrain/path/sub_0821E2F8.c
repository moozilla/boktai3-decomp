#include "objects/warp_circle/engine_map.h"
#include "global.h"
#include "engine/engine_globals.h"
s32 Script_SeekToKeyword(s32);
void *Script_GetPc(void);
s32 Script_GetValue(void);
u32 sub_0821E2F8(void)
{
    s32 n = gTerrainMap->trapCount;
    Script_SeekToKeyword('n');
    while (Script_GetPc()) {
        struct TerrainTrap *src = gTerrainMap->traps;
        s32 kept;
        s32 id;
        s32 i;
        void **tsrc;
        tsrc = gTerrainMap->trapTops;
        kept = 0;
        id = Script_GetValue();
        for (i = 0; i < n; i++, src++, tsrc++) {
            if (id != src->id) {
                gTerrainMap->traps[kept] = *src;
                gTerrainMap->trapTops[kept] = *tsrc;
                kept++;
            }
        }
        n = kept;
    }
    gTerrainMap->trapCount = n;
    return 0;
}
