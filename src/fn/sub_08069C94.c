#include "burst_emitter.h"
/* BurstEmitter_TickIdle: No-op callback for the emitter idle state; parallels BurstEmitter_TickActive. */
#define BurstEmitter_TickIdle  sub_08069C94
#define BurstEmitter_StateIdle sub_08069C94
void BurstEmitter_StateIdle(struct BurstEmitterPool *unusedPool, struct BurstEmitter *unusedEmitter)
{
}
