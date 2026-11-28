#include "effects/bursts/linked_effect.h"
#include "effects/bursts/spiral_burst.h"
#include "engine/math/monster_particle_random.h"
#include "player/cutscene/player_cue_sequence.h"

s32 Mod(s32, s32);
s32 Div(s32, s32);
void LinkedEffect_SetVariant(void *, u32);

// confidence=low: expands the linked effect while preparing an elevated travel target.
void LinkedEffect_BeginRisingTravel(struct LinkedEffectPool *pool, struct LinkedEffect *effect)
{
    if (effect->entered) {
        u32 heading;
        u32 index;
        s32 distance;

        effect->sprite.flags = (effect->sprite.flags & ~1) | 2;
        effect->animationEnded = 0;
        heading = effect->heading;
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        distance = *(const u16 *)(index * 2 + RANDOM_TABLE_ADDR) & 255;
        LinkedEffect_SetPlanarVector(&effect->velocity, heading, distance + 64);
        LinkedEffect_AddPosition(&effect->velocity, &effect->sprite.position);
        effect->velocity.y = effect->sprite.position.y + 512;
        effect->riseTimer = 15;
        effect->frame = 0;
        effect->entered = 0;
        effect->sprite.scaleX = 1;
        effect->sprite.scaleY = 1;
    }
    if (!Mod(effect->frame, 8))
        LinkedEffect_SetVariant(&effect->sprite.position, (u8)(effect->heading + 128));
    if (effect->riseTimer) {
        effect->sprite.scaleX += Div(64 - effect->sprite.scaleX, effect->riseTimer);
        effect->sprite.scaleY += Div(64 - effect->sprite.scaleY, effect->riseTimer);
        effect->riseTimer--;
    }
    effect->animationEnded = PlayerCue_StepSequenceEnded(
        (struct ContactSwarmSprite *)&effect->sprite, &effect->animation);
    if (effect->animationEnded && !effect->riseTimer) {
        effect->state = 2;
        effect->sprite.flags &= ~2;
        effect->entered = 1;
    } else
        effect->frame++;
}
