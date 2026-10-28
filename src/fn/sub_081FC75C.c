#define gRandomTableIndex gRandomIndex
#include "global.h"
#include "ram.h"
struct SceneProjectile {
    u32 age;
    u32 flags;
    u8 pad08[0x22];
    u16 lifetime;
    u8 pad2C[0x8C];
    s32 position[3];
};
extern u32 gRandomTableIndex;
s32 Mod(s32, s32);
void SlotMotion_Initialize(u8 *);
void SceneParticles_SpawnAtPosition(u8 *, s32 *);
void Ratatosk_ConfigureParticle(u8 *, u16, u32, u8, u32, u32);
s32 SceneProjectile_EmitPeriodicParticle(struct SceneProjectile *p)
{
    u8 particle[0x18];
    u32 scale;
    u32 offset;
    if (p->age > p->lifetime)
        return 1;
    {
        u32 mask = 2;
        p->flags = mask | p->flags;
    }
    if (Mod(p->age, 8) == 0) {
        gRandomTableIndex = (gRandomTableIndex + 1) & 1023;
        offset = gRandomTableIndex * 2;
        scale = *(const u16 *)(offset + (u32)gRandomTable);
        scale &= 1;
        scale += 1;
        SlotMotion_Initialize(particle);
        Ratatosk_ConfigureParticle(particle, 40, 0, 0, 1, scale << 9);
        SceneParticles_SpawnAtPosition(particle, p->position);
    }
    return 0;
}
