#include "global.h"

void SparkleEmitter_UpdateParticles(u8 *);

void SparkleEmitter_UpdateDraining(u8 *p)
{
    SparkleEmitter_UpdateParticles(p);
    *(u32 *)(p + 0x770) += 1;
}
