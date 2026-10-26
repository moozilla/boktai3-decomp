#include "global.h"
#include "engine.h"

extern const u16 gUnk_0203B400[];
s32 Mod(s32, s32);
void ActorMarker_EmitIndexedEffect(u8 *, u32);

void Armor_SpawnRandomDirectionalParticle(u8 *p)
{
    u32 n;
    gRandomIndex = n = (gRandomIndex + 1) & 0x3FF;
    ActorMarker_EmitIndexedEffect(p, Mod(*(u16 *)((u8 *)gUnk_0203B400 + n * 2), 10));
}
