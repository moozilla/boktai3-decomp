#include "global.h"

/* tentative: spawns a jittered pair of particles moving left and right */
#define ActorParticles_SpawnSideBurst sub_0811C168

struct ParticleOffset {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 unused : 16;
};
struct Actor {
    u8 pad0[0x4a];
    u8 field4A;
    u8 field4B;
};
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
u32 sub_081134B0(struct Actor *, struct ParticleOffset *, struct ParticleOffset *, u32, u32, u32,
                 u32, u32, u32, u32, u32, u32, u32, u32, u32);
static inline u32 ParticleRandom(void)
{
    gUnk_03005304 = (gUnk_03005304 + 1) & 1023;
    return gUnk_086141BC[gUnk_03005304];
}
/* Spawns two 0x3D99 particles from the actor: a shared jittered base offset
   (y = -32) and two velocity offsets, one leftward then one rightward (tentative). */
void ActorParticles_SpawnSideBurst(struct Actor *p)
{
    struct ParticleOffset a, b;
    u32 anim = p->field4B + 0x378;
    a.x = (ParticleRandom() & 31) - 7;
    a.y = -32;
    a.z = 0;
    b.x = -(ParticleRandom() & 3) - 3;
    b.y = -(ParticleRandom() & 7) - 3;
    b.z = (ParticleRandom() & 7) - 3;
    {
        u8 layer = p->field4A;
        sub_081134B0(p, &a, &b, 0x3d99, 0, layer, 1, 0, 0, anim, 0x30, 1, 0, 2, 6);
    }
    b.x = (ParticleRandom() & 3) + 2;
    b.y = -(ParticleRandom() & 7) - 2;
    b.z = (ParticleRandom() & 7) - 3;
    {
        u8 layer = p->field4A;
        sub_081134B0(p, &a, &b, 0x3d99, 0, layer, 1, 0, 0, anim, 0x30, 1, 0, 2, 6);
    }
}
