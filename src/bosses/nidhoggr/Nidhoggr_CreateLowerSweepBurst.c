#include "global.h"
struct T {
    u16 a, b, c, d;
};
struct V {
    u16 a, b, c;
};
extern const struct T gUnk_0824F1A4[];
void SpriteParticle_SpawnBatch(struct V *, s32, s32, s32, s32);
void Sound_PlaySongChecked(s32);
void Nidhoggr_CreateLowerSweepBurst(struct V *p)
{
    s32 i = 0;
    struct V v;
    struct V *q = &v;
    do {
        const struct T *t = &gUnk_0824F1A4[i];
        q->a = p->a + t->a;
        q->b = p->b + t->b;
        q->c = p->c + t->c;
        SpriteParticle_SpawnBatch(&v, 1, 2, 1, 0x154);
        i++;
    } while (i <= 6);
}
