#include "global.h"

struct S {
    u8 pad[0x34];
    u16 f34;
};
void SceneTransition_Finish(struct S *);

void RegistryC281_UpdateClosingFade(struct S *p)
{
    p->f34++;
    if (p->f34 > 0x40)
        SceneTransition_Finish(p);
}
