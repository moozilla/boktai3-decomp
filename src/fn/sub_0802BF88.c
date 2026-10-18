#include "global.h"

struct S {
    u8 f[0x818];
    u8 a;
};
void Collider_Remove(u8 *);
void GridAttackActor_RemoveSprite(struct S *);
void SceneActors_Destroy(struct S *);
u32 GridAttackActor_Destroy(struct S *p)
{
    Collider_Remove(&p->a);
    GridAttackActor_RemoveSprite(p);
    SceneActors_Destroy(p);
    return 0;
}
