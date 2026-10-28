#include "global.h"
#include "rotation_phase.h"
#include "player.h"
// confidence=low: saves the player position and turns the axe before the attack callback.
struct WindupContext {
    u8 unknown0[0xE3E];
    u8 delay, turnStep;
    u8 unknownE40[8];
    s32 healthThreshold;
    u8 unknownE4C[0x84];
    u8 active, unknownED1[2], direction;
    u8 unknownED4[0x48];
    struct ContactPosition target;
};
void Sound_PlaySong(u32);
void Axe_UpdateTargetLunge(void);
void Axe_UpdateTargetWindup(struct SceneActorRegistryResetView *actor, s32 elapsed)
{
    struct WindupContext *state = actor->context;
    struct RotatingDrawable *drawable = actor->drawable;
    s32 angle, magnitude;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        drawable->flags |= 2;
        state->direction = actor->fieldE9;
        state->target = *(struct ContactPosition *)((*(u8 **)gPlayers) + 0x30);
        state->active = 1;
        Sound_PlaySong(0x16F);
    }
    angle = NormalizeAngle(drawable->angle);
    magnitude = angle;
    if (magnitude < 0)
        magnitude = -magnitude;
    if (magnitude > 48) {
        s32 delay;
        if (actor->health <= state->healthThreshold)
            delay = state->delay >> 1;
        else
            delay = state->delay;
        if (elapsed > delay) {
            void *callback = Axe_UpdateTargetLunge;
            u32 phase = 0x36;
            actor->field2B2 = 1;
            actor->field2B0 = 0;
            actor->field2B5 = phase;
            actor->target = callback;
            actor->field2C8 = 0;
            {
                u32 *flags = &actor->flags114;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->word104 = 0;
        }
    } else {
        s32 step, turn;
        if (actor->health <= state->healthThreshold)
            step = state->turnStep;
        else
            step = state->turnStep >> 1;
        if (step <= 0)
            step = 1;
        turn = NormalizeAngle(32 - state->direction);
        if (turn < 0)
            drawable->angle -= step;
        else
            drawable->angle += step;
    }
}
