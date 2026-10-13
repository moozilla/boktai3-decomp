#include "global.h"
int sub_08012058(void *, void *);
void sub_0801204C(void *, void *, int);
void AnimatedParticles_EmitOffset(void *, int, int, int, int, int, int, int);
void AnimatedHazard_EmitWarning(void *m, u8 *p)
{
    sub_08012058(m, p);
    if (*(u32 *)(p + 8) < 60) {
        switch (*(u32 *)(p + 8)) {
            case 0:
            case 24:
            case 48:
                AnimatedParticles_EmitOffset(p + 128, 1, 3, 0x3000, 64, 8, 256, 256);
                break;
        }
    }
    if (++*(u32 *)(p + 8) > 89)
        sub_0801204C(m, p, 3);
}
