// MATCHING_CONCESSION: register-pinning
#include "spark_particle.h"
#define SparkParticle_CreateFromScript sub_0806A420
union SparkParticleVector {
    struct ContactPackedVector packed;
    struct ContactPosition position;
};
void SparkParticle_CreateFromScript(void)
{
    u32 zero;
    union SparkParticleVector position, spread;
    union SparkParticleVector *positionVector;
    register const struct ContactPosition *positionArg asm("r9");
    u32 hasSpread;
    register const struct ContactPosition *spreadArg asm("r8");
    s32 angle, speed, kind, priority;
    u8 count;
    asm("" : "=m"(zero));
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
            const void *source;
            register void *target asm("r1");
            zero = 0;
            source = &zero;
            target = destination;
            asm("" : : "r"(source), "r"(target) : "r2");
            CpuSet(source, target, 0x5000002);
            spreadArg = &destination->position;
        }
        angle = Script_GetKeywordValue('d', 0);
        speed = Script_GetKeywordValue('s', 0);
        kind = Script_GetKeywordValue('k', 1);
        priority = Script_GetKeywordValue('r', 2);
        sub_0806A160(positionArg, count, spreadArg, angle, speed, 0, kind, priority);
    }
}
