#include "global.h"
struct Trap {
    u32 serial;
    u16 f4, f6, id, fa, flags, fe;
    u16 w[4], s[4];
    u32 f20, command;
    void *f28;
};
struct Context {
    u16 count;
    u8 pad[0x222];
    struct Trap traps[64];
    u8 *tops[64];
};
extern struct Context *gTerrainMap;
s32 Script_SeekToKeyword(s32);
void *Script_GetPc(void);
s32 Script_GetValue(void);
u32 sub_0821E2F8(void)
{
    s32 n = gTerrainMap->count;
    Script_SeekToKeyword('n');
    while (Script_GetPc()) {
        struct Trap *src = gTerrainMap->traps;
        s32 kept;
        s32 id;
        s32 i;
        u8 **tsrc;
        tsrc = gTerrainMap->tops;
        kept = 0;
        id = Script_GetValue();
        for (i = 0; i < n; i++, src++, tsrc++) {
            if (id != src->id) {
                gTerrainMap->traps[kept] = *src;
                gTerrainMap->tops[kept] = *tsrc;
                kept++;
            }
        }
        n = kept;
    }
    gTerrainMap->count = n;
    return 0;
}
