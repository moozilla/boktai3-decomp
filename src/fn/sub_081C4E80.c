#include "global.h"

struct S {
    u8 pad[0x18];
    u32 f18;
};
u32 RegistryF3E4_GetStateResult(u32);
void RegistryF3E4_ClearState(u32);
void SceneTransition_StartCompletionDelay(struct S *);

void RegistryC281_UpdateFinalMovement(struct S *p)
{
    if (RegistryF3E4_GetStateResult(p->f18)) {
        RegistryF3E4_ClearState(p->f18);
        SceneTransition_StartCompletionDelay(p);
    }
}
