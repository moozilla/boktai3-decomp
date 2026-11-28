#include "effects/bursts/fragment_burst.h"
#include "player/cutscene/player_cue_sequence.h"

void Sound_PlaySong(u32);

// confidence=low: animates three projected burst sprites and enables their short-lived impact collider.
void FragmentBurst_UpdateImpactAnimation(void *unused, struct FragmentBurst *burst)
{
    s32 i;

    if (burst->enter) {
        Sound_PlaySong(0x3B1);
        burst->enter = 0;
    }
    for (i = 0; i < 3; i++) {
        struct DisplaySequenceSprite *sprite = &burst->sprites[i];

        sprite->flags &= ~1;
        PlayerCue_StepSequence((struct ContactSwarmSprite *)sprite, &burst->sequences[i]);
        FragmentBurst_ProjectToScreen(&sprite->position, &burst->origin);
        sprite->position.y += i * 32;
    }
    if (burst->timer > 4) {
        burst->state = 2;
        burst->enter = 1;
    } else {
        Collider_SetPosition(&burst->collider, &burst->position, 0);
        Collider_Refresh(&burst->collider);
        burst->timer++;
    }
}
