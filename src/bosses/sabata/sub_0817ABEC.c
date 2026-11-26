#include "global.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
#include "scene/actor/contact_position.h"
/* SceneActors_SpawnDestructionParticles_0817ABEC: same as SceneActors_SpawnDestructionParticles but spawns the sparks on the object itself (provisional name) */
void SparkParticle_Spawn(void *, int, void *, int, int, int, int, int);
void SceneActors_SpawnDestructionParticles_0817ABEC(u8 *p)
{
    struct ContactPosition size;
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
    SparkParticle_Spawn(p, 3, &size, angle, count, speed, 2, 2);
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
    SparkParticle_Spawn(p, 3, &size, angle, count, speed, 2, 2);
}
