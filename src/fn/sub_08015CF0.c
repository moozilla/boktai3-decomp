#include "global.h"
#include "engine_globals.h"
#include "ram.h"
struct Config {
    u16 v[20];
};
struct Vec {
    s16 x, y, z, pad;
};
extern s16 gUnk_086149C4[];
void AnimatedParticles_Create(int, void *, struct Config *);
static inline int scale(int a, int b)
{
    int v = a * 40;
    v *= b;
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void AnimatedParticles_EmitOffset(struct Vec *pos, int unused, int type, int flags, int speed,
                                  int horizontal, int start, int decay)
{
    struct Config s;
    struct Vec out;
    s.v[0] = type;
    s.v[1] = flags;
    s.v[3] = speed;
    s.v[4] = 0;
    s.v[5] = 0;
    s.v[6] = horizontal;
    s.v[7] = 0;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        u32 offset = gRandomIndex * 2;
        u8 *sample = (u8 *)(offset + RANDOM_TABLE_ADDR);
        s.v[10] = *sample;
    }
    s.v[11] = 0;
    s.v[9] = 0;
    s.v[8] = 0;
    s.v[12] = start;
    s.v[13] = 0;
    s.v[14] = decay;
    s.v[15] = 0;
    s.v[16] = start;
    s.v[17] = 0;
    s.v[18] = decay;
    s.v[19] = 0;
    out.x = pos->x + scale(gUnk_086149C4[(s.v[10] + 192) & 255], horizontal);
    out.y = pos->y;
    out.z = pos->z + scale(gUnk_086149C4[(s.v[10] + 128) & 255], horizontal);
    AnimatedParticles_Create(1, &out, &s);
}
