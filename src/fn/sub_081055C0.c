#include "global.h"
#include "directional_interaction.h"
#include "callback_pool.h"

// MATCHING_CONCESSION: register-pinning
#define CallbackPool_RunSpawnScript ProximityActor_SetState
#define gShakeRandomTable           gUnk_0203B400
#define gShakeRandomIndex           gUnk_03005308
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
static inline u32 NextRandom(u16 *table)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
    return *(u16 *)((gUnk_03005308 << 1) + (u32)table);
}
void ProximityActor_SetState(struct CallbackPoolEntry *entry, s32 count)
{
    s32 values[4];
    struct DirectionalScriptArgs args;
    s32 i;
    if (count > 0 && entry->spawnScript) {
        args.count = 4;
        args.values = values;
        for (i = 0; i < count; i++) {
            register u16 *table asm("r3") = gUnk_0203B400;
            {
                u32 random = NextRandom(table);
                s32 position = entry->position.x - 64;
                random &= 127;
                values[0] = position + random;
            }
            {
                u32 random = NextRandom(table);
                s32 position = entry->position.y - 64;
                random &= 127;
                values[1] = position + random;
            }
            {
                u32 random = NextRandom(table);
                s32 position = entry->position.z - 64;
                random &= 127;
                values[2] = position + random;
            }
            values[3] = entry->spawnIndex;
            Script_Call(entry->spawnScript, &args);
            entry->spawnIndex++;
        }
    }
}
