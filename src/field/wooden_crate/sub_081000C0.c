#define gCrateRandomIndex gRandomIndex
#include "engine/shared/global.h"
#include "engine/shared/engine_globals.h"
#include "engine/shared/ram.h"
struct CratePosition {
    u16 x, xHigh, y, yHigh;
};
struct WoodenCrate {
    u8 pad0[0x1C];
    struct CratePosition position;
    u8 pad24[0x58 - 0x24];
    struct CratePosition basePosition;
    u8 pad60[0xD5 - 0x60];
    u8 shakeTimer;
};
s32 Mod(s32, s32);
void WoodenCrate_UpdateShake(struct WoodenCrate *p)
{
    if (!p->shakeTimer)
        p->position = p->basePosition;
    else {
        p->position.x = p->basePosition.x +
                        Mod(gRandomTable[gRandomIndex = (gRandomIndex + 1) & 0x3FF], 20) - 10;
        p->position.y = p->basePosition.y +
                        Mod(gRandomTable[gRandomIndex = (gRandomIndex + 1) & 0x3FF], 20) - 10;
        p->shakeTimer--;
    }
}
