#include "engine/shared/display_sequence_sprite.h"
#include "effects/scripted_particle/scripted_particle.h"
#include "engine/shared/engine_globals.h"
/* Drifting sprite: holds hidden for a delay, then drifts along its angle (sine table), rises, and steps its display sequence; result=2 when it ends. */
void Anim_SetDurationScale(struct DisplaySequenceState *, u32);
void DriftingSprite_Update(struct ScriptedParticle *s)
{
    struct DisplaySequenceSprite *sprite = &s->sprite;
    struct DisplaySequenceState *sequence;
    if (s->hiddenFrames) {
        s->sprite.flags |= 1;
        s->hiddenFrames--;
        return;
    }
    s->sprite.flags &= ~1;
    sequence = &s->sequence;
    Anim_SetDurationScale(sequence, s->durationScale);
    s->sprite.position.x += (s->speed * gUnk_086149C4[(s->angle + 64) & 255]) >> 12;
    s->sprite.position.z += (s->speed * gUnk_086149C4[s->angle]) >> 12;
    if (s->riseDelay)
        s->riseDelay--;
    else {
        s->sprite.position.y += s->rise;
        if (s->riseSteps)
            s->riseSteps--;
        else
            s->sprite.priority = 2;
    }
    if (DisplaySequence_StepSprite(sprite, &s->sequence))
        s->state = 2;
}
