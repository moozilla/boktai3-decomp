#include "sword_wave_component.h"

extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
void IvyChild_UpdateAfterBob(struct SwordWaveComponent *);
void Sword_UpdateDepthWaveReturn(struct SwordWaveComponent *p)
{
    s32 wave, position;
    const s16 *table;
    p->rotation += p->rotationStep;
    table = gUnk_086149C4;
    position = p->timer;
    wave = (table[(256 - position) & 255] >> 3) * p->multiplier;
    p->x = p->startX + Div(p->dx * position, 128);
    p->y = p->startY + Div(p->dy * p->timer, 128);
    p->z = p->startZ + Div(p->dz * p->timer, 128) + wave;
    p->timer--;
    if (p->timer == 0) {
        u32 one;
        do {
            p->rotation = 128;
            one = 1;
            p->active = one;
        } while (0);
        p->phase = one;
        one |= p->flags;
        p->flags = one;
        p->update = IvyChild_UpdateAfterBob;
    }
}
