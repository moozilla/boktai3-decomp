#include "global.h"
#include "engine.h"
#include "ram.h"

s32 Mod(s32, s32);
void ActorMarker_EmitIndexedEffect(u8 *, u32);

void Armor_SpawnRandomDirectionalParticle(u8 *p)
{
    u32 n;
    gRandomIndex = n = (gRandomIndex + 1) & 0x3FF;
    ActorMarker_EmitIndexedEffect(p, Mod(*(u16 *)((u8 *)gRandomTable + n * 2), 10));
}
