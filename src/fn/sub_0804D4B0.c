#include "tether_actor.h"
extern struct TetherActor *gUnk_02000488;
void SceneActorContacts_RemoveNode(void *);
void sub_08225938(void *);
void DisplayAnimation_Destroy(void *);

void TetherActor_RemoveSegments(struct TetherActor *);
void TetherActor_RemoveAuxiliarySprite(struct TetherActor *);
struct TetherActor *TetherActor_Destroy(struct TetherActor *p)
{
    if (p->auxiliaryRegistered != 0)
        SceneActorContacts_RemoveNode(p->auxiliaryRegistration);
    sub_08225938(&p->bodyConfig);
    DisplayAnimation_Destroy(&p->display.animation);
    Collider_Remove(&p->chargeCollider);
    TetherActor_RemoveSegments(p);
    TetherActor_RemoveAuxiliarySprite(p);
    return gUnk_02000488 = 0;
}
