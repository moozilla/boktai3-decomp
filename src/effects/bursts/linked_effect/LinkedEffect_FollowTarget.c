#include "effects/bursts/linked_effect.h"
#include "engine/math/monster_particle_random.h"
#include "player/cutscene/player_cue_sequence.h"

s32 Mod(s32, s32);
s32 Div(s32, s32);
void LinkedEffect_UpdateColliders(struct LinkedEffect *);

static inline s32 RandomValue(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)((index << 1) + RANDOM_TABLE_ADDR);
}

static inline u16 RandomByte(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return (u8)((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
}

static inline void LinkedEffect_TargetDelta(struct ContactPosition *out,
                                            struct FragmentEffectSprite *sprite,
                                            struct LinkedEffect *effect)
{
    LinkedEffect_Subtract(out, &sprite->position, effect->target);
}

// confidence=low: steers and accelerates a linked effect toward a target until expiry.
void LinkedEffect_FollowTarget(struct LinkedEffectPool *pool, struct LinkedEffect *effect)
{
    struct FragmentEffectSprite *sprite = &effect->sprite;
    struct DisplaySequenceState *sequence = &effect->animation;

    if (effect->entered) {
        effect->sprite.flags &= ~1;
        effect->riseTimer = 60;
        effect->frame = 0;
        effect->entered = 0;
    }
    if (effect->animationEnded) {
        s16 value = RandomByte();

        if (value <= 63)
            Anim_SetSequence(sequence, pool->sequences, 21, 0, 0);
        else if (value <= 77)
            Anim_SetSequence(sequence, pool->sequences, 22, 0, 0);
        else
            Anim_SetSequence(sequence, pool->sequences, 20, 0, 0);
        effect->animationEnded = 0;
    }
    if (!effect->delay) {
        struct ContactPosition delta;
        s32 turn;

        LinkedEffect_TargetDelta(&delta, sprite, effect);
        turn = Math_Atan2(delta.x, delta.z) - effect->heading;
        if (turn > 128)
            turn -= 256;
        else if (turn < -128)
            turn += 256;
        if (effect->turnTimer) {
            s32 magnitude = turn;

            if (magnitude < 0)
                magnitude = -magnitude;
            if (magnitude > 2) {
                if (turn < 0)
                    effect->heading -= (RandomValue() & 1) + 1;
                else
                    effect->heading += (RandomValue() & 1) + 1;
                effect->turnTimer--;
            } else
                effect->turnTimer = 0;
        }
    } else
        effect->delay--;
    LinkedEffect_SetPlanarVector(&effect->velocity, effect->heading, effect->speed);
    if (!Mod(effect->frame, 10) && effect->speed < effect->maxSpeed)
        effect->speed++;
    sprite->position.x += effect->velocity.x;
    if (effect->riseTimer) {
        s32 targetY = effect->target->y;
        s32 height = sprite->position.y - 200;

        sprite->position.y += Div(targetY - height, effect->riseTimer);
        effect->riseTimer--;
    }
    sprite->position.z += effect->velocity.z;
    effect->animationEnded =
        PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)sprite, sequence);
    LinkedEffect_UpdateColliders(effect);
    effect->frame++;
    if (effect->lifetime)
        effect->lifetime--;
    else {
        effect->state = 3;
        effect->entered = 1;
    }
}
