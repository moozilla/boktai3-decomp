#include "global.h"
#include "engine/ram.h"
#include "objects/hazards/timed_hazard.h"

/* Tentative: per-frame update of a collider-carrying object (restart animation once, move colliders, advance timer). */
void ColliderObject_Update(struct TimedHazard *hazard)
{
    if (hazard->entered) {
        hazard->entered = 0;
        hazard->sprite.flags &= ~1;
        Anim_SetSequence(&hazard->anim, ((struct TimedHazardPool *)gColliderSpritePool)->animation,
                         1, 0, 0);
    }
    if (hazard->health <= 0 || hazard->animationDone) {
        hazard->remove = 1;
    } else {
        if (hazard->collider)
            Collider_SetPosition(hazard->collider, &hazard->position, 0);
        if (hazard->attack) {
            Collider_SetPosition(hazard->attack, &hazard->position, 0);
            Collider_Refresh(hazard->attack);
        }
        hazard->timer++;
    }
}
