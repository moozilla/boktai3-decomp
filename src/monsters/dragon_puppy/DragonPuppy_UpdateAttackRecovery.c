#include "monsters/dragon_puppy.h"
#include "engine/engine.h"
void DragonPuppy_UpdateAttackTiming(void);
u32 DragonPuppy_UpdateAttackRecovery(struct DragonPuppy *p)
{
    if (p->unknown12C > 0) {
        p->unknown12C--;
    } else {
        Anim_SetSequence(&p->animation, p->deathAnimation, 3, p->deathVariant, 0);
        p->update = DragonPuppy_UpdateAttackTiming;
    }
}
