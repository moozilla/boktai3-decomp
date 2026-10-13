#include "global.h"
extern u32 gUnk_02000060;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 ParticleManager_Initialize(void *, void *);
void sub_0821A0C0(void *);
void ParticleManager_Update(void);
void sub_0801450C(void);

void *sub_080145E4(void *a)
{
    void *p;
    if (gUnk_02000060 != 0)
        return (void *)gUnk_02000060;
    p = sub_08219FBC(10, 0xabc);
    if (p != 0) {
        sub_0821A04C(p, ParticleManager_Update, sub_0801450C);
        if (ParticleManager_Initialize(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
