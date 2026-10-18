#include "global.h"

struct S {
    u8 pad[0x30];
    void (*f30)(void);
    s16 f34;
    s16 f36;
};
void SceneTransition_Finish(struct S *);
void RegistryC281_UpdateClosingDelay(void);

void SceneTransition_StartCompletionDelay(struct S *p)
{
    if (p->f36 < 0) {
        SceneTransition_Finish(p);
    } else {
        p->f30 = RegistryC281_UpdateClosingDelay;
        p->f34 = 0;
    }
}
