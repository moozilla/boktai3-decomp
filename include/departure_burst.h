#ifndef GUARD_DEPARTURE_BURST_H
#define GUARD_DEPARTURE_BURST_H

#include "ram.h"
#include "scene_departure_display.h"

extern const u16 gUnk_086141BC[];

static inline u16 DepartureBurst_NextBit(void)
{
    u32 sample;
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    sample = gUnk_086141BC[gEffectRandomTableIndex];
    return sample & 1;
}

void ActorEffect_SpawnRandomizedBurst(struct DepartureActor *, u32);
void ActorEffect_SpawnBurstAtAnchor(struct DepartureActor *, u32);

#endif
