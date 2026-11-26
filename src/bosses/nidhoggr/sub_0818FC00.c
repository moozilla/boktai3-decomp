#include "global.h"
#include "engine/video/display_animation.h"
#include "player/cutscene/player_cue_sequence.h"
#include "bosses/nidhoggr/nidhoggr_effect_pool.h"
static inline u32 Nidhoggr_StepAnimation(struct ScenePanelOwnedEffect *owner)
{
    return PlayerCue_StepSequenceEnded((void *)&owner->sprite, &owner->sequence);
}

void Nidhoggr_UpdateDetachedAttackEffect(struct ScenePanelOwnedEffect *p)
{
    u32 ended = Nidhoggr_StepAnimation(p);
    switch (p->state) {
        case 0:
            if (ended) {
                struct ContactCollider *collider;
                u32 mask;
                p->anchor = NULL;
                p->motion.x = 12;
                p->motion.y = 0;
                p->motion.z = 0;
                p->referenceCoordinate = p->sprite.position.x;
                p->state++;
                collider = &p->attackCollider;
                mask = ~4;
                collider->flags &= mask;
            }
            break;
        case 1:
            if (p->sprite.position.x - p->referenceCoordinate > 0x1000) {
                struct ContactCollider *collider;
                p->update = 0;
                if (p->onComplete) {
                    Callback_InvokeActor(p, p->onComplete);
                    p->onComplete = 0;
                }
                collider = &p->attackCollider;
                collider->flags |= 4;
            }
            break;
    }
}
