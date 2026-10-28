#include "global.h"
#include "ram.h"
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
void Trap_Register(struct Trap *trap, u8 *top)
{
    s32 i, found, j;
    if (gTerrainMap->count > 63)
        return;
    if (!gTerrainTrapSerial)
        gTerrainTrapSerial = 1;
    trap->serial = gTerrainTrapSerial++;
    found = 0;
    i = gTerrainMap->count;
    while (i > 0) {
        if (trap->id == gTerrainMap->traps[i - 1].id) {
            found = 1;
            break;
        }
        i--;
    }
    if (!found) {
        gTerrainMap->traps[gTerrainMap->count] = *trap;
        gTerrainMap->tops[gTerrainMap->count] = top;
    } else {
        for (j = gTerrainMap->count; j > i; j--) {
            gTerrainMap->traps[j] = gTerrainMap->traps[j - 1];
            gTerrainMap->tops[j] = gTerrainMap->tops[j - 1];
        }
        gTerrainMap->traps[i] = *trap;
        gTerrainMap->tops[i] = top;
    }
    gTerrainMap->count++;
}
