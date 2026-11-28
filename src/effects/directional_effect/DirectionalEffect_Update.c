#include "player/player.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/video/display_sprite_playback.h"

void DirectionalEffect_Update(struct DirectionalSpriteEffect *effect)
{
    if (effect->active) {
        if (PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)&effect->sprite,
                                        &effect->animation)) {
            effect->active = 0;
            effect->sprite.flags |= 1;
        } else {
            effect->sprite.position.axis.x += effect->velocityX;
            effect->sprite.position.axis.depth += effect->velocityY;
        }
    }
}
