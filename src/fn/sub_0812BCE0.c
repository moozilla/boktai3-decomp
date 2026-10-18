#define PursuingActor_UpdateRetirement sub_0812BCE0
#define PursuingActor_UpdateSprite     sub_0812B5D0
#define PursuingActor_RemoveTrailSprites SwooshProjectile_RemoveTrailSprites
#include "global.h"
#include "engine.h"
#include "pursuing_actor.h"
void sub_0812B5D0(struct PursuingActor *);
void SwooshProjectile_RemoveTrailSprites(void *);
u8 sub_082212DC(void *, u32, s32, s32);
void Sprite_Remove(void *);
static inline void Disable(struct PursuingActorCollider *c)
{
    c->flags |= 4;
}
void sub_0812BCE0(struct PursuingActorManager *manager, struct PursuingActor *actor)
{
    struct SceneVec *position, *velocity;
    if (actor->entered) {
        Disable(&actor->primary);
        Disable(&actor->secondary);
        actor->timer = 0;
        actor->entered = 0;
    }
    position = &actor->position;
    velocity = &actor->velocity;
    actor->position.x = velocity->x + actor->position.x;
    position->y = velocity->y + position->y;
    position->z = velocity->z + position->z;
    sub_0812B5D0(actor);
    actor->phase += 8;
    if (sub_082212DC(actor, actor->timer, 10, 30)) {
        Collider_Remove(&actor->secondary);
        SwooshProjectile_RemoveTrailSprites(actor);
        Sprite_Remove(actor);
        manager->activeSlots &= ~(1 << actor->slot);
        actor->slot = -1;
        actor->active = 0;
    } else {
        actor->timer++;
    }
}
