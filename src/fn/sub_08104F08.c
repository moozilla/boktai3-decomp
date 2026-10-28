#define gShakeRandomIndex gRandomIndex
#include "global.h"
#include "callback_pool.h"
#include "engine_globals.h"
#include "ram.h"
s32 Mod(s32, s32);
static inline u16 CallbackPool_NextRandom(void)
{
    u16 *table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return table[gRandomIndex];
}
void CallbackPool_UpdateShake(struct CallbackPoolEntry *entry)
{
    if (entry->shakeTimer == 0) {
        entry->position = entry->savedPosition;
    } else {
        entry->position.x = entry->savedPosition.x + Mod(CallbackPool_NextRandom(), 20) - 10;
        entry->position.z = entry->savedPosition.z + Mod(CallbackPool_NextRandom(), 20) - 10;
        entry->shakeTimer--;
    }
}
