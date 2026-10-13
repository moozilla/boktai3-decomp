#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 AnimatedParticleManager_Initialize(void *, void *);
void sub_0821A0C0(void *);
void AnimatedParticleManager_Update(void);
void sub_0801537C(void);

void *sub_0801548C(void *a)
{
    void *p = sub_08219FBC(9, 0xf24);
    if (p != 0) {
        sub_0821A04C(p, AnimatedParticleManager_Update, sub_0801537C);
        if (AnimatedParticleManager_Initialize(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
