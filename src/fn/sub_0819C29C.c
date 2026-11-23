#include "hresvelgr_travel.h"

void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void ActorReaction_BeginCollisionPhase(void *);
void sub_081A1D18(void *);
void sub_081A14FC(void *);

int Hresvelgr_BeginDamageReaction(struct HresvelgrTravelActor *p, int kind, int damage)
{
    struct SceneVec size;
    switch (kind) {
        case 0:
            if (damage > 0) {
                size.x = 32;
                size.y = 0;
                size.z = 32;
                DamageNumberDisplay_Show(p->bodyCollider, damage, 0, &p->position, &size, 1);
            }
            p->savedPhase = p->actionPhase;
            Hresvelgr_StartAction(p, 1, 0, 3);
            Hresvelgr_StartReaction(p, ActorReaction_BeginCollisionPhase, 0x24);
            break;
        case 2:
            Hresvelgr_StartAction(p, 1, 0, 4);
            Hresvelgr_StartReaction(p, sub_081A1D18, 0x25);
            break;
        case 1:
            Hresvelgr_StartAction(p, 1, 0, 5);
            Hresvelgr_StartReaction(p, sub_081A14FC, 0x26);
            break;
    }
    return 1;
}
