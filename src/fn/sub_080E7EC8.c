#define SceneActor_InitializeReactionColliders sub_080E7EC8
#include "global.h"
#include "contact_actor.h"
void Mimic_OnAttackContact(void);
void Mimic_OnBodyContact(void);
void SceneActorCollision_InitAttack(void *, void *, void *, void (*)(void), u32, void *);
void SceneActorCollision_InitBody(void *, void *, void *, void (*)(void), void *);
void sub_080E7EC8(void *p)
{
    union {
        struct ContactPackedVector packed;
        struct ContactPosition position;
    } bounds, offset;
    bounds.packed.x = 50;
    bounds.packed.y = 30;
    bounds.packed.z = 50;
    offset.packed.x = 0;
    offset.packed.y = 40;
    offset.packed.z = 0;
    SceneActorCollision_InitAttack(p, &bounds.position, &offset.position, Mimic_OnAttackContact, 0, p);
    bounds.position.x = 80;
    bounds.position.y = 120;
    bounds.position.z = 80;
    offset.position.x = 0;
    offset.position.y = 120;
    offset.position.z = 0;
    SceneActorCollision_InitBody(p, &bounds.position, &offset.position, Mimic_OnBodyContact, p);
}
