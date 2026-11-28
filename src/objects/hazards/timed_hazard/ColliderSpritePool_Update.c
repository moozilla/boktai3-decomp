#include "global.h"
#include "engine/ram.h"
#include "objects/hazards/timed_hazard.h"
#include "player/cutscene/player_cue_sequence.h"

extern void *const gUnk_08E879CC[];
s32 Callback_InvokeActor(void *, void *);

s32 ColliderSpritePool_Update(struct TimedHazardPool *pool)
{
    struct TimedHazard *p = pool->head, *next;
    for (; p; p = next) {
        next = p->next;
        if (p->remove) {
            ColliderSprite_Destroy(p);
            continue;
        }
        p->animationDone =
            PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)&p->sprite, &p->anim);
        p->sprite.position = p->position;
        Callback_InvokeActor(p, gUnk_08E879CC[p->state]);
    }
    return 0;
}
