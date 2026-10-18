#include "global.h"

void ActorMarker_EmitIndexedEffect(u32, u32);

void Armor_SpawnDirectionalParticleFan(u32 a)
{
    s32 i = 0;
    do {
        ActorMarker_EmitIndexedEffect(a, i);
        i++;
    } while (i <= 8);
}
