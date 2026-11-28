#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

void ActorSoundPhase_StartTimedAnimation(void *, s32);
void Vanargand_UpdateOffsetMotion(void *, s32);
void Vanargand_UpdatePaletteLeap(void *, s32);
void DamageNumberDisplay_ShowWithDefaultScale(void *, s32, s32, void *, s32);
static inline void SetState(struct VanargandPairAttack *p, void (*callback)(void *, s32), s32 id)
{
    p->state = id;
    p->stateCallback = callback;
    p->callbackFrame = 0;
    p->pending = 1;
    p->phase = 0;
    p->timer = 0;
}
s32 Vanargand_OnDamage(struct VanargandPairAttack *p, s32 kind, s32 damage)
{
    switch (kind) {
        case 0:
            if (damage > 0)
                DamageNumberDisplay_ShowWithDefaultScale(&p->damageSource, damage, 0,
                                                         &p->animation.position, 1);
            Vanargand_StartAction(p, 1, 0, 4);
            SetState(p, ActorSoundPhase_StartTimedAnimation, 24);
            break;
        case 2:
            Vanargand_StartAction(p, 1, 0, 5);
            SetState(p, Vanargand_UpdateOffsetMotion, 25);
            break;
        case 1:
            p->health = 0;
            Vanargand_StartAction(p, 1, 0, 6);
            SetState(p, Vanargand_UpdatePaletteLeap, 20);
            break;
    }
    return 1;
}
