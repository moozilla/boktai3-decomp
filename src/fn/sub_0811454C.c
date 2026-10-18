#include "global.h"
void SpriteEffect_RemoveWithBurst(void);
void NativeEffect_SetState(void *, void (*)(void));
void NativeEffect_OnColliderImpact(void *p)
{
    NativeEffect_SetState(p, SpriteEffect_RemoveWithBurst);
}
