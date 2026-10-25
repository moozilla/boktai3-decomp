#include "global.h"

void SparkleEmitter_EmitParticle(u8 *);
void SparkleEmitter_UpdateParticles(u8 *);

void SparkleEmitter_UpdateEmitting(u8 *p)
{
    u32 *c = (u32 *)(p + 0x770);
    if ((*c & 1) == 0) {
        SparkleEmitter_EmitParticle(p);
    }
    SparkleEmitter_UpdateParticles(p);
    *c = *c + 1;
}
