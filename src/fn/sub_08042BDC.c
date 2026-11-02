#include "contact_actor.h"
#include "display_sprite_playback.h"
void ContactActor_UpdateDisplay(struct ContactActor *owner)
{
    struct ContactSwarmSprite *sprite = (struct ContactSwarmSprite *)&owner->sprite;
    s32 diff;
    s32 wave, offset;
    owner->sprite.position = owner->position;
    owner->finished = PlayerCue_StepSequenceEnded(sprite, &owner->sequence);
    diff = owner->position.y - owner->targetHeight;
    if (diff <= -4)
        owner->position.y += 4;
    else if (diff > 3)
        owner->position.y -= 4;
    else
        owner->position.y = owner->targetHeight;
    wave = gUnk_086149C4[owner->bobPhase] * 32;
    if (wave >= 0)
        offset = wave >> 12;
    else
        offset = -((-wave) >> 12);
    sprite->position.y += offset;
    owner->bobPhase += 4;
}
