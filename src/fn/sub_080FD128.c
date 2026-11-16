#include "global.h"

void Axe_UpdateScreenShake(struct S080FC3F8 *s);
void Axe_TickAuxiliarySpriteTimers(struct S080FBEDC *s);
struct SceneActorRegistryActor;
u32 Axe_UpdatePaletteTransition(struct ChaseActor *actor);
void Axe_UpdateBodyColliders(struct AxeCollisionActor *actor);
void Axe_TickCooldowns(void *);
void sub_080FCDEC(void *);

s32 Axe_UpdateEffects(void *s)
{
    Axe_UpdateScreenShake(s);
    Axe_TickAuxiliarySpriteTimers(s);
    Axe_UpdatePaletteTransition(s);
    Axe_UpdateBodyColliders(s);
    Axe_TickCooldowns(s);
    sub_080FCDEC(s);
    return 1;
}
