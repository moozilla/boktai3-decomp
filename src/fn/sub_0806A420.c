#include "spark_particle.h"
/* SparkParticle_CreateFromScript: Parses particle count and position plus optional spread and motion settings from script keywords and calls the particle creator */
#define SparkParticle_CreateFromScript sub_0806A420
union SparkParticleVector {
    struct ContactPackedVector packed;
    struct ContactPosition position;
};
static inline void ClearVector(void *destination, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, destination, 0x05000002);
}
void SparkParticle_CreateFromScript(void)
{
    u8 zero[4];
    union SparkParticleVector position, spread;
    union SparkParticleVector *positionVector;
    const struct ContactPosition *positionArg;
    u32 hasSpread;
    const struct ContactPosition *spreadArg;
    s32 angle, speed, kind, priority;
    u8 count;
    count = Script_GetKeywordValue('n', 0);
    if (count && Script_SeekToKeyword('p')) {
        position.packed.x = Script_GetValue();
        position.packed.y = Script_GetValue();
        {
            u32 z = (u16)Script_GetValue();
            position.packed.z = z;
            positionVector = &position;
        }
        hasSpread = Script_SeekToKeyword('o');
        positionArg = &positionVector->position;
        if (hasSpread) {
            spread.packed.x = Script_GetValue();
            spread.packed.y = Script_GetValue();
            spread.packed.z = Script_GetValue();
            spreadArg = &spread.position;
        } else {
            union SparkParticleVector *destination = &spread;
            ClearVector(destination, (u32 *)zero);
            spreadArg = &destination->position;
        }
        angle = Script_GetKeywordValue('d', 0);
        speed = Script_GetKeywordValue('s', 0);
        kind = Script_GetKeywordValue('k', 1);
        priority = Script_GetKeywordValue('r', 2);
        SparkParticle_Spawn(positionArg, count, spreadArg, angle, speed, 0, kind, priority);
    }
}
