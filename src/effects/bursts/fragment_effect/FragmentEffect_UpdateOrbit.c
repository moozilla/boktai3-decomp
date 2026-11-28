#include "effects/bursts/fragment_effect.h"
s32 Mod(s32, s32);
s32 Div(s32, s32);
#include "player/cutscene/player_cue_sequence.h"

void FragmentEffect_SpawnRandomParticle(struct ContactPosition *);
void FragmentEffect_UpdateCollider(struct Fragment *);

// confidence=low: circular fragment motion around the saved center, using F422 graphics
void FragmentEffect_UpdateOrbit(struct FragmentEffectPool *pool, struct Fragment *fragment)
{
    u32 heading;

    if (fragment->entered) {
        fragment->timer = 0;
        fragment->entered = 0;
    }
    if (fragment->direction > 0) {
        fragment->angle += fragment->angularSpeed;
        if (fragment->angle > 0xFF000)
            fragment->angle -= 0xFF000;
    } else {
        fragment->angle -= fragment->angularSpeed;
    }
    heading = fragment->angle;
    FragmentEffect_ComputeOffset(&fragment->offset, heading, fragment->radius);
    fragment->offset.y = 0;
    FragmentEffect_AddPositions(&fragment->sprite.position, &fragment->center, &fragment->offset);
    if (!Mod(fragment->timer, 30)) {
        s32 sample;
        u32 index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        sample = *(const u8 *)(RANDOM_TABLE_ADDR + index * 2);
        if (sample <= 127)
            FragmentEffect_SpawnRandomParticle(&fragment->sprite.position);
    }
    FragmentEffect_UpdateCollider(fragment);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&fragment->sprite, &fragment->sequence);
    fragment->timer++;
    if (fragment->lifetime > 0)
        fragment->lifetime--;
    else {
        fragment->state = 3;
        fragment->entered = 1;
    }
}
