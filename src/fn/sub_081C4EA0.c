#include "global.h"

struct S {
    u8 pad[0x34];
    u16 f34;
    s16 f36;
};
void SceneTransition_StartFadeOut(struct S *);

void RegistryC281_UpdateClosingDelay(struct S *p)
{
    p->f34++;
    if (p->f34 > p->f36)
        SceneTransition_StartFadeOut(p);
}
