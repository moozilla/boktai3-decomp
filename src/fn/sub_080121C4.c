#include "animated_hazard.h"

s32 sub_08012058(void *, struct AnimatedHazard *);
void sub_0801204C(void *, struct AnimatedHazard *, u32);

void AnimatedHazard_EmitWarning(void *m, struct AnimatedHazard *p)
{
    sub_08012058(m, p);
    if (p->timer < 60) {
        switch (p->timer) {
            case 0:
            case 24:
            case 48:
                AnimatedParticles_EmitOffset(&p->sprite.position, 1, 3, 0x3000, 64, 8, 256, 256);
                break;
        }
    }
    if (++p->timer > 89)
        sub_0801204C(m, p, 3);
}
