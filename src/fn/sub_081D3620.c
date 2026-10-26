// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "ram.h"
struct CloudPosition {
    u16 x, y, z, pad;
};
struct CloudParticle {
    u8 pad0[0x14];
    struct CloudPosition origin;
    struct CloudPosition *anchor;
    u8 pad20[0x28];
};
struct CloudTask {
    u8 pad0[0x30];
    struct CloudParticle particles[40];
};
// confidence=low: scatters particle anchors horizontally and raises them by sixteen units.
#define PlayerParticleCloud_ScatterAnchors sub_081D3620
s32 PlayerParticleCloud_ScatterAnchors(void)
{
    struct CloudTask *p = gCloudTask;
    struct CloudParticle *particle;
    register const u16 *table asm("r5");
    s32 i;
    s32 bias;
    u32 *indexPtr;
    u32 mask;
    if (!p)
        return -1;
    particle = p->particles;
    table = (const u16 *)0x0203B400;
    indexPtr = &gRandomIndex;
    mask = 1023;
    bias = -512;
    for (i = 39; i >= 0; i--, particle++) {
        u32 index;
        register u32 random asm("r0");
        register s32 shiftedX asm("r2");
        s32 shiftedZ;
        index = *indexPtr;
        index++;
        index &= mask;
        *indexPtr = index;
        random = table[index];
        shiftedX = particle->origin.x + bias;
        random &= mask;
        shiftedX += random;
        particle->origin.x = shiftedX;
        index = (index + 1) & mask;
        *indexPtr = index;
        index <<= 1;
        index += (u32)table;
        random = *(const u16 *)index;
        shiftedZ = particle->origin.z + bias;
        random &= mask;
        shiftedZ += random;
        particle->origin.z = shiftedZ;
        particle->origin.y += 4096;
        particle->anchor = &particle->origin;
    }

    return 0;
}
