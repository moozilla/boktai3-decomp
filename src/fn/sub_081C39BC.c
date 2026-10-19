#include "global.h"

struct S {
    u8 pad[0x34];
    u32 f34;
    u8 pad0[0xd8 - 0x38];
    u32 d8;
    u8 pad1[0xf4 - 0xdc];
    void (*f4)(void);
    u32 f8;
};
void SpriteController_AnimateGrowing(void);

void RegistryF3E4_StartAppearing(struct S *p)
{
    void (*f)(void);
    p->f34 &= ~1;
    f = SpriteController_AnimateGrowing;
    p->f4 = f;
    p->f8 = 0;
    p->d8 = 0;
}
