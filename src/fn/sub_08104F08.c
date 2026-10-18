#define CallbackPool_UpdateShake sub_08104F08
#define gShakeRandomTable gUnk_0203B400
#define gShakeRandomIndex gUnk_03005308
#include "global.h"
#include "callback_pool.h"
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
s32 Mod(s32, s32);
static inline u16 CallbackPool_NextRandom(void)
{
    u16 *table = gUnk_0203B400;
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
    return table[gUnk_03005308];
}
void sub_08104F08(struct CallbackPoolEntry *entry)
{
    if (entry->shakeTimer == 0) {
        entry->position = entry->savedPosition;
    } else {
        entry->position.x = entry->savedPosition.x + Mod(CallbackPool_NextRandom(), 20) - 10;
        entry->position.z = entry->savedPosition.z + Mod(CallbackPool_NextRandom(), 20) - 10;
        entry->shakeTimer--;
    }
}
