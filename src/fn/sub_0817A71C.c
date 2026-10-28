// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine_globals.h"
#include "ram.h"
struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void SparkParticle_Spawn(void *, int, void *, int, int, int, int, int);
void SceneActors_EmitRandomBursts(u8 *p)
{
    u8 *graphics = p + 0x2CC;
    struct Packed size;
    int i;
    for (i = 0; i < 2; i++) {
        int angle, speed;
        int count;
        register u32 index asm("r1");
        u32 *indexPtr;
        u32 mask;
        int smallMask;
        register u16 *table asm("r4");
        size.x = 164;
        size.y = 16;
        size.z = 164;
        indexPtr = &gRandomIndex;
        mask = 1023;
        smallMask = 3;
        table = gRandomTable;
        index = (*indexPtr + 1) & mask;
        angle = (u8)table[index];
        index = (index + 1) & mask;
        count = (table[index] & smallMask) + 2;
        index = (index + 1) & mask;
        *indexPtr = index;
        speed = (table[index] & smallMask) + 20;
        SparkParticle_Spawn(graphics + 0x20, 3, &size, angle, count, speed, 2, 2);
    }
}
