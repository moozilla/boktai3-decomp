#include "engine/shared/global.h"
#include "engine/shared/engine_globals.h"
#include "engine/shared/ram.h"
struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void SparkParticle_Spawn(void *, int, void *, int, int, int, int, int);
void SceneActors_SpawnDestructionParticles(u8 *p)
{
    struct Packed size;
    int angle;
    int speed;
    int count;
    u16 *table;
    u32 *indexPtr;
    u32 smallMask;
    u32 *alias;
    u32 index;
    size.x = 164;
    size.y = 16;
    size.z = 164;
    table = gRandomTable;
    indexPtr = &gRandomIndex;
    index = *indexPtr;
    index++;
    index &= 1023;
    angle = (u8)table[index];
    index = (index + 1) & 1023;
    count = table[index];
    smallMask = 3;
    count = (count & smallMask) + 2;
    index = (index + 1) & 1023;
    *indexPtr = index;
    alias = &index;
    speed = (table[*alias] & smallMask) + 20;
    SparkParticle_Spawn(p + 0x260, 3, &size, angle, count, speed, 2, 2);
    {
        u32 *ptr = indexPtr;
        index = ((*ptr) + 1) & 1023;
    }
    angle = (u8)table[*alias];
    index = (index + 1) & 1023;
    count = (table[index] & smallMask) + 2;
    index = (index + 1) & 1023;
    *indexPtr = index;
    speed = (table[*alias] & smallMask) + 20;
    SparkParticle_Spawn(p + 0x260, 3, &size, angle, count, speed, 2, 2);
}
