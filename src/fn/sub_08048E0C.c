// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
#define TetherActor_BlendPalette sub_08048E0C
void TetherActor_BlendPalette(struct TetherActor *s)
{
    u16 *a = (u16 *)(gUnk_030042E4 + (s->paletteIndex + 0x283) * 32);
    u16 *b = (u16 *)(gUnk_030042E4 + 0x5160);
    s32 inv = 64 - s->paletteBlend;
    s32 weight = s->paletteBlend;
    s32 i = 0;
    register u32 componentMask asm("r8") = 31;
    u32 redMask;
    redMask = 31;
    do {
        u16 c = *a;
        s32 r = redMask & c;
        u32 packedC = (u32)c << 16;
        s32 g = (packedC >> 21) & componentMask;
        s32 bl = (packedC >> 26) & componentMask;
        u16 d = *b;
        s32 rr = redMask & d;
        u32 packedD = (u32)d << 16;
        s32 gg = (packedD >> 21) & componentMask;
        s32 bb = (packedD >> 26) & componentMask;
        s32 red = ((inv * r + weight * rr) >> 6) & redMask;
        s32 green = ((inv * g + weight * gg) >> 6) & redMask;
        s32 blue = ((inv * bl + weight * bb) >> 6) & redMask;
        s->palette[i] = red | (green << 5) | (blue << 10);
        a++;
        b++;
    } while (++i <= 15);
}