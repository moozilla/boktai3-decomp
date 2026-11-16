#include "global.h"
#include "axe.h"
#include "rotation_phase.h"
#include "player.h"
// confidence=low: saves the player position and turns the axe before the attack callback.
void Sound_PlaySong(u32);
void Axe_UpdateTargetLunge(void);
void Axe_UpdateTargetWindup(struct ChaseActor *actor, s32 elapsed)
{
    struct Axe *state = actor->base.context;
    struct RotatingDrawable *drawable = actor->base.drawable;
    s32 angle, magnitude;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        drawable->flags |= 2;
        state->savedAngle = actor->base.tracking.heading;
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
        if (actor->base.health <= state->healthThreshold)
            delay = state->delay >> 1;
        else
            delay = state->delay;
        if (elapsed > delay) {
            void *callback = Axe_UpdateTargetLunge;
            u32 phase = 0x36;
            actor->base.field2B2 = 1;
            actor->base.stepDone = 0;
            actor->base.controlKind = phase;
            actor->base.target = callback;
            actor->base.field2C8 = 0;
            {
                u32 *flags = &actor->base.pendingControlFlags;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->base.pendingControl = 0;
        }
    } else {
        s32 step, turn;
        if (actor->base.health <= state->healthThreshold)
            step = state->turnStep;
        else
            step = state->turnStep >> 1;
        if (step <= 0)
            step = 1;
        turn = NormalizeAngle(32 - state->savedAngle);
        if (turn < 0)
            drawable->angle -= step;
        else
            drawable->angle += step;
    }
}
