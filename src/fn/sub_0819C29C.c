#include "grid_attack_actor.h"
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void ActorReaction_BeginCollisionPhase(void *);
void sub_081A1D18(void *);
void sub_081A14FC(void *);
static inline void SetState(struct GridAttackActor *p, void (*fn)(void *), int id)
{
    int zero;
    p->state = id;
    zero = 0;
    p->callback = fn;
    p->timer = zero;
    p->changed = 1;
    p->stateReset = zero;
    p->pending = zero;
}
int Hresvelgr_BeginDamageReaction(struct GridAttackActor *p, int kind, int damage)
{
    struct GridAttackTransform *position = &p->transform;
    struct GridAttackPacked size;
    switch (kind) {
        case 0:
            if (damage > 0) {
                size.x = 32;
                size.y = 0;
                size.z = 32;
                DamageNumberDisplay_Show((u8 *)p + 0x248, damage, 0, &position->coordinates, &size,
                                         1);
            }
            {
                u8 saved = p->action;
                *((u8 *)p + 0x388) = saved;
            }
            {
                int one = 1;
                int zero = 0;
                int id = 3;
                p->active = one;
                p->actionReset = zero;
                p->action = id;
                p->actionTimer = zero;
                p->entered = one;
            }
            SetState(p, ActorReaction_BeginCollisionPhase, 0x24);
            break;
        case 2: {
            int one = 1;
            int zero = 0;
            int id = 4;
            p->active = one;
            p->actionReset = zero;
            p->action = id;
            p->actionTimer = zero;
            p->entered = one;
            SetState(p, sub_081A1D18, 0x25);
        } break;
        case 1: {
            int one = 1;
            int zero = 0;
            int id = 5;
            p->active = one;
            p->actionReset = zero;
            p->action = id;
            p->actionTimer = zero;
            p->entered = one;
        }
            SetState(p, sub_081A14FC, 0x26);
            break;
    }
    return 1;
}
