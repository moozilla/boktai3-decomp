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
void Trap_Register(struct Trap *trap, u8 *top)
{
    s32 i, found, j;
    if (((struct Context *)gTerrainMap)->count > 63)
        return;
    if (!gTerrainTrapSerial)
        gTerrainTrapSerial = 1;
    trap->serial = gTerrainTrapSerial++;
    found = 0;
    i = ((struct Context *)gTerrainMap)->count;
    while (i > 0) {
        if (trap->id == ((struct Context *)gTerrainMap)->traps[i - 1].id) {
            found = 1;
            break;
        }
        i--;
    }
    if (!found) {
        ((struct Context *)gTerrainMap)->traps[((struct Context *)gTerrainMap)->count] = *trap;
        ((struct Context *)gTerrainMap)->tops[((struct Context *)gTerrainMap)->count] = top;
    } else {
        for (j = ((struct Context *)gTerrainMap)->count; j > i; j--) {
            ((struct Context *)gTerrainMap)->traps[j] =
                ((struct Context *)gTerrainMap)->traps[j - 1];
            ((struct Context *)gTerrainMap)->tops[j] = ((struct Context *)gTerrainMap)->tops[j - 1];
        }
        ((struct Context *)gTerrainMap)->traps[i] = *trap;
        ((struct Context *)gTerrainMap)->tops[i] = top;
    }
    ((struct Context *)gTerrainMap)->count++;
}
