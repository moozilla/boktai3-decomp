#include "global.h"

void NativeEffect_SetState(void *, void (*)(void));
void NativeEffect_DestroyEntry(void);

void NativeEffect_OnColliderExpire(void *p)
{
    NativeEffect_SetState(p, NativeEffect_DestroyEntry);
}
