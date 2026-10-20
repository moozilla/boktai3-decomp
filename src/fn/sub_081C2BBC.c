#include "global.h"

void SparkleEmitter_EmitParticle(u8 *);
void sub_081C2838(u8 *);

void SparkleEmitter_UpdateEmitting(u8 *p)
{
    u32 *c = (u32 *)(p + 0x770);
    if ((*c & 1) == 0) {
        SparkleEmitter_EmitParticle(p);
    }
    sub_081C2838(p);
    *c = *c + 1;
}
