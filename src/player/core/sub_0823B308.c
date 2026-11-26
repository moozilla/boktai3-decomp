#include "global.h"
struct SparkVector {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void SparkParticle_Spawn(void *, u32, const void *, u32, u32, u32, u32, u32);
// confidence=low: chooses a raised position or stored interaction position for the action spark.
#define Player_SpawnActionSpark sub_0823B308
void Player_SpawnActionSpark(u8 *p)
{
    struct SparkVector position;
    struct SparkVector size;
    if ((u8)(*(u8 *)(p + 0x418) - 3) <= 1) {
        position = *(struct SparkVector *)(p + 0x168);
        size.x = 32;
        size.y = 32;
        size.z = 32;
    } else {
        position = *(struct SparkVector *)(p + 0x30);
        position.y += 80;
        size.x = 64;
        size.y = 60;
        size.z = 64;
    }
    SparkParticle_Spawn(&position, 1, &size, 0, 0, 12, 2, 2);
    ++*(u16 *)(p + 0x442);
}
