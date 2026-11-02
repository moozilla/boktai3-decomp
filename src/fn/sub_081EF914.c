// MATCHING_CONCESSION: register-pinning
#include "scene_departure_display.h"
#include "slot7_transition.h"

static inline u32 StepPlayback(struct DepartureDrawable *owner, struct DisplaySequenceState *state)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, state);
}

static inline u32 StepPlaybackOne(struct DepartureDrawable *owner,
                                  struct DisplaySequenceState *state, u32 maskBit)
{
    return PlayerCue_StepSequenceEndedWithFirstFlipMask(&owner->sprite, state, maskBit);
}

u32 Anim_RestartSequence(struct DisplaySequenceState *, void *, u16, u8, u8);
struct ScreenTween;
void SceneMover_InterpolateScreenX(struct ScreenTween *, s32);

static inline void SetCallback(void (*v)(void), void (**a)(void))
{
    *a = v;
}
static inline void ReverseEndingPose(struct DepartureDrawable *drawable)
{
    drawable->sequence.flags |= 4;
}
void SingletonSlot7_UpdateEndingScene(struct DepartureActor *input)
{
    struct DepartureActor *screen = input;
    struct Slot7Transition *motion = (struct Slot7Transition *)input;
    u8 flag = Slot7_TakeActionEntry(motion);
    if (flag == 1) {
        struct DepartureDrawable *foreground = &screen->foreground;
        struct DisplaySequenceState *state;
        screen->displacementTimer = 0;
        screen->displacement = 0;
        screen->targetX = 0x28;
        screen->targetMode = 0x3C;
        foreground->sequenceId = 2;
        state = &screen->foreground.sequence;
        Anim_SetSequence(state, screen->sequenceResource, 2, 0, 0);
        StepPlaybackOne(foreground, state, flag);
        Slot7_TransitionTo(motion, (void (*)(void))SceneMover_InterpolateScreenX);
        screen->endingPhase = 0;
    }
    switch (screen->endingPhase) {
        case 0:
            if (motion->actionFlags[0]) {
                struct DepartureDrawable *foreground;
                struct DisplaySequenceState *state;
                u32 *field = &screen->foreground.mode;
                register u32 one asm("r5") = 1;
                *field = one;
                screen->foreground.sequenceId = 3;
                state = &screen->foreground.sequence;
                Anim_SetSequence(state, screen->sequenceResource, 3, 0, 0);
                foreground = &screen->foreground;
                StepPlaybackOne(foreground, state, one);
                screen->endingPhase = 1;
                screen->endingTimer = 0;
            }
            break;
        case 1:
            if ((s16)screen->endingTimer > 0x78) {
                u32 *field = &screen->foreground.mode;
                u32 zero = 0;
                *field = zero;
                screen->targetX = 0xA0;
                {
                    u16 *timer = &screen->targetMode;
                    register u8 byteZero asm("r3") = 0;

                    u16 duration = 0x1E;
                    *timer = duration;

                    SetCallback((void (*)(void))SceneMover_InterpolateScreenX, &motion->callback);
                    motion->callbackTimer = zero;
                    motion->callbackEntered = 1;
                    {
                        u8 *phase = &motion->phase;
                        *phase = byteZero;
                    }
                    motion->phaseTimer = zero;
                }
                screen->endingPhase = 2;
            }
            screen->endingTimer++;
            break;
        case 2:
            if (motion->actionFlags[0]) {
                screen->endingPhase = 3;
                screen->endingTimer = 0;
            }
            break;
        case 3:
            if ((s16)screen->endingTimer > 0x78) {
                u32 *field = &screen->foreground.mode;
                u32 zero = 0;
                struct DisplaySequenceState *state;
                *field = zero;
                screen->foreground.sequenceId = 3;
                ReverseEndingPose(&screen->foreground);
                state = &screen->foreground.sequence;
                Anim_RestartSequence(state, screen->sequenceResource,
                                     (u16)screen->foreground.sequenceId, 0, zero);
                screen->targetX = 0x3C;
                {
                    u16 *timer = &screen->targetMode;
                    register u8 byteZero asm("r2") = 0;

                    u16 duration = 0xB4;
                    *timer = duration;

                    SetCallback((void (*)(void))SceneMover_InterpolateScreenX, &motion->callback);
                    motion->callbackTimer = zero;
                    motion->callbackEntered = 1;
                    {
                        u8 *phase = &motion->phase;
                        *phase = byteZero;
                    }
                    motion->phaseTimer = zero;
                }
                screen->endingPhase = 4;
            }
            screen->endingTimer++;
            break;
        case 4:
            if (motion->actionFlags[0]) {
                struct DepartureDrawable *foreground;
                struct DisplaySequenceState *state;
                u32 *field = &screen->endingFinished;
                register u32 one asm("r5") = 1;
                *field = one;
                screen->foreground.sequenceId = 0;
                state = &screen->foreground.sequence;
                Anim_SetSequence(state, screen->sequenceResource, 0, 0, 0);
                foreground = &screen->foreground;
                StepPlaybackOne(foreground, state, one);
                Trigger_ConsumePending(&((struct DepartureActor *)motion)->messages, 1);
            }
            break;
    }
    motion->actionTimer++;
}
