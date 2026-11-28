#include "effects/bursts/fragment_effect.h"
s32 Mod(s32, s32);
s32 Div(s32, s32);
#include "player/cutscene/player_cue_sequence.h"

void FragmentEffect_SpawnRandomParticle(struct ContactPosition *);

// confidence=low: F422 fragment state interpolates sprite scale on appearance
void FragmentEffect_GrowToFullSize(struct FragmentEffectPool *pool, struct Fragment *fragment)
{
    if (fragment->entered) {
        fragment->sprite.flags = (fragment->sprite.flags & ~1) | 2;
        fragment->scaleTimer = 30;
        fragment->timer = 0;
        fragment->entered = 0;
        fragment->sprite.scaleX = 1;
        fragment->sprite.scaleY = 1;
    }
    if (!Mod(fragment->timer, 12))
        FragmentEffect_SpawnRandomParticle(&fragment->sprite.position);
    if (fragment->scaleTimer > 0) {
        fragment->sprite.scaleX += Div(64 - fragment->sprite.scaleX, fragment->scaleTimer);
        fragment->sprite.scaleY += Div(64 - fragment->sprite.scaleY, fragment->scaleTimer);
        fragment->scaleTimer--;
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&fragment->sprite, &fragment->sequence);
    if (fragment->scaleTimer <= 0) {
        fragment->state = 2;
        fragment->sprite.flags &= ~2;
        fragment->entered = 1;
    } else {
        fragment->timer++;
    }
}
