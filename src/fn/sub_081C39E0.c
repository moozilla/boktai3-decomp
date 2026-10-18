#include "global.h"

struct S {
    u8 pad0[0xd8];
    u32 d8;
    u8 pad1[0xf4 - 0xdc];
    void (*f4)(void);
    u32 f8;
};
void SpriteController_AnimateShrinking(void);

void RegistryF3E4_StartDisappearing(struct S *p)
{
    void (*f)(void) = SpriteController_AnimateShrinking;
    p->f4 = f;
    p->f8 = 0;
    p->d8 = 0;
}
