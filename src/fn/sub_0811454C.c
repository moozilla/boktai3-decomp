#include "global.h"
void sub_081136B0(void);
void NativeEffect_SetState(void *, void (*)(void));
void NativeEffect_OnColliderImpact(void *p)
{
    NativeEffect_SetState(p, sub_081136B0);
}
