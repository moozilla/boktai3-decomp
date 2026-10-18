#define PursuingActor_RemoveTrailSprites SwooshProjectile_RemoveTrailSprites
#include "global.h"
#include "engine.h"
#include "pursuing_actor.h"
void TimedEffect_UpdateVisibility(struct PursuingActor *);
void SwooshProjectile_RemoveTrailSprites(void *);
u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);
void Sprite_Remove(void *);
static inline void Disable(struct PursuingActorCollider *c)
{
    c->flags |= 4;
}
void PursuingActor_UpdateRetirement(struct PursuingActorManager *manager,
                                    struct PursuingActor *actor)
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
    TimedEffect_UpdateVisibility(actor);
    actor->phase += 8;
    if (Sprite_BlinkUntilExpired(actor, actor->timer, 10, 30)) {
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
