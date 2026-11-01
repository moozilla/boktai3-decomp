#include "scene_departure_display.h"
#include "ram.h"

extern const u16 gUnk_086141BC[];
s32 SceneMover_StepPastScreenX(void *, s32, s32);
void SceneMover_ApproachScreenX(void *, s32);
s32 Player_GetHorizontalVelocity(s32);
void Player_GetWorldPosition(s32, void *);

static inline u32 RandomValue(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    return gUnk_086141BC[gEffectRandomTableIndex];
}

void SceneActors_UpdateHitSensitiveDeparture(struct DepartureActor *input, s32 time)
{
    struct DepartureActor *actor = input;
    if (input->entered) {
        struct DepartureDrawable *layer;
        struct DisplaySequenceState *state;
        input->entered = 0;
        layer = &input->foreground;
        input->displacementTimer = 0;
        input->displacement = 0;
        input->targetX = 40;
        layer->sequenceId = 2;
        state = &input->foreground.sequence;
        Anim_SetSequence(state, input->sequenceResource, 2, 0, 0);
        PlayerCue_StepSequenceEnded(&layer->sprite, state);
        actor->motionState = 0;
        Sound_PlaySong(0x586);
    }
    switch (actor->motionState) {
    case 0:
        if (time > 90) {
            struct DisplaySequenceState *state;
            actor->foreground.mode = 1;
            actor->foreground.sequenceId = 3;
            state = &actor->foreground.sequence;
            Anim_SetSequence(state, actor->sequenceResource, 3, 0, 0);
            PlayerCue_StepSequenceEnded(&actor->foreground.sprite, &actor->foreground.sequence);
            actor->motionState = 1;
        }
        actor->jitter = RandomValue() & 1;
        SceneMover_ApproachScreenX(input, actor->targetX);
        ActorDisplay_UpdateScreenAnchor(input);
        ActorDisplay_UpdateForegroundSprite(input);
        ActorDisplay_UpdateAccentSprite(input);
        SceneActors_EmitPeriodicParticles(input, time);
        break;
    case 1:
        if (time > 150) {
            actor->foreground.mode = 0;
            actor->motionState = 2;
            actor->targetX = 160;
        }
        actor->jitter = RandomValue() & 3;
        SceneMover_ApproachScreenX(input, actor->targetX);
        ActorDisplay_UpdateScreenAnchor(input);
        ActorDisplay_UpdateForegroundSprite(input);
        ActorDisplay_UpdateAccentSprite(input);
        SceneActors_EmitPeriodicParticles(input, time);
        break;
    case 2: {
        s32 velocity = Player_GetHorizontalVelocity(actor->playerIndex);
        struct ScenePoolVec32 position;
        if (velocity < 0x6000) velocity = 0x6000;
        if (actor->slowdownTimer > 0) velocity >>= 1;
        SceneMover_StepPastScreenX(input, actor->targetX, velocity);
        actor->jitter = RandomValue() & 7;
        ActorDisplay_UpdateScreenAnchor(input);
        ActorDisplay_UpdateForegroundSprite(input);
        ActorDisplay_UpdateAccentSprite(input);
        SceneActors_EmitPeriodicParticles(input, time);
        if (actor->foreground.ended) actor->foreground.mode = 1;
        Player_GetWorldPosition(0, &position);
        if (actor->screen.x > 159 || time > 420) {
            actor->foreground.mode = 0;
            actor->foreground.sequenceId = 3;
            actor->foreground.sequence.flags |= 4;
            Anim_RestartSequence(&actor->foreground.sequence, actor->sequenceResource,
                 (u16)actor->foreground.sequenceId, 0, 0);
            actor->targetX = 60;
            actor->motionState = 4;
        } else if (actor->hit == 1) {
            actor->targetX = 60;
            actor->motionState = 3;
        }
        break;
    }
    case 3:
        SceneMover_ApproachScreenX(input, actor->targetX);
        actor->jitter = RandomValue() & 1;
        ActorDisplay_UpdateScreenAnchor(input);
        ActorDisplay_UpdateForegroundSprite(input);
        ActorDisplay_UpdateAccentSprite(input);
        SceneActors_EmitPeriodicParticles(input, time);
        actor->foreground.mode = 0;
        actor->foreground.sequenceId = 3;
        actor->foreground.sequence.flags |= 4;
        Anim_RestartSequence(&actor->foreground.sequence, actor->sequenceResource,
              (u16)actor->foreground.sequenceId, 0, 0);
        actor->motionState = 4;
        actor->targetX = 60;
        break;
    case 4:
        SceneMover_ApproachScreenX(input, actor->targetX);
        actor->foreground.sequence.flags |= 4;
        actor->jitter = RandomValue() & 1;
        ActorDisplay_UpdateScreenAnchor(input);
        ActorDisplay_UpdateForegroundSprite(input);
        ActorDisplay_UpdateAccentSprite(input);
        SceneActors_EmitPeriodicParticles(input, time);
        if (actor->foreground.ended) {
            actor->foreground.sequence.flags &= ~4;
            input->finished = 1;
        }
        break;
    }
}
