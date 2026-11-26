#include "effects/particles/burst_emitter.h"
/* SparkExplosion_TickIdle: No-op callback for the emitter idle state; parallels SparkExplosion_TickActive. */
#define SparkExplosion_TickIdle  sub_08069C94
#define SparkExplosion_StateIdle sub_08069C94
void SparkExplosion_StateIdle(struct BurstEmitterPool *unusedPool, struct BurstEmitter *unusedEmitter)
{
}
