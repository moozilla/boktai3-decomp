#include "global.h"
#include "engine_globals.h"
// confidence=low: spawns six spark particles around the actor and plays a sound.
extern u8 gUnk_0203B400[];
void SparkParticle_Spawn(u8 *, u32, void *, u32, u32, u32, u32, u32);
void Sound_PlaySong(u32);
struct VAB {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};
static inline u32 rnd(u8 *tbl, s32 *idx)
{
    return *(u16 *)(((*idx = (*idx + 1) & 0x3ff) << 1) + (u32)tbl);
}
void TentacleActor_SpawnSparkBurst(u8 *p)
{
    struct VAB v;
    u8 *tbl;
    s32 *idx;
    v.x = 0x50;
    v.y = 0x3c;
    v.z = 0x3c;
    tbl = gUnk_0203B400;
    asm("" : "+r"(tbl));
    idx = &gRandomIndex;
    SparkParticle_Spawn(p, (rnd(tbl, idx) & 3) + 1, &v, 0x20, 4, 20, 2, 1);
    SparkParticle_Spawn(p, (rnd(tbl, idx) & 1) + 1, &v, 0x40, 4, 20, 2, 1);
    SparkParticle_Spawn(p, (rnd(tbl, idx) & 3) + 1, &v, 0x5e, 4, 20, 2, 1);
    SparkParticle_Spawn(p, (rnd(tbl, idx) & 1) + 1, &v, 0x80, 4, 20, 2, 1);
    SparkParticle_Spawn(p, (rnd(tbl, idx) & 3) + 1, &v, 0xc4, 4, 20, 2, 1);
    SparkParticle_Spawn(p, (rnd(tbl, idx) & 1) + 1, &v, 0xf0, 4, 20, 2, 1);
    Sound_PlaySong(0xe5);
}
