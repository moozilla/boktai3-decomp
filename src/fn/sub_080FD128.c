#include "global.h"

void Axe_UpdateScreenShake(void *);
void Axe_TickAuxiliarySpriteTimers(void *);
void sub_080FB744(void *);
void Axe_UpdateBodyColliders(void *);
void Axe_TickCooldowns(void *);
void sub_080FCDEC(void *);

s32 Axe_UpdateEffects(void *s)
{
    Axe_UpdateScreenShake(s);
    Axe_TickAuxiliarySpriteTimers(s);
    sub_080FB744(s);
    Axe_UpdateBodyColliders(s);
    Axe_TickCooldowns(s);
    sub_080FCDEC(s);
    return 1;
}
