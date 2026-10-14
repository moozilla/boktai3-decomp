// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
#define TetherActor_EmitPointEffect sub_08048CB0
void sub_080154EC(u32, void *, u32);
union TetherPointPacket {
    struct ContactPackedVector bits;
    u32 words[2];
};
void TetherActor_EmitPointEffect(const struct ContactPackedVector *source, u32 kind)
{
    union TetherPointPacket position;
    const u16 *table;
    register u32 *state asm("r1");
    register u32 *savedState asm("r12");
    register u32 index asm("r3");
    u32 indexMask;
    s32 sampleX;
    s32 sampleZ;
    register s32 sampleY asm("r3");
    register u32 jitterMask asm("r9");
    register u32 upperMask asm("r10");
    register u32 xy asm("r5");
    s32 x;
    register s32 z asm("r1");
    s32 y;
    u32 highY;
    position.bits = *source;
    table = (const u16 *)RANDOM_TABLE_ADDR;
    state = &gUnk_03005308;
    savedState = state;
    index = *state + 1;
    indexMask = 1023;
    index &= indexMask;
    sampleX = table[index];
    x = (u16)position.bits.x - 15;
    jitterMask = 31;
    asm("" : "+r"(jitterMask));
    x += sampleX & jitterMask;
    x = (u16)x;
    upperMask = 0xFFFF0000;
    xy = (position.words[0] & upperMask) | x;
    position.words[0] = xy;
    index = (index + 1) & indexMask;
    sampleZ = table[index];
    z = (u16)position.bits.z - 15;
    z += sampleZ & jitterMask;
    z = (u16)z;
    asm volatile("" : "+r"(upperMask) : "r"(z) : "memory");
    position.words[1] = (position.words[1] & upperMask) | (u16)z;
    index = (index + 1) & indexMask;
    *savedState = index;
    sampleY = table[index];
    y = ((s32)xy >> 16) - 16;
    sampleY &= 63;
    y -= sampleY;
    highY = (u32)y << 16;
    position.words[0] = (xy & 0xFFFF) | highY;
    sub_080154EC(1, &position, kind);
}
